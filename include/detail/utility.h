#ifndef CPP_STREAM_DETAIL_UTILITY_H
#define CPP_STREAM_DETAIL_UTILITY_H

#include <type_traits>  // ::std::result_of_t, ::std::invoke_result_t
#include <utility>      // ::std::declval

namespace stream
{
    namespace detail
    {
        #ifdef __cpp_lib_is_invocable
        template<typename Callable, typename... Args>
        using InvokeResultT = ::std::invoke_result_t<Callable, Args...>;
        #else
        template<typename Callable, typename... Args>
        using InvokeResultT = ::std::result_of_t<Callable(Args...)>;
        #endif


        // CWG issue #1558 workaround
        template<typename... Args>
        struct MakeVoid
        {
            using Type = void;
        };

        template<typename... Args>
        using VoidT = typename MakeVoid<Args...>::Type;


        template<typename AlwaysVoid, typename F, typename... Args>
        struct IsInvokableImpl : ::std::false_type
        {};

        template<typename F, typename... Args>
        struct IsInvokableImpl<VoidT<decltype(::std::declval<F>()(::std::declval<Args>()...))>, F, Args...> : ::std::true_type
        {};

        template<typename F, typename... Args>
        struct IsInvokable : IsInvokableImpl<void, F, Args...>
        {};
    }
}

#endif //CPP_STREAM_DETAIL_UTILITY_H
