#ifndef CPP_STREAM_DETAIL_LAZY_INVOKER_H
#define CPP_STREAM_DETAIL_LAZY_INVOKER_H

#include "utility.h"
#include <type_traits>
#include <tuple>
#include <utility>
#include <functional>   // ::std::bad_function_call

namespace stream
{
    struct BadInvokeException : ::std::bad_function_call
    {};

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
                    isEmpty = true;
                    (void)::std::apply(callable, ::std::move(args));
                }
            }


            operator InvokeResultT<Callable&, Params...>()
            {
                if (isEmpty)
                {
                    throw BadInvokeException{};
                }

                isEmpty = true;
                return ::std::apply(callable, ::std::move(args));
            }

            operator InvokeResultT<const Callable&, Params...>() const
            {
                if (isEmpty)
                {
                    throw BadInvokeException{};
                }

                isEmpty = true;
                return ::std::apply(callable, ::std::move(args));
            }


        private:
            ::std::decay_t<Callable> callable;
            mutable ::std::tuple<Params...> args;
            mutable bool isEmpty;
        };
    }
}

#endif //CPP_STREAM_DETAIL_LAZY_INVOKER_H
