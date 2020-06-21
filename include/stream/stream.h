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


    // Container guides
    template<typename Container,
             typename ::std::enable_if_t< !detail::IsGeneratorV<Container&&> >* = nullptr,
             typename ::std::enable_if_t<  detail::IsContainerV<Container&&> >* = nullptr,
             typename ::std::enable_if_t<  ::std::is_rvalue_reference_v<Container&&> >* = nullptr,
             typename ::std::enable_if_t< !::std::is_const_v< ::std::remove_reference_t<Container&&> > >* = nullptr
    >
    Stream(Container&&) -> Stream< typename ContainerTraits<Container&&>::ValueType, detail::RemoveCVRefT<Container&&> >;

    template<typename T>
    Stream(::std::initializer_list<T>) -> Stream<T>;

    template<typename T, typename... Ts>
    Stream(T&&, Ts&&...) -> Stream<::std::remove_reference_t<T>>;


    // Container ref guide
    template<typename Container,
             typename ::std::enable_if_t< !detail::IsGeneratorV<Container&&> >* = nullptr,
             typename ::std::enable_if_t<  detail::IsContainerV<Container&&> >* = nullptr,
             typename ::std::enable_if_t<  ::std::is_lvalue_reference_v<Container&&> >* = nullptr
    >
    Stream(Container&&) -> Stream< typename ContainerTraits<Container&&>::ValueType, const Container& >;


    // Generator guides
    template<typename Generator,
             typename ::std::enable_if_t<  detail::IsGeneratorV<Generator&&> >* = nullptr,
             typename ::std::enable_if_t< !detail::IsContainerV<Generator&&> >* = nullptr
    >
    Stream(Generator&&) -> Stream< typename GeneratorTraits<Generator&&>::ValueType, ::std::remove_reference_t<Generator> >;


    // Range guides
    template<typename Iterator1, typename Iterator2>
    Stream(Iterator1, Iterator2) -> Stream< typename RangeTraits<Iterator1>::ValueType, Iterator1 >;
}

#endif //CPP_STREAM_STREAM_H
