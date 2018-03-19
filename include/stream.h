#ifndef CPP_STREAM_STREAM_STREAM_H
#define CPP_STREAM_STREAM_STREAM_H

#include <initializer_list> // // ::std::initializer_list
#include <utility>          // ::std::move
#include <memory>           // ::std::addressof
#include <type_traits>      // ::std::remove_reference

namespace stream
{
    namespace detail
    {
        template<typename Callable>
        struct AutoInvokableFunctor
        {
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


            template<typename ThenCallable>
            auto then(ThenCallable &&thenCallable)
            {
                auto lambda = [before = ::std::move(*this), after = ::std::forward<ThenCallable>(thenCallable)]() mutable {
                    (void)before();
                    return after();
                };

                return AutoInvokableFunctor<decltype(lambda)>(::std::move(lambda));
            }
        };
    }


    template<typename T>
    class Stream
    {
    private:

    };
}

#endif //CPP_STREAM_STREAM_STREAM_H