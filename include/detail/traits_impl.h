#ifndef CPP_STREAM_DETAIL_TRAITS_IMPL_H
#define CPP_STREAM_DETAIL_TRAITS_IMPL_H

#include <type_traits>  // ::std::invoke_result_t, ::std::decay_t, ::std::enable_if_t, ::std::is_same, ::std::is_base_of
#include <iterator>     // ::std::iterator_traits, ::std::forward_iterator_tag, ::std::input_iterator_tag
#include <utility>      // ::std::declval

namespace stream
{
    namespace detail
    {
        // =================================== CWG #1558 issue workaround =============================================
        template<typename...>
        struct MakeVoid
        {
            using Type = void;
        };

        template<typename... Ts>
        using VoidT = typename MakeVoid<Ts...>::Type;
        // ============================================================================================================


        template<typename Callable, typename... Args>
        using InvokeResultT = ::std::invoke_result_t<Callable, Args...>;


        template<typename T>
        using RemoveCVRefT = ::std::remove_cv_t<::std::remove_reference_t<T>>;


        // ============================================================================================================
        template<typename C, typename = void>
        struct ContainerTraitsImpl
        {
            static constexpr bool IsContainer = false;
        };

        template<typename C>
        struct ContainerTraitsImpl<C, VoidT<::std::enable_if_t<::std::is_same<decltype(::std::declval<C>().begin()),
                                                                              decltype(::std::declval<C>().end())>::value>,
                                            ::std::enable_if_t<::std::is_base_of<::std::forward_iterator_tag,
                                                               typename ::std::iterator_traits<decltype(::std::declval<C>().begin())>::iterator_category>::value>>
                                  >
        {
            static constexpr bool IsContainer = true;
            using Iterator = decltype(::std::declval<C>().begin());
        };

        template<typename C>
        struct ContainerTraits : ContainerTraitsImpl<::std::remove_reference_t<C>>
        {};

        template<typename C>
        static constexpr bool IsContainerV = ContainerTraits<C>::IsContainer;
        // ============================================================================================================

        // ============================================================================================================
        template<typename G, typename = void>
        struct GeneratorTraitsImpl
        {
            static constexpr bool IsGenerator = false;
        };

        template<typename G>
        struct GeneratorTraitsImpl<G, VoidT<InvokeResultT<::std::decay_t<G>>>>
        {
            static constexpr bool IsGenerator = true;
            using ValueType = InvokeResultT<::std::decay_t<G>>;
        };

        template<typename G>
        struct GeneratorTraits : GeneratorTraitsImpl<G>
        {};

        template<typename G>
        static constexpr bool IsGeneratorV = GeneratorTraits<G>::IsGenerator;
        // ============================================================================================================

        // ============================================================================================================
        template<typename It, typename = void>
        struct RangeTraitsImpl
        {
            static constexpr bool IsRange = false;
        };

        template<typename It>
        struct RangeTraitsImpl<It, VoidT<::std::enable_if_t<::std::is_base_of<::std::input_iterator_tag,
                                                                              typename ::std::iterator_traits<It>::iterator_category>::value>>
                              >
        {
            static constexpr bool IsRange = true;
        };

        template<typename It>
        struct RangeTraits : RangeTraitsImpl<It>
        {};

        template<typename It>
        static constexpr bool IsRangeV = RangeTraits<It>::IsRange;
        // ============================================================================================================
    };
}

#endif //CPP_STREAM_DETAIL_TRAITS_IMPL_H
