#ifndef CPP_STREAM_DETAIL_STREAM_IMPL_H
#define CPP_STREAM_DETAIL_STREAM_IMPL_H

#include "stream_base.h"        // StreamBase
#include "continuation_impl.h"  // Continuation
#include "traits_impl.h"        // VoidT, ContainerTraits, IsContainerV, IsGeneratorV, IsRangeV
#include <type_traits>          // ::std::remove_reference_t, ::std::enable_if_t, ::std::is_reference, ::std::decay_t
#include <utility>              // ::std::move, ::std::forward
#include <initializer_list>     // ::std::initializer_list
#include <functional>           // ::std::reference_wrapper

namespace stream
{
    namespace detail
    {
        template<typename T, typename Source, typename Derived, typename = void>
        class StreamImpl;


        // Container (non-reference)
        template<typename T, typename Container, typename Derived>
        class StreamImpl<T,
                         Container,
                         Derived,
                         VoidT<::std::enable_if_t<IsContainerV<Container>>,
                               ::std::enable_if_t<!::std::is_reference<Container>::value>>
                        > : public StreamBase<T, true, Derived>
        {
        public:
            using RealType = stream::ValueHolder<CopyCVT<typename ContainerTraits<Container>::ReferenceType,
                                                         typename StreamBase<T, true, Derived>::ValueType&>>;

            StreamImpl()
            {}

            StreamImpl(Container &&container)
                : container(::std::move(container))
            {}

            StreamImpl(::std::initializer_list<T> initList)
                : container(initList)
            {}

            template<typename Arg1, typename... Args>
            StreamImpl(Arg1 &&arg1, Args&&... args)
            {
                initialize(::std::forward<Arg1>(arg1), ::std::forward<Args>(args)...);
            }


            ::std::optional<RealType> getNext()
            {
                if (isEndImpl())
                {
                    return ::std::nullopt;
                }

                return *it++;
            }

        protected:
            bool isEndImpl() const
            {
                if (!iteratorInitialized)
                {
                    iteratorInitialized = true;
                    it = container.begin();
                }

                return (it == container.end());
            }

        private:
            mutable Container container;
            mutable typename ContainerTraits<Container>::Iterator it;
            mutable bool iteratorInitialized = false;


            template<typename Arg1>
            void initialize(Arg1 &&arg1)
            {
                container.emplace_back(::std::forward<Arg1>(arg1));
            }

            template<typename Arg1, typename... Args>
            void initialize(Arg1 &&arg1, Args&&... args)
            {
                container.emplace_back(::std::forward<Arg1>(arg1));
                initialize(::std::forward<Args>(args)...);
            }
        };


        // Container reference
        template<typename T, typename ContainerRef, typename Derived>
        class StreamImpl<T,
                         ContainerRef,
                         Derived,
                         VoidT<::std::enable_if_t<IsContainerV<ContainerRef>>,
                               ::std::enable_if_t<::std::is_reference<ContainerRef>::value>>
                        > : public StreamBase<T, true, Derived>
        {
        public:
            static_assert(!::std::is_rvalue_reference<ContainerRef>::value, "RValue references isn't allowed");

            using RealType = stream::ValueHolder<CopyCVT<typename ContainerTraits<ContainerRef>::ReferenceType,
                                                         typename StreamBase<T, true, Derived>::ValueType&>>;


            StreamImpl(ContainerRef containerRef)
                : containerRef(containerRef)
            {}


            ::std::optional<RealType> getNext()
            {
                if (isEndImpl())
                {
                    return ::std::nullopt;
                }

                return *it++;
            }

        protected:
            bool isEndImpl() const
            {
                if (!iteratorInitialized)
                {
                    iteratorInitialized = true;
                    it = containerRef.get().begin();
                }

                return (it == containerRef.get().end());
            }

        private:
            ::std::reference_wrapper<::std::remove_reference_t<ContainerRef>> containerRef;
            mutable typename ContainerTraits<ContainerRef>::Iterator it;
            mutable bool iteratorInitialized = false;
        };


