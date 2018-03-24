#ifndef CPP_STREAM_DETAIL_STREAMV2_H
#define CPP_STREAM_DETAIL_STREAMV2_H

#include "utility.h"        // VoidT
#include "traits.h"         // IsContainer,
#include <optional>         // ::std::optional
#include <utility>          // ::std::move, ::std::forward, ::std::declval
#include <type_traits>      // ::std::enable_if_t, ::std::is_reference, ::std::is_const
#include <initializer_list> // ::std::initializer_list
#include <functional>       // ::std::reference_wrapper

namespace stream
{
    namespace detail
    {
        template<typename T, typename Source, typename StreamImpl, typename = void>
        class Stream;


        template<typename T, bool Finiteness, typename StreamImpl>
        class StreamBase
        {
        public:
            using Type = T;
            static constexpr bool IsFinite = Finiteness;

        private:
            template<typename, typename, typename, typename>
            friend class Stream;


            bool isEnd() const
            {
                static_assert(IsFinite, "This stream is infinite");
                return static_cast<const StreamImpl *>(this)->isEndImpl();
            }
        };


        // Container
        template<typename T, typename Container, typename StreamImpl>
        class Stream<T, Container, StreamImpl, VoidT< ::std::enable_if_t<ContainerTraits<Container>::IsContainer>,
                                                      ::std::enable_if_t<!::std::is_reference<Container>::value>,
                                                      ::std::enable_if_t<!::std::is_const<Container>::value> >
                    > : public StreamBase<T, true, StreamImpl>
        {
        public:
            Stream(Container &&container)
                : container(::std::move(container))
            {}

            Stream(::std::initializer_list<T> initList)
                : container(initList)
            {}

            template<typename Arg1, typename... Args>
            Stream(Arg1 &&arg1, Args&&... args)
            {
                initialize(::std::forward<Arg1>(arg1), ::std::forward<Args>(args)...);
            }

        private:
            friend class StreamBase<T, true, StreamImpl>;

            template<typename, typename, typename, typename>
            friend class Stream;


            Container container;
            mutable typename ContainerTraits<Container>::Iterator iterator;
            mutable bool iteratorInitialized = false;


            ::std::optional<::std::reference_wrapper<const T>> getNext()
            {
                if (isEndImpl())
                {
                    return { ::std::nullopt };
                }

                return ::std::cref(*iterator++);
            }

            bool isEndImpl() const
            {
                if (!iteratorInitialized)
                {
                    iterator = container.begin();
                    iteratorInitialized = true;
                }

                return (iterator == container.end());
            }


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


        // Container cref
        template<typename T, typename Container, typename StreamImpl>
        class Stream<T, const Container&, StreamImpl, VoidT<::std::enable_if_t<ContainerTraits<Container>::IsContainer>>
                    > : public StreamBase<T, true, StreamImpl>
        {
        public:
            Stream(const Container &container)
                : container(container)
            {}

        private:
            friend class StreamBase<T, true, StreamImpl>;


            ::std::reference_wrapper<const Container> container;
            mutable typename ContainerTraits<Container>::Iterator iterator;
            mutable bool iteratorInitialized = false;


            ::std::optional<::std::reference_wrapper<const T>> getNext()
            {
                if (isEndImpl())
                {
                    return { ::std::nullopt };
                }

                return ::std::cref(*iterator++);
            }

            bool isEndImpl() const
            {
                if (!iteratorInitialized)
                {
                    iterator = container.get().begin();
                    iteratorInitialized = true;
                }

                return (iterator == container.get().end());
            }
        };


        // Non-references generator
        template<typename T, typename Generator, typename StreamImpl>
        class Stream<T, Generator, StreamImpl, VoidT<InvokeResultT<::std::decay_t<Generator>>,
                                                     ::std::enable_if_t<!::std::is_reference<T>::value>>
                    > : public StreamBase<T, false, StreamImpl>
        {
        public:
            template<typename Callable>
            Stream(Callable &&callable)
                : generator(::std::forward<Callable>(callable))
            {}

        private:
            ::std::decay_t<Generator> generator;


            ::std::optional<T> getNext()
            {
                return generator();
            }
        };

        // References generator
        template<typename T, typename Generator, typename StreamImpl>
        class Stream<T, Generator, StreamImpl, VoidT< InvokeResultT<::std::decay_t<Generator>>,
                                                      ::std::enable_if_t<::std::is_reference<T>::value>,
                                                      ::std::enable_if_t<::std::is_lvalue_reference<InvokeResultT<::std::decay_t<Generator>>>::value> >
                    > : public StreamBase<T, false, StreamImpl>
        {
        public:
            template<typename Callable>
            Stream(Callable &&callable)
                : generator(::std::forward<Callable>(callable))
            {}

        private:
            ::std::decay_t<Generator> generator;


            ::std::optional<::std::reference_wrapper<::std::remove_reference_t<T>>> getNext()
            {
                return generator();
            }
        };



    }
}

#endif //CPP_STREAM_DETAIL_STREAMV2_H
