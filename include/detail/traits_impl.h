#ifndef CPP_STREAM_DETAIL_TRAITS_IMPL_H
#define CPP_STREAM_DETAIL_TRAITS_IMPL_H

#include <type_traits>  // ::std::invoke_result_t, ::std::decay_t, ::std::enable_if_t, ::std::is_same, ::std::is_base_of
#include <iterator>     // ::std::iterator_traits, ::std::forward_iterator_tag, ::std::input_iterator_tag
#include <utility>      // ::std::declval

namespace stream
{
    template<typename T, typename Source>
    struct Stream;

    template<typename T, bool Fin>
    class Continuation;

    template<typename T>
    class ValueHolder;


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
        using RemoveCRefT = ::std::remove_const_t<::std::remove_reference_t<T>>;

        template<typename T>
        using RemoveCVRefT = ::std::remove_cv_t<::std::remove_reference_t<T>>;


        // ============================================================================================================
        template<typename C, typename = void>
        struct ContainerTraitsImpl
        {
            static constexpr bool IsContainer = false;
        };

        template<typename C>
        struct ContainerTraitsImpl<C, VoidT<typename C::value_type,
                                            ::std::enable_if_t<::std::is_same<decltype(::std::declval<C>().begin()),
                                                                              decltype(::std::declval<C>().end())>::value>,
                                            ::std::enable_if_t<::std::is_base_of<::std::forward_iterator_tag,
                                                               typename ::std::iterator_traits<decltype(::std::declval<C>().begin())>::iterator_category>::value>>
                                  >
        {
            static constexpr bool IsContainer = true;
            using ValueType = typename C::value_type;
            using Iterator = decltype(::std::declval<C>().begin());
            using ReferenceType = decltype(*(::std::declval<Iterator>()));
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
        struct GeneratorTraits : GeneratorTraitsImpl<::std::remove_reference_t<G>>
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
            using ValueType = typename ::std::iterator_traits<It>::value_type;
        };

        template<typename It>
        struct RangeTraits : RangeTraitsImpl<::std::remove_reference_t<It>>
        {};

        template<typename It>
        static constexpr bool IsRangeV = RangeTraits<It>::IsRange;
        // ============================================================================================================

        // ============================================================================================================
        template<typename From, typename To>
        struct CopyCVImpl
        {
            using Type = To;
        };

        template<typename From, typename To>
        struct CopyCVImpl<const From, To> : CopyCVImpl<From, const To>
        {};

        template<typename From, typename To>
        struct CopyCVImpl<volatile From, To> : CopyCVImpl<From, volatile To>
        {};

        template<typename From, typename To>
        struct CopyCVImpl<const volatile From, To> : CopyCVImpl<From, const volatile To>
        {};


        template<typename From, typename To>
        struct CopyCV : CopyCVImpl<::std::remove_reference_t<From>, To>
        {};

        template<typename From, typename To>
        struct CopyCV<From, To&>
        {
            using Type = typename CopyCV<From, To>::Type&;
        };

        template<typename From, typename To>
        struct CopyCV<From, To&&>
        {
            using Type = typename CopyCV<From, To>::Type&&;
        };


        template<typename From, typename To>
        using CopyCVT = typename CopyCV<From, To>::Type;
        // ============================================================================================================

        // ============================================================================================================
        template<typename S>
        struct StreamTraitsImpl
        {
            static constexpr bool IsStream = false;
        };

        template<typename T, typename Source>
        struct StreamTraitsImpl<stream::Stream<T, Source>>
        {
            static constexpr bool IsStream = true;
            using ValueType = typename stream::Stream<T, Source>::ValueType;
            using RealType = typename stream::Stream<T, Source>::RealType;
            static constexpr bool IsFinite = stream::Stream<T, Source>::IsFinite;
        };

        template<typename S>
        struct StreamTraits : StreamTraitsImpl<RemoveCVRefT<S>>
        {};
        // ============================================================================================================

        // ============================================================================================================
        template<typename C, typename S, typename = void>
        struct ConsumerForTraitsImpl
        {
            static constexpr bool IsConsumer = false;
        };

        template<typename C, typename S>
        struct ConsumerForTraitsImpl<C, S, VoidT<InvokeResultT<::std::decay_t<C>, S&&>>>
        {
            static constexpr bool IsConsumer = true;
            using ValueType = InvokeResultT<::std::decay_t<C>, S&&>;
        };

