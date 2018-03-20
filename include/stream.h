#ifndef CPP_STREAM_STREAM_STREAM_H
#define CPP_STREAM_STREAM_STREAM_H

#include <initializer_list> // // ::std::initializer_list
#include <utility>          // ::std::move, ::std::forward, ::std::declval, ::std::ref
#include <memory>           // ::std::addressof
#include <type_traits>      // ::std::remove_reference, ::std::result_of_t, ::std::enable_if_t, ::std::is_base_of
#include <iterator>         // ::std::iterator_traits
#include <cstddef>          // ::std::size_t
#include <vector>           // ::std::vector

namespace stream
{
    namespace detail
    {
        template<typename... Args>
        struct MakeVoid
        {
            using Type = void;
        };

        template<typename... Args>
        using VoidT = typename MakeVoid<Args...>::Type;


        template<typename Callable>
        struct AutoInvokableFunctor
        {
            using ResultType = ::std::result_of_t<Callable()>;

            Callable callable;
            bool isEmpty;


            AutoInvokableFunctor(const Callable &callable)
                : callable(callable), isEmpty(false)
            {
            }

            AutoInvokableFunctor(Callable &&callable)
                : callable(::std::move(callable)), isEmpty(false)
            {
            }


            ~AutoInvokableFunctor()
            {
                if (!isEmpty)
                {
                    (void)callable();
                }
            }


            AutoInvokableFunctor(const AutoInvokableFunctor &) = default;

            AutoInvokableFunctor(AutoInvokableFunctor &&src)
                : callable(::std::move(src.callable)), isEmpty(src.isEmpty)
            {
                src.isEmpty = true;
            }


            AutoInvokableFunctor& operator=(const AutoInvokableFunctor &) = default;

            AutoInvokableFunctor& operator=(AutoInvokableFunctor &&right)
            {
                if (this != ::std::addressof(right))
                {
                    isEmpty = right.isEmpty;
                    right.isEmpty = true;
                    callable = ::std::move(right.callable);
                }

                return *this;
            }


            decltype(auto) operator()()
            {
                isEmpty = true;
                return callable();
            }


            operator ResultType()
            {
                return this->operator()();
            }


            template<typename ThenCallable>
            auto operator|(ThenCallable &&thenCallable)
            {
                auto lambda = [before = ::std::move(*this), after = ::std::forward<ThenCallable>(thenCallable)]() mutable {
                    (void)before();
                    return after();
                };

                return AutoInvokableFunctor<decltype(lambda)>(::std::move(lambda));
            }
        };


        template<typename T, typename StreamImpl>
        class StreamBase
        {
        public:
            using value_type = T;


            template<typename Callable>
            auto operator|(Callable &&callable)
            {
                auto lambda = [self = ::std::ref(*static_cast<StreamImpl>(this)), callable = ::std::forward<Callable>(callable)]() mutable {
                    return callable(self.get());
                };

                return AutoInvokableFunctor<decltype(lambda)>(::std::move(lambda));
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
        class Stream<VoidT<::std::result_of_t<Generator()>>,
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
            {
            }


            Stream(::std::initializer_list<T> initList)
                : container(initList)
            {
            }


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


    template<typename... Args>
    struct Stream : public detail::Stream<void, Stream<Args...>, Args...>
    {
        using detail::Stream<void, Stream<Args...>, Args...>::Stream;
    };


    template<typename Iterator>
    Stream(Iterator, Iterator) -> Stream<typename ::std::iterator_traits<Iterator>::value_type, Iterator>;

    template<typename Range>
    Stream(const Range&) -> Stream<typename Range::value_type, typename Range::const_iterator>;


    template<typename Container>
    Stream(Container&&) -> Stream<typename Container::value_type, Container>;


    template<typename T>
    Stream(::std::initializer_list<T>) -> Stream<T, ::std::vector<T>>;

    template<typename T, typename... Ts>
    Stream(T&&, Ts&&...) -> Stream< ::std::remove_reference_t<T>, ::std::vector<::std::remove_reference_t<T>> >;

    /*auto get(::std::size_t n)
    {
        return [n](auto &stream) mutable {
            using type = typename ::std::remove_reference_t<decltype(stream)>::value_type;

            ::std::vector<type> result;
            result.reserve(n);

            auto iter = stream.begin();
            while (n > 0)
            {
                result.emplace_back(*iter++);
                --n;
            }

            return Stream<type>(::std::move(result));
        };
    }*/
}

#endif //CPP_STREAM_STREAM_STREAM_H
