#ifndef CPP_STREAM_STREAM_DETAIL_STREAM_H
#define CPP_STREAM_STREAM_DETAIL_STREAM_H

#include "utility.h"        // VoidT, InvokeResultT
#include "lazy_invoker.h"   // LazyInvoker
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
        template<typename T, typename StreamImpl, bool Finiteness>
        class StreamBase
        {
        public:
            using value_type = const T;
            static constexpr bool IsFinite = Finiteness;


            template<typename Callable>
            auto operator|(Callable &&callable) const
            {
                using ReturnType = InvokeResultT<Callable&, const StreamImpl&>;
                auto lambda = [self = ::std::cref(static_cast<const StreamImpl&>(*this)), callable = ::std::forward<Callable>(callable)]() mutable
                    -> ReturnType {
                    return callable(self.get());
                };

                return LazyInvoker<decltype(lambda)>(::std::move(lambda));
            }


            typename StreamImpl::const_iterator begin() const
            {
                return static_cast<const StreamImpl *>(this)->begin();
            }

            typename StreamImpl::const_iterator end() const
            {
                static_assert(IsFinite, "This stream is infinite");
                return static_cast<const StreamImpl *>(this)->end();
            }
        };


        template<typename AlwaysVoid, typename StreamImpl, typename T, typename... Initializer>
        class Stream;


        // range and const-ref-container
        template<typename StreamImpl, typename T, typename Iterator>
        class Stream<VoidT<::std::enable_if_t<::std::is_base_of<::std::input_iterator_tag, typename ::std::iterator_traits<Iterator>::iterator_category>::value, Iterator>>,
                     StreamImpl,
                     T,
                     Iterator
                    > : public StreamBase<T, StreamImpl, true>
        {
        public:
            using const_iterator = Iterator;


            Stream(const_iterator begin, const_iterator end)
                : rangeBegin(begin), rangeEnd(end)
            {
            }

            template<typename Container>
            Stream(const Container &container)
                : rangeBegin(container.begin()), rangeEnd(container.end())
            {
            }


            const_iterator begin() const
            {
                return rangeBegin;
            }

            const_iterator end() const
            {
                return rangeEnd;
            }

        private:
            const_iterator rangeBegin;
            const_iterator rangeEnd;
        };


        // generator
        template<typename StreamImpl, typename T, typename Generator>
        class Stream<VoidT<InvokeResultT<::std::decay_t<Generator>>>,
                     StreamImpl,
                     T,
                     Generator
                    > : public StreamBase<T, StreamImpl, false>
        {
        public:
            class const_iterator
            {
            public:
                using difference_type = ::std::ptrdiff_t;
                using value_type = const T;
                using pointer = value_type;
                using reference = value_type;


                const_iterator() = default;


                reference operator*() const
                {
                    return parent->generator();
                };


                const_iterator& operator++()
                {
                    return *this;
                }


                bool operator==(const const_iterator &right) const
                {
                    return parent == right.parent;
                }

                bool operator!=(const const_iterator &right) const
                {
                    return !(*this == right);
                }


                const_iterator operator++(int)
                {
                    iterator result(*this);
                    ++(*this);
                    return result;
                }

            private:
                using ParentType = Stream<void, StreamImpl, T, Generator>;
                friend class Stream<void, StreamImpl, T, Generator>;


                const ParentType *parent = nullptr;


                const_iterator(const ParentType *parent)
                    : parent(parent)
                {
                }
            };


            template<typename Callable>
            Stream(Callable &&generator)
                : generator(::std::forward<Callable>(generator))
            {
            }


            const_iterator begin() const
            {
                return { this };
            }

        private:
            using GeneratorType = ::std::decay_t<Generator>;


            GeneratorType generator;
        };


        // container
        template<typename StreamImpl, typename T, typename Container>
        class Stream<VoidT<decltype(::std::declval<Container&>().begin()), decltype(::std::declval<Container&>().end()),
                           ::std::enable_if_t<!::std::is_same<Container, ::std::initializer_list<T>>::value, Container> >,
                     StreamImpl,
                     T,
                     Container
                    > : public StreamBase<T, StreamImpl, true>
        {
        public:
            using const_iterator = typename Container::const_iterator;


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


            const_iterator begin() const
            {
                return container.begin();
            }

            const_iterator end() const
            {
                return container.end();
            }

        private:
            ::std::remove_reference_t<Container> container;
        };
    }
}

#endif //CPP_STREAM_STREAM_DETAIL_STREAM_H