        template<typename C, typename S>
        struct ConsumerForTraits : ConsumerForTraitsImpl<C, ::std::remove_reference_t<S>>
        {};

        template<typename C, typename S>
        static constexpr bool IsConsumerForV = ConsumerForTraits<C, S>::IsConsumer;
        // ============================================================================================================

        // ============================================================================================================
        template<typename C, typename S, typename = void>
        struct ContinuationInvokeResult
        {};

        template<typename C, typename S>
        struct ContinuationInvokeResult<Continuation<C, false>, S,
                                        VoidT<InvokeResultT<Continuation<C, false>,
                                                            typename StreamTraits<S>::RealType&&,
                                                            const S&>>>
        {
            using Type = InvokeResultT<Continuation<C, false>,
                                       typename StreamTraits<S>::RealType&&,
                                       const S&>;
        };

        template<typename C, typename S>
        struct ContinuationInvokeResult<Continuation<C, true>, S,
                                        VoidT<InvokeResultT<Continuation<C, true>,
                                                            typename StreamTraits<S>::RealType&&,
                                                            const S&,
                                                            bool&>>>
        {
            using Type = InvokeResultT<Continuation<C, true>,
                                       typename StreamTraits<S>::RealType&&,
                                       const S&,
                                       bool&>;
        };

        template<typename C, typename S>
        using ContinuationInvokeResultT = typename ContinuationInvokeResult<C, S>::Type;



        template<typename C, typename S, typename = void>
        struct ContinuationForTraitsImpl
        {
            static constexpr bool IsContinuation = false;
        };

        template<typename C, typename S>
        struct ContinuationForTraitsImpl<Continuation<C, false>, S,
                                         VoidT<InvokeResultT<Continuation<C, false>,
                                                             decltype(::std::declval<typename StreamTraits<S>::RealType>().get()),
                                                             const S&>>>
        {
            static constexpr bool IsContinuation = true;
            using ValueType = InvokeResultT<Continuation<C, false>,
                                            decltype(::std::declval<typename StreamTraits<S>::RealType>().get()),
                                            const S&>;
        };

        template<typename C, typename S>
        struct ContinuationForTraitsImpl<Continuation<C, false>, S,
                                         VoidT<::std::enable_if_t<::std::is_same<>::value>>
                                        >

        template<typename C, typename S>
        struct ContinuationForTraitsImpl<Continuation<C, true>, S,
                                         VoidT<InvokeResultT<Continuation<C, true>,
                                                             decltype(::std::declval<typename StreamTraits<S>::RealType>().get()),
                                                             const S&,
                                                             bool&>>>
        {
            static constexpr bool IsContinuation = true;
            using ValueType = InvokeResultT<Continuation<C, true>,
                                            decltype(::std::declval<typename StreamTraits<S>::RealType>().get()),
                                            const S&,
                                            bool&>;
        };

        template<typename C, typename Stream>
        struct ContinuationForTraits : ContinuationForTraitsImpl<::std::remove_reference_t<C>,
                                                                 ::std::remove_reference_t<Stream>>
        {};

        template<typename C, typename Stream>
        static constexpr bool IsContinuationForV = ContinuationForTraits<C, Stream>::IsContinuation;
        // ============================================================================================================

        // ============================================================================================================
        template<typename Factory, typename Stream, typename = void>
        struct ContinuationsFactoryForTraits
        {
            static constexpr bool IsFactory = false;
        };

        template<typename Factory, typename Stream>
        struct ContinuationsFactoryForTraits<Factory,
                                             Stream,
                                             VoidT<::std::enable_if_t<IsContinuationForV<decltype(::std::declval<Factory>().template createContinuation<Stream>()), Stream>>>
                                            >
        {
            static constexpr bool IsFactory = true;
        };

        template<typename Factory, typename Stream>
        static constexpr bool IsContinuationsFactoryForV = ContinuationsFactoryForTraits<Factory, Stream>::IsFactory;
        // ============================================================================================================

        // ============================================================================================================
        template<typename T>
        struct UnwrapValueHolder
        {
            using Type = T;
        };

        template<typename T>
        struct UnwrapValueHolder<ValueHolder<T>> : UnwrapValueHolder<T>
        {};

        template<typename T>
        using UnwrapValueHolderT = typename UnwrapValueHolder<T>::Type;
        // ============================================================================================================
    };
}

#endif //CPP_STREAM_DETAIL_TRAITS_IMPL_H
