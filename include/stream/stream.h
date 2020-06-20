#ifndef CPP_STREAM_STREAM_H
#define CPP_STREAM_STREAM_H

#include "detail/stream_impl.h"
#include "traits.h"
#include <vector>               // ::std::vector
#include <type_traits>          // ::std::remove_cv_t, ::std::remove_reference_t

namespace stream
{
    template<typename T, typename Source = ::std::vector<::std::remove_cv_t<T>>>
    struct Stream : detail::StreamImpl<T, Source, Stream<T, Source>>
    {
        template<typename, bool, typename>
        friend struct detail::StreamBase;

        template<typename, typename, typename, typename>
        friend class detail::StreamImpl;


        using detail::StreamImpl<T, Source, Stream<T, Source>>::StreamImpl;
    };


    //namespace detail
    //{
    //    template<typename... Ts>
    //    struct CtadDeductorParams {};

    //    template<typename CDP, typename = void>
    //    struct CtadDeductor;


    //    // Container guide for rvalue reference
    //    template<typename Container>
    //    struct CtadDeductor<
    //                         CtadDeductorParams< Container&& >,
    //                         VoidT< ::std::enable_if_t< ::std::is_rvalue_reference_v<Container&&> >,
    //                                ::std::enable_if_t< IsContainerV<Container&&> >
    //                              >
    //                       >
    //    {
    //        using Type = Stream< typename ContainerTraits<Container>::ValueType, ::std::remove_reference_t<Container> >;
    //    };

    //    // Container guide for ::std::initializer_list
    //    template<typename T>
    //    struct CtadDeductor<
    //                         CtadDeductorParams< ::std::initializer_list<T> >,
    //                         VoidT<>
    //                       >
    //    {
    //        using Type = Stream<T>;
    //    };

    //    // Container guide for values pack
    //    template<typename T, typename... Ts>
    //    struct CtadDeductor<
    //                         CtadDeductorParams< T&&, Ts&&... >
    //                       >
    //    {
    //        using Type = Stream< ::std::remove_reference_t<T&&> >;
    //    };


    //    // Container ref guide for non const lref
    //    template<typename Container>
    //    struct CtadDeductor<
    //                         CtadDeductorParams< Container& >,
    //                         VoidT< ::std::enable_if_t< IsContainerV<Container&> >
    //                              >
    //                       >
    //    {
    //        using Type = Stream<typename ContainerTraits<Container&>::ValueType, const Container&>;
    //    };

    //    // Container ref guide for const lref
    //    template<typename Container>
    //    struct CtadDeductor<
    //                         CtadDeductorParams< const Container& >,
    //                         VoidT< ::std::enable_if_t< IsContainerV<Container&> >
    //                              >
    //                       >
    //    {
    //        using Type = Stream<typename ContainerTraits<Container&>::ValueType, const Container&>;
    //    };

    //    // Container ref guide for const rref
    //    template<typename Container>
    //    struct CtadDeductor<
    //                         CtadDeductorParams< const Container&& >,
    //                         VoidT< ::std::enable_if_t< IsContainerV<Container&&> >
    //                              >
    //                       >
    //    {
    //        using Type = Stream<typename ContainerTraits<Container&&>::ValueType, const Container&>;
    //    };


    //    // Generator guide
    //    template<typename Generator>
    //    struct CtadDeductor<
    //                         CtadDeductorParams< Generator&& >,
    //                         VoidT< ::std::enable_if_t< IsGeneratorV<Generator&&> >
    //                              >
    //                       >
    //    {
    //        using Type = Stream< typename GeneratorTraits<Generator&&>::ValueType, ::std::remove_reference_t<Generator&&> >;
    //    };


    //    // Range guide
    //    template<typename Iterator1, typename Iterator2>
    //    struct CtadDeductor<
    //                         CtadDeductorParams< Iterator1&&, Iterator2&& >,
    //                         VoidT< ::std::enable_if_t< IsRangeV<Iterator1&&> >
    //                              >
    //                       >
    //    {
    //        using Type = Stream< typename RangeTraits<Iterator1&&>::ValueType, RemoveCRefT<Iterator1&&> >;
    //    };


    //    template<typename... Ts>
    //    using CtadDeductorT = typename CtadDeductor< CtadDeductorParams<Ts...> >::Type;
    //}


    // Container guide for rvalue reference
    template<typename Container,
             ::std::enable_if_t< ::std::is_rvalue_reference_v<Container&&> >* = nullptr,
             ::std::enable_if_t< IsContainerV<Container&&> >* = nullptr
    >
    Stream(Container&&) -> Stream<typename ContainerTraits<Container>::ValueType, ::std::remove_reference_t<Container>>;

    // Container guide for ::std::initializer_list
    template<typename T>
    Stream(::std::initializer_list<T>) -> Stream<T>;

    // Container guide for values pack
    template<typename T, typename... Ts>
    Stream(T&&, Ts&&...) -> Stream<::std::remove_reference_t<T>>;


    // Container ref guide for non const lref
    template<typename Container,
             ::std::enable_if_t< IsContainerV<Container&> >* = nullptr
    >
    Stream(Container&) -> Stream<typename ContainerTraits<Container&>::ValueType, const Container&>;

    // Container ref guide for const lref
    template<typename Container,
             ::std::enable_if_t< IsContainerV<Container&> >* = nullptr
    >
    Stream(const Container&) -> Stream<typename ContainerTraits<Container&>::ValueType, const Container&>;

    // Container ref guide for const rref
    template<typename Container,
             ::std::enable_if_t< IsContainerV<Container&&> >* = nullptr
    >
    Stream(const Container&&) -> Stream<typename ContainerTraits<Container&&>::ValueType, const Container&>;


    // Generator guide
    template<typename Generator,
             ::std::enable_if_t< IsGeneratorV<Generator&&> >* = nullptr
    >
    Stream(Generator&&) -> Stream< typename GeneratorTraits<Generator&&>::ValueType, ::std::remove_reference_t<Generator&&> >;


    // Range guide
    template<typename Iterator1, typename Iterator2,
             ::std::enable_if_t< IsRangeV<Iterator1&&> >* = nullptr
    >
    Stream(Iterator1&&, Iterator2&&) -> Stream< typename RangeTraits<Iterator1&&>::ValueType, RemoveCRefT<Iterator1&&> >;

    //// Container guides
    //template<typename Container>
    //Stream(Container&&) -> Stream<typename ContainerTraits<Container>::ValueType, ::std::remove_reference_t<Container>>;

    //template<typename T>
    //Stream(::std::initializer_list<T>) -> Stream<T>;

    //template<typename T, typename... Ts>
    //Stream(T&&, Ts&&...) -> Stream<::std::remove_reference_t<T>>;


    //// Container ref guides
    //template<typename Container>
    //Stream(Container&) -> Stream<typename ContainerTraits<Container>::ValueType, const Container&>;

    //template<typename Container>
    //Stream(const Container&) -> Stream<typename ContainerTraits<Container>::ValueType, const Container&>;

    //template<typename Container>
    //Stream(const Container&&) -> Stream<typename ContainerTraits<Container>::ValueType, const Container&>;


    //// Generator guides
    //template<typename Generator>
    //Stream(Generator&&) -> Stream<typename GeneratorTraits<Generator>::ValueType, ::std::remove_reference_t<Generator>>;


    //// Range guides
    //template<typename Iterator1, typename Iterator2>
    //Stream(Iterator1&&, Iterator2&&) -> Stream<typename RangeTraits<Iterator1>::ValueType,
    //                                           RemoveCRefT<Iterator1>>;
}

#endif //CPP_STREAM_STREAM_H