        // Generator
        template<typename T, typename Generator, typename Derived>
        class StreamImpl<T,
                        Generator,
                        Derived,
                        VoidT<::std::enable_if_t<IsGeneratorV<Generator>>>
                        > : public StreamBase<T, false, Derived>
        {
        public:
            using RealType = ValueHolder<typename GeneratorTraits<Generator>::ValueType>;


            template<typename Callable>
            StreamImpl(Callable &&callable)
                : generator(::std::forward<Callable>(callable))
            {}


            ::std::optional<RealType> getNext()
            {
                return generator();
            }

        private:
            ::std::decay_t<Generator> generator;
        };


        // Range
        template<typename T, typename Iterator, typename Derived>
        class StreamImpl<T,
                         Iterator,
                         Derived,
                         VoidT<::std::enable_if_t<IsRangeV<Iterator>>>
                        > : public StreamBase<T, true, Derived>
        {
        public:
            using RealType = ValueHolder<typename RangeTraits<Iterator>::ValueType>;


            template<typename B, typename E>
            StreamImpl(B &&rangeBegin, E &&rangeEnd)
                : rangeBegin(::std::forward<B>(rangeBegin)), rangeEnd(::std::forward<E>(rangeEnd))
            {}

            ::std::optional<RealType> getNext()
            {
                if (isEndImpl())
                {
                    return ::std::nullopt;
                }

                return ::std::move(*rangeBegin++);
            }

        protected:
            bool isEndImpl() const
            {
                return (rangeBegin == rangeEnd);
            }

        private:
            Iterator rangeBegin;
            Iterator rangeEnd;
        };


        // Combined
        template<typename OldStream, typename Contin>
        struct CombinedStreamTag;

        template<typename OldStream, typename F, bool ManagesFiniteness>
        struct CombinedStreamTag<OldStream, Continuation<F, ManagesFiniteness>>
        {};

        #define CS_PARENT StreamBase<T, StreamTraits<OldStream>::IsFinite | ManagesFiniteness, Derived>

        template<typename T, typename F, bool ManagesFiniteness, typename Derived, typename OldStream>
        class StreamImpl<T,
                         CombinedStreamTag<OldStream, Continuation<F, ManagesFiniteness>>,
                         Derived,
                         void
                        > : public CS_PARENT
        {
        public:
            using RealType = ValueHolder<T>;

            StreamImpl(OldStream &&oldStream, Continuation<F, ManagesFiniteness> &&continuation)
                : oldStream(::std::move(oldStream)), continuation(::std::move(continuation))
            {}


            ::std::optional<RealType> getNext()
            {
                if constexpr (CS_PARENT::IsFinite)
                {
                    if (isEndImpl())
                    {
                        return ::std::nullopt;
                    }
                }

                auto oldNext = oldStream.getNext();
                if (!oldNext)
                {
                    return ::std::nullopt;
                }

                if constexpr (ManagesFiniteness)
                {
                    auto result = continuation(::std::move(*oldNext), static_cast<const OldStream &>(oldStream), end);

                    if (!result)
                    {
                        return ::std::nullopt;
                    }

                    return ::std::move(*result);
                }
                else
                {
                    auto result = continuation(::std::move(*oldNext), static_cast<const OldStream &>(oldStream));
                    if (!result)
                    {
                        return ::std::nullopt;
                    }

                    return ::std::move(*result);
                }
            }

        protected:
            template<bool Fin = CS_PARENT::IsFinite>
            ::std::enable_if_t<Fin, bool> isEndImpl() const
            {
                if constexpr (ManagesFiniteness)
                {
                    if constexpr (StreamTraits<OldStream>::IsFinite)
                    {
                        return end | oldStream.isEnd();
                    }
                    else
                    {
                        return end;
                    }
                }
                else
                {
                    return oldStream.isEnd();
                }
            }

        private:
            OldStream oldStream;
            Continuation<F, ManagesFiniteness> continuation;
            bool end = false;
        };

        #undef CS_PARENT
    };
}

#endif //CPP_STREAM_DETAIL_STREAM_IMPL_H
