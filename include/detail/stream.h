#ifndef CPP_STREAM_STREAM_DETAIL_STREAM_H
#define CPP_STREAM_STREAM_DETAIL_STREAM_H

#include "utility.h"        // VoidT, InvokeResultT
#include <type_traits>      // ::std::enable_if_t, ::std::is_base_of, ::std::remove_reference_t, ::std::is_same
#include <utility>          // ::std::forward, ::std::move, ::std::declval
#include <functional>       // ::std::ref
#include <iterator>         // ::std::iterator_traits, ::std::input_iterator_tag
#include <cstddef>          // ::std::ptrdiff_t
#include <initializer_list> // ::std::initializer_list

namespace stream
{
    namespace detail
    {
        template<typename T, typename StreamImpl>
        class StreamBase
        {
        public:
            using value_type = T;


            /*template<typename Callable>
            auto operator|(Callable &&callable)
            {
                auto lambda = [self = ::std::ref(*static_cast<StreamImpl>(this)), callable = ::std::forward<Callable>(callable)]() mutable {
                    return callable(self.get());
                };

                return LazyCallable<decltype(lambda)>(::std::move(lambda));
            }*/
        };


        template<typename AlwaysVoid, typename StreamImpl, typename T, typename... Initializer>
        class Stream;


        // range and const-ref-container
        template<typename StreamImpl, typename T, typename Iterator>
        class Stream<VoidT<::std::enable_if_t<::std::is_base_of<::std::input_iterator_tag, typename ::std::iterator_traits<Iterator>::iterator_category>::value, Iterator>>,
                     StreamImpl,
                     T,
                     Iterator
                    > : public StreamBase<T, StreamImpl>
        {
        public:
            Stream(Iterator begin, Iterator end)
                : rangeBegin(begin), rangeEnd(end)
            {
            }

            template<typename Container>
            Stream(const Container &container)
                : rangeBegin(container.begin()), rangeEnd(container.end())
            {
            }


            Iterator begin() const
            {
                return rangeBegin;
            }

            Iterator end() const
            {
                return rangeEnd;
            }


        private:
            Iterator rangeBegin;
            Iterator rangeEnd;
        };


        // generator
        template<typename StreamImpl, typename T, typename Generator>
        class Stream<VoidT<InvokeResultT<Generator>>,
                     StreamImpl,
                     T,
                     Generator
                    > : public StreamBase<T, StreamImpl>
        {
        public:
            class iterator
            {
            public:
                using difference_type = ::std::ptrdiff_t;
                using value_type = T;
                using pointer = value_type;
                using reference = value_type;


                iterator() = default;


                reference operator*() const
                {
                    return parent->generator();
                };


                iterator& operator++()
                {
                    return *this;
                }


                bool operator==(const iterator &right) const
                {
                    return parent == right.parent;
                }

                bool operator!=(const iterator &right) const
                {
                    return !(*this == right);
                }


                iterator operator++(int)
                {
                    iterator result(*this);
                    ++(*this);
                    return result;
                }

            private:
                using ParentType = Stream<void, StreamImpl, T, Generator>;
                friend class Stream<void, StreamImpl, T, Generator>;


                ParentType *parent = nullptr;


                iterator(ParentType *parent)
                    : parent(parent)
                {
                }
            };


            template<typename Callable>
            Stream(Callable &&generator)
                : generator(::std::forward<Callable>(generator))
            {
            }


            iterator begin()
            {
                return { this };
            }

            iterator end()
            {
                return endImpl<>();
            }

        private:
            using GeneratorType = ::std::remove_reference_t<Generator>;


            GeneratorType generator;


            template<typename... Args>
            iterator endImpl()
            {
                static_assert(sizeof...(Args) > 0, "This stream is infinite");
                return { this };
            }
        };


        // container
        template<typename StreamImpl, typename T, typename Container>
        class Stream<VoidT<decltype(::std::declval<Container&>().begin()), decltype(::std::declval<Container&>().end()),
                           ::std::enable_if_t<!::std::is_same<Container, ::std::initializer_list<T>>::value, Container> >,
                     StreamImpl,
                     T,
                     Container
                    > : public StreamBase<T, StreamImpl>
        {
        public:
            Stream(Container &&container)
                : container(::std::move(container))
            {}


            Stream(::std::initializer_list<T> initList)
                : container(initList)
            {}


            template<typename T1, typename... Ts>
            Stream(T1&& t1, Ts&&... ts)
                : container{::std::forward<T1>(t1), ::std::forward<Ts>(ts)...}
            {
            }


            auto begin()
            {
                return container.begin();
            }

            auto end()
            {
                return container.end();
            }

        private:
            ::std::remove_reference_t<Container> container;
        };
    }
}

#endif //CPP_STREAM_STREAM_DETAIL_STREAM_H
