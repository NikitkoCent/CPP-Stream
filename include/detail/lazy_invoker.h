#ifndef CPP_STREAM_DETAIL_LAZY_INVOKER_H
#define CPP_STREAM_DETAIL_LAZY_INVOKER_H

#include "utility.h"
#include <type_traits>  // ::std::decay_t
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
        template<typename, typename, bool>
        class StreamBase;


        template<typename Callable>
        class LazyInvoker
        {
        public:
            template<typename F>
            LazyInvoker(F &&callable)
                : callable(::std::forward<F>(callable)), isEmpty(false)
            {}


            ~LazyInvoker()
            {
                if (!isEmpty)
                {
                    (void)invoke();
                }
            }


            operator InvokeResultT<Callable&>()
            {
                return invoke();
            }


            template<typename Visitor>
            auto operator|(Visitor &&visitor)
            {
                if constexpr (detail::IsInvokable<Visitor&, decltype(invoke())>::value)
                {
                    using ReturnType = detail::InvokeResultT<Visitor&, decltype(invoke())>;

                    auto lambda = [now = ::std::move(*this), visitor = ::std::forward<Visitor>(visitor)]() mutable
                        -> ReturnType {
                        return visitor(now.invoke());
                    };

                    return LazyInvoker<decltype(lambda)>(::std::move(lambda));
                }
                else
                {
                    static_assert(detail::IsInvokable<Visitor&>::value, "Invalid visitor type");
                    using ReturnType = detail::InvokeResultT<Visitor&>;

                    auto lambda = [now = ::std::move(*this), visitor = ::std::forward<Visitor>(visitor)]() mutable
                        -> ReturnType {
                        (void)now.invoke();
                        return visitor();
                    };

                    return LazyInvoker<decltype(lambda)>(::std::move(lambda));
                }
            };


        private:
            template<typename, typename>
            friend struct stream::Stream;

            template<typename>
            friend class LazyInvoker;

            template<typename, typename, bool>
            friend class StreamBase;


            ::std::decay_t<Callable> callable;
            bool isEmpty;


            LazyInvoker(const LazyInvoker &) = delete;
            LazyInvoker& operator=(const LazyInvoker &) = delete;

            LazyInvoker(LazyInvoker &&src)
                : callable(::std::move(src.callable)), isEmpty(src.isEmpty)
            {
                src.isEmpty = true;
            }

            LazyInvoker& operator=(LazyInvoker &&right)
            {
                if (this != ::std::addressof(right))
                {
                    isEmpty = right.isEmpty;
                    right.isEmpty = true;
                    callable = ::std::move(right.callable);
                }

                return *this;
            }


            InvokeResultT<Callable&> invoke()
            {
                if (isEmpty)
                {
                    throw BadInvokeException{};
                }

                isEmpty = true;
                return callable();
            }
        };
    }
}

#endif //CPP_STREAM_DETAIL_LAZY_INVOKER_H
