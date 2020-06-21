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


    namespace detail
    {
        //template<typename... Ts>
        //struct DeductorParams {};


        //template<typename T, typename = void>
        //struct Deductor;

        //// Stream(Container&&) -> Stream<T, Container>
        //template<typename Container>
        //struct Deductor<
        //                 DeductorParams<Container&&>,
        //                 detail::VoidT<
        //                                ::std::enable_if_t< !detail::IsGeneratorV<Container&&> >,
        //                                ::std::enable_if_t<  detail::IsContainerV<Container&&> >,
        //                                ::std::enable_if_t< ::std::is_rvalue_reference_v<Container&&> >,
        //                                ::std::enable_if_t< !::std::is_const_v< ::std::remove_reference_t<Container&&> > >
        //                              >
        //               >
        //{
        //    using first_type = typename ContainerTraits<Container&&>::ValueType;
        //    using second_type = RemoveCVRefT<Container&&>;
        //};

        //// Stream(T&&, Ts&&...) -> Stream<T>
        //template<typename T, typename FakeTForMinPriority, typename... Ts>
        //struct Deductor<
        //                 DeductorParams<T, Ts...>,
        //                 FakeTForMinPriority
        //               >
        //{
        //    using first_type = RemoveCVRefT<T>;
        //    using second_type = ::std::vector< RemoveCVRefT<T> >;
        //};

        //// Stream(Container&) -> Stream<T, const Container&>
        //template<typename Container>
        //struct Deductor<
        //                 DeductorParams<Container&&>,
        //                 detail::VoidT<
        //                                ::std::enable_if_t< !detail::IsGeneratorV<Container&&> >,
        //                                ::std::enable_if_t<  detail::IsContainerV<Container&&> >,
        //                                ::std::enable_if_t< !::std::is_rvalue_reference_v<Container&&> ||
        //                                                    ::std::is_const_v< ::std::remove_reference_t<Container&&> > >
        //                              >
        //               >
        //{
        //    using first_type = typename ContainerTraits<Container&&>::ValueType;
        //    using second_type = ::std::add_lvalue_reference_t< ::std::add_const_t< ::std::remove_reference_t<Container&&> > >;
        //};

        //// Stream(Generator&&) -> Stream<T, Generator>
        //template<typename Generator>
        //struct Deductor<
        //                 DeductorParams<Generator&&>,
        //                 detail::VoidT<
        //                                ::std::enable_if_t<  detail::IsGeneratorV<Generator&&> >,
        //                                ::std::enable_if_t< !detail::IsContainerV<Generator&&> >
        //                              >
        //               >
        //{
        //    using first_type = typename GeneratorTraits<Generator&&>::ValueType;
        //    using second_type = ::std::remove_reference_t<Generator&&>;
        //};

        //// Stream(Iterator1&&, Iterator2&&) -> Stream<T, Iterator1>
        //template<typename Iterator1, typename Iterator2>
        //struct Deductor<
        //                 DeductorParams<Iterator1&&, Iterator2&&>,
        //                 detail::VoidT<
        //                                ::std::enable_if_t< detail::IsRangeV<Iterator1&&> >,
        //                                ::std::enable_if_t< detail::IsRangeV<Iterator2&&> >
        //                              >
        //               >
        //{
        //    using first_type = typename RangeTraits<Iterator1&&>::ValueType;
        //    using second_type = RemoveCRefT<Iterator1&&>;
        //};


        //template<typename... Ts>
        //using Deductor1stT = typename Deductor< DeductorParams<Ts&&...> >::first_type;

        //template<typename... Ts>
        //using Deductor2ndT = typename Deductor< DeductorParams<Ts&&...> >::second_type;


        template<typename... Ts>
        struct DeductorParams {};


        template<typename T, typename = void>
        struct Deductor;

        // Stream(Container) -> Stream<T, Container>
        template<typename Container>
        struct Deductor<
                         DeductorParams<Container>,
                         detail::VoidT<
                                        ::std::enable_if_t< !detail::IsGeneratorV<Container> >,
                                        ::std::enable_if_t<  detail::IsContainerV<Container> >,
                                        ::std::enable_if_t< !::std::is_lvalue_reference_v<Container> >,
                                        ::std::enable_if_t< !::std::is_reference_v<Container> || !::std::is_const_v< ::std::remove_reference_t<Container> > >
                                      >
                       >
        {
            using first_type = typename ContainerTraits<Container>::ValueType;
            using second_type = RemoveCVRefT<Container>;
        };

        // Stream(T, Ts...) -> Stream<T>
        template<typename T, typename FakeTForMinPriority, typename... Ts>
        struct Deductor<
                         DeductorParams<T, Ts...>,
                         FakeTForMinPriority
                       >
        {
            using first_type = RemoveCVRefT<T>;
            using second_type = ::std::vector< RemoveCVRefT<T> >;
        };

        // Stream(Container&) -> Stream<T, const Container&>
        template<typename Container>
        struct Deductor<
                         DeductorParams<Container>,
                         detail::VoidT<
                                        ::std::enable_if_t< !detail::IsGeneratorV<Container> >,
                                        ::std::enable_if_t<  detail::IsContainerV<Container> >,
                                        ::std::enable_if_t< ::std::is_lvalue_reference_v<Container> ||
                                                            ( ::std::is_reference_v<Container> &&   
                                                              ::std::is_const_v< ::std::remove_reference_t<Container> >) >
                                      >
                       >
        {
            using first_type = typename ContainerTraits<Container>::ValueType;
            using second_type = ::std::add_lvalue_reference_t< ::std::add_const_t< ::std::remove_reference_t<Container> > >;
        };

        // Stream(Generator&&) -> Stream<T, Generator>
        template<typename Generator>
        struct Deductor<
                         DeductorParams<Generator>,
                         detail::VoidT<
                                        ::std::enable_if_t<  detail::IsGeneratorV<Generator> >,
                                        ::std::enable_if_t< !detail::IsContainerV<Generator> >
                                      >
                       >
        {
            using first_type = typename GeneratorTraits<Generator>::ValueType;
            using second_type = ::std::remove_reference_t<Generator>;
        };

        // Stream(Iterator1&&, Iterator2&&) -> Stream<T, Iterator1>
        template<typename Iterator1, typename Iterator2>
        struct Deductor<
                         DeductorParams<Iterator1, Iterator2>,
                         detail::VoidT<
                                        ::std::enable_if_t< detail::IsRangeV<Iterator1> >,
                                        ::std::enable_if_t< detail::IsRangeV<Iterator2> >
                                      >
                       >
        {
            using first_type = typename RangeTraits<Iterator1>::ValueType;
            using second_type = RemoveCRefT<Iterator1>;
        };


        template<typename... Ts>
        using Deductor1stT = typename Deductor< DeductorParams<Ts...> >::first_type;

        template<typename... Ts>
        using Deductor2ndT = typename Deductor< DeductorParams<Ts...> >::second_type;
    }


    template<typename T>
    Stream(::std::initializer_list<T>) -> Stream<T>;

    template<typename T, typename... Ts>
    Stream(T&&, Ts&&...) -> Stream< detail::Deductor1stT<T&&, Ts&&...>, detail::Deductor2ndT<T&&, Ts&&...> >;

    //// Container guides
    //template<typename Container>
    //Stream(Container&&) -> Stream<typename ContainerTraits<Container>::ValueType, ::std::remove_reference_t<Container>>;

    //template<typename T>
    //Stream(::std::initializer_list<T>) -> Stream<T>;

    //template<typename T, typename... Ts>
    //Stream(T&&, Ts&&...) -> Stream<::std::remove_reference_t<T>>;


    //// Container cref guides
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
