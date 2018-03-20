#ifndef CPP_STREAM_DETAIL_LAZY_INVOKER_H
#define CPP_STREAM_DETAIL_LAZY_INVOKER_H

#include "utility.h"
#include <type_traits>  // ::std::decay_t
#include <tuple>        // ::std::tuple, ::std::apply
#include <utility>      // ::std::forward, ::std::move
#include <functional>   // ::std::bad_function_call
#include <memory>       // ::std::addressof

namespace stream
{
    struct BadInvokeException : ::std::bad_function_call
    {};


    template<typename, typename>
    struct Stream;


    namespace detail
    {
        template<typename Callable, typename... Params>
        class LazyInvoker
        {
        public:
            template<typename F, typename... Args>
            LazyInvoker(F &&callable, Args&&... args)
                : callable(::std::forward<F>(callable)), args(::std::forward<Args>(args)...), isEmpty(false)
            {}


            ~LazyInvoker()
            {
                if (!isEmpty)
                {
                    (void)invoke();
                }
            }


            operator InvokeResultT<Callable&, Params...>()
            {
                return invoke();
            }


            template<typename Visitor>
            auto operator|(Visitor &&visitor)
            {
                auto lambda = [now = ::std::move(*this), visitor = ::std::forward<Visitor>(visitor)]() mutable {
                    if constexpr (::std::is_same<void, InvokeResultT<Callable&, Params...> >::value)
                    {
                        now.invoke();
                        return visitor();
                    }
                    else
                    {
                        return visitor(now.invoke());
                    }
                };

                return LazyInvoker<decltype(lambda)>(::std::move(lambda));
            };


        private:
            template<typename, typename>
            friend struct stream::Stream;

            template<typename, typename...>
            friend class LazyInvoker;


            ::std::decay_t<Callable> callable;
            ::std::tuple<Params...> args;
            bool isEmpty;


            LazyInvoker(const LazyInvoker &) = delete;
            LazyInvoker& operator=(const LazyInvoker &) = delete;

            LazyInvoker(LazyInvoker &&src)
                : callable(::std::move(src.callable)), args(::std::move(src.args)), isEmpty(src.isEmpty)
            {
                src.isEmpty = true;
            }

            LazyInvoker& operator=(LazyInvoker &&right)
            {
                if (this != ::std::addressof(right))
                {
                    isEmpty = right.isEmpty;
                    right.isEmpty = true;
                    args = ::std::move(right.args);
                    callable = ::std::move(right.callable);
                }

                return *this;
            }


            InvokeResultT<Callable&, Params...> invoke()
            {
                if (isEmpty)
                {
                    throw BadInvokeException{};
                }

                isEmpty = true;
                return ::std::apply(callable, ::std::move(args));
            }
        };
    }
}

#endif //CPP_STREAM_DETAIL_LAZY_INVOKER_H
