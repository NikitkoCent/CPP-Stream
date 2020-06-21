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


    // CTAD SFINAE deduction helper
    namespace detail::ctad
    {
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
            using first_type = typename detail::ContainerTraits<Container>::ValueType;
            using second_type = detail::RemoveCVRefT<Container>;
        };

        // Stream(T, Ts...) -> Stream<T>
        template<typename T, typename FakeTForMinPriority, typename... Ts>
        struct Deductor<
                         DeductorParams<T, Ts...>,
                         FakeTForMinPriority
                       >
        {
            using first_type = detail::RemoveCVRefT<T>;
            using second_type = ::std::vector< detail::RemoveCVRefT<T> >;
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
            using first_type = typename detail::ContainerTraits<Container>::ValueType;
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
            using first_type = typename detail::GeneratorTraits<Generator>::ValueType;
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
            using first_type = typename detail::RangeTraits<Iterator1>::ValueType;
            using second_type = detail::RemoveCRefT<Iterator1>;
        };


        template<typename... Ts>
        using DeductorT1 = typename Deductor< DeductorParams<Ts...> >::first_type;

        template<typename... Ts>
        using DeductorT2 = typename Deductor< DeductorParams<Ts...> >::second_type;
    }


    template<typename T>
    Stream(::std::initializer_list<T>) -> Stream<T>;

    template<typename T, typename... Ts>
    Stream(T&&, Ts&&...) -> Stream< detail::ctad::DeductorT1<T&&, Ts&&...>, detail::ctad::DeductorT2<T&&, Ts&&...> >;
}

#endif //CPP_STREAM_STREAM_H
