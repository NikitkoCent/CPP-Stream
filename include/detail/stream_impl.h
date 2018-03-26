#ifndef CPP_STREAM_DETAIL_STREAM_H
#define CPP_STREAM_DETAIL_STREAM_H

#include "stream_base.h"        // StreamBase
#include "utility.h"            // VoidT
#include "traits_impl.h"        // ContainerTraits
#include "stream_traits_impl.h" // IsStreamFilterFor
#include "stream_filter.h"      // StreamFilter
#include <optional>             // ::std::optional
#include <utility>              // ::std::move, ::std::forward, ::std::declval
#include <type_traits>          // ::std::enable_if_t, ::std::is_reference, ::std::is_const
#include <initializer_list>     // ::std::initializer_list
#include <functional>           // ::std::reference_wrapper
#include <iterator>             // ::std::iterator_traits

namespace stream
{
    namespace detail
    {
        template<typename, typename, typename, typename = void>
        class StreamImpl;

        // Container
        template<typename T, typename Container, typename Stream>
        class StreamImpl<T, Container, Stream, VoidT< ::std::enable_if_t<ContainerTraits<Container>::IsContainer>,
                                                      ::std::enable_if_t<!::std::is_reference<Container>::value>,
                                                      ::std::enable_if_t<!::std::is_const<Container>::value> >
                        > : public StreamBase<T, true, Stream>
        {
        public:
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

        protected:
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

        private:
            Container container;
            mutable typename ContainerTraits<Container>::Iterator iterator;
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


        // Container cref
        template<typename T, typename Container, typename Stream>
        class StreamImpl<T, const Container&, Stream, VoidT<::std::enable_if_t<ContainerTraits<Container>::IsContainer>>
                        > : public StreamBase<T, true, Stream>
        {
        public:
            StreamImpl(const Container &container)
                : container(container)
            {}

        protected:
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

        private:
            ::std::reference_wrapper<const Container> container;
            mutable typename ContainerTraits<Container>::Iterator iterator;
            mutable bool iteratorInitialized = false;
        };


        // Non-references generator
        template<typename T, typename Generator, typename Stream>
        class StreamImpl<T, Generator, Stream, VoidT<InvokeResultT<::std::decay_t<Generator>>,
                                                     ::std::enable_if_t<!::std::is_reference<T>::value>>
                        > : public StreamBase<T, false, Stream>
        {
        public:
            template<typename Callable>
            StreamImpl(Callable &&callable)
                : generator(::std::forward<Callable>(callable))
            {}


            ::std::optional<T> getNext()
            {
                return generator();
            }

        private:
            ::std::decay_t<Generator> generator;
        };

        // References generator
        template<typename T, typename Generator, typename Stream>
        class StreamImpl<T, Generator, Stream, VoidT< InvokeResultT<::std::decay_t<Generator>>,
                                                      ::std::enable_if_t<::std::is_reference<T>::value>,
                                                      ::std::enable_if_t<::std::is_lvalue_reference<InvokeResultT<::std::decay_t<Generator>>>::value> >
                        > : public StreamBase<T, false, Stream>
        {
        public:
            template<typename Callable>
            StreamImpl(Callable &&callable)
                : generator(::std::forward<Callable>(callable))
            {}


            ::std::optional<::std::reference_wrapper<::std::remove_reference_t<T>>> getNext()
            {
                return generator();
            }

        private:
            ::std::decay_t<Generator> generator;
        };


        // Range
        template<typename T, typename Iterator, typename Stream>
        class StreamImpl<T, Iterator, Stream,
                         VoidT<::std::enable_if_t<::std::is_base_of<::std::input_iterator_tag,
                                                                typename ::std::iterator_traits<Iterator>::iterator_category>::value>>
                        > : public StreamBase<T, true, Stream>
        {
        public:
            template<typename B, typename E>
            StreamImpl(B &&begin, E &&end)
                : begin(::std::forward<B>(begin)), end(::std::forward<E>(end))
            {}


            ::std::optional<T> getNext()
            {
                if (isEndImpl())
                {
                    return { ::std::nullopt };
                }

                return *begin++;
            }

        protected:
            bool isEndImpl() const
            {
                return (begin == end);
            }

        private:
            Iterator begin;
            Iterator end;
        };


        // StreamFilter
        template<typename T, typename S, typename Filter, typename Stream>
        class StreamImpl<T, StreamFilter<S, Filter>, Stream,
                         VoidT<::std::enable_if_t<IsStreamFilterFor<Filter, S>::value>>
                        > : public StreamFilter<S, Filter, Stream>
        {
        public:
            using StreamFilter<S, Filter, Stream>::StreamFilter;
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_H
