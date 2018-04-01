#ifndef CPP_STREAM_DETAIL_TRAITS_IMPL_H
#define CPP_STREAM_DETAIL_TRAITS_IMPL_H

#include "utility.h"    // VoidT
#include <type_traits>  // ::std::result_of_t, ::std::invoke_result_t, ::std::enable_if_t, ::std::true_type, ::std::false_type
#include <utility>      // ::std::declval
#include <iterator>     // ::std::forward_iterator_tag, ::std::iterator_traits

namespace stream
{
    namespace detail
    {
        #ifdef __cpp_lib_is_invocable
        template<typename Callable, typename... Args>
        using InvokeResultT = ::std::invoke_result_t<Callable, Args...>;
        //#elif defined(__cpp_lib_result_of_sfinae)
        #else
        template<typename Callable, typename... Args>
        using InvokeResultT = ::std::result_of_t<Callable&&(Args&&...)>;
        #endif


        template<typename F, typename = void, typename... Args>
        struct IsInvokable : ::std::false_type
        {};

        template<typename F, typename... Args>
        struct IsInvokable<F, VoidT<InvokeResultT<F, Args...>>, Args...> : ::std::true_type
        {};


        template<typename T>
        using RemoveCRefT = ::std::remove_const_t<::std::remove_reference_t<T>>;

        template<typename T>
        using RemoveCVRefT = ::std::remove_cv_t<::std::remove_reference_t<T>>;


        template<typename C, typename = void>
        struct ContainerTraitsImpl
        {
            static constexpr bool IsContainer = false;
        };

        template<typename C>
        struct ContainerTraitsImpl<C, VoidT<decltype(::std::declval<C>().begin()),
                                            decltype(::std::declval<C>().end()),
                                            ::std::enable_if_t<::std::is_base_of<::std::forward_iterator_tag,
                                                                                 typename ::std::iterator_traits<typename C::iterator>::iterator_category>::value>,
                                            ::std::enable_if_t<::std::is_base_of<::std::forward_iterator_tag,
                                                                                 typename ::std::iterator_traits<typename C::const_iterator>::iterator_category>::value>>
                                  >
        {
            static constexpr bool IsContainer = true;
            using Iterator = typename C::iterator;
            using ConstIterator = typename C::const_iterator;
            using ValueType = typename C::value_type;
        };

        template<typename C>
        struct ContainerTraits : ContainerTraitsImpl<RemoveCVRefT<C>>
        {};
    }
}

#endif //CPP_STREAM_DETAIL_TRAITS_IMPL_H
