#ifndef CPP_STREAM_DETAIL_STREAM_IMPL_H
#define CPP_STREAM_DETAIL_STREAM_IMPL_H

#include "stream_base.h"    // StreamBase
#include "traits_impl.h"    // VoidT, ContainerTraits, IsContainerV, IsGeneratorV, IsRangeV
#include <type_traits>      // ::std::remove_reference_t, ::std::enable_if_t, ::std::is_reference, ::std::decay_t
#include <utility>          // ::std::move, ::std::forward
#include <initializer_list> // ::std::initializer_list
#include <functional>       // ::std::reference_wrapper

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
            using RealType = stream::ValueHolder<typename ContainerTraits<Container>::ReferenceType>;


            StreamImpl()
                : it(container.begin())
            {}

            StreamImpl(Container &&container)
                : container(::std::move(container)), it(container.begin())
            {}

            StreamImpl(::std::initializer_list<T> initList)
                : container(initList), it(container.begin())
            {}

            template<typename Arg1, typename... Args>
            StreamImpl(Arg1 &&arg1, Args&&... args)
            {
                initialize(::std::forward<Arg1>(arg1), ::std::forward<Args>(args)...);
                it = container.begin();
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
                return (it == container.end());
            }

        private:
            Container container;
            typename ContainerTraits<Container>::Iterator it;


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

            using RealType = stream::ValueHolder<typename ContainerTraits<ContainerRef>::ReferenceType>;


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
            template<typename Callable>
            StreamImpl(Callable &&callable)
                : generator(::std::forward<Callable>(callable))
            {}

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
            template<typename B, typename E>
            StreamImpl(B &&rangeBegin, E &&rangeEnd)
                : rangeBegin(::std::forward<B>(rangeBegin)), rangeEnd(::std::forward<E>(rangeEnd))
            {}

        private:
            Iterator rangeBegin;
            Iterator rangeEnd;
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_IMPL_H
