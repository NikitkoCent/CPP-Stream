#ifndef CPP_STREAM_STREAM_H
#define CPP_STREAM_STREAM_H

#include "detail/stream.h"
#include <initializer_list> // ::std::initializer_list
#include <type_traits>      // ::std::remove_reference_t
#include <iterator>         // ::std::iterator_traits
#include <vector>           // ::std::vector

namespace stream
{
    template<typename T, typename Source>
    struct Stream : detail::Stream<void, Stream<T, Source>, T, Source>
    {
        using detail::Stream<void, Stream<T, Source>, T, Source>::Stream;
    };


    template<typename Iterator>
    Stream(Iterator, Iterator) -> Stream<typename ::std::iterator_traits<Iterator>::value_type, Iterator>;

    template<typename Range>
    Stream(const Range&) -> Stream<typename Range::value_type, typename Range::const_iterator>;


    template<typename Container>
    Stream(Container&&) -> Stream<typename Container::value_type, Container>;


    template<typename T>
    Stream(::std::initializer_list<T>) -> Stream<T, ::std::vector<T>>;

    template<typename T, typename... Ts>
    Stream(T&&, Ts&&...) -> Stream< ::std::remove_reference_t<T>, ::std::vector<::std::remove_reference_t<T>> >;
}

#endif //CPP_STREAM_STREAM_H
