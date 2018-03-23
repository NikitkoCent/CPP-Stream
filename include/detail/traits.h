#ifndef CPP_STREAM_DETAIL_TRAITS_H
#define CPP_STREAM_DETAIL_TRAITS_H

#include "utility.h"    // VoidT
#include <type_traits>  // ::std::result_of_t, ::std::invoke_result_t, ::std::enable_if_t
#include <utility>      // ::std::declval
#include <iterator>     // ::std::input_iterator_tag, ::std::iterator_traits

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


        template<typename AlwaysVoid, typename F, typename... Args>
        struct IsInvokableImpl : ::std::false_type
        {};

        template<typename F, typename... Args>
        struct IsInvokableImpl<VoidT<decltype(::std::declval<F>()(::std::declval<Args>()...))>, F, Args...> : ::std::true_type
        {};

        template<typename F, typename... Args>
        struct IsInvokable : IsInvokableImpl<void, F, Args...>
        {};


        template<typename C, typename = void>
        struct ContainerTraits
        {
            static constexpr bool IsContainer = false;
        };

        template<typename C>
        struct ContainerTraits<C, VoidT<decltype(::std::declval<C>().begin()),
                                        decltype(::std::declval<C>().end()),
                                        ::std::enable_if_t<::std::is_base_of<::std::input_iterator_tag,
                                                                             typename ::std::iterator_traits<typename C::const_iterator>::iterator_category>::value> >
                              >
        {
            static constexpr bool IsContainer = true;
            using Iterator = typename C::const_iterator;
            using ValueType = typename C::value_type;
        };

        template<typename C>
        struct ContainerTraits<C&> : ContainerTraits<C>
        {};

        template<typename C>
        struct ContainerTraits<C&&> : ContainerTraits<C>
        {};
    }
}

#endif //CPP_STREAM_DETAIL_TRAITS_H
