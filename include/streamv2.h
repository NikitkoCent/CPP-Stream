#ifndef CPP_STREAM_STREAMV2_H
#define CPP_STREAM_STREAMV2_H

#include "detail/streamv2.h"
#include "detail/traits.h"
#include <initializer_list>     // ::std::initializer_list
#include <vector>               // ::std::vector


namespace stream
{
    template<typename T, typename Source = ::std::vector<T>>
    struct Stream : detail::Stream<T, Source, Stream<T, Source>>
    {
        using detail::Stream<T, Source, Stream<T, Source>>::Stream;
    };


    // Container guides
    template<typename Container>
    Stream(Container&&) -> Stream<typename detail::ContainerTraits<Container>::ValueType, Container>;

    template<typename T>
    Stream(::std::initializer_list<T>) -> Stream<T>;

    template<typename T, typename... Ts>
    Stream(T&&, Ts&&...) -> Stream<::std::remove_reference_t<T>>;


    // Container cref guides
    template<typename Container>
    Stream(Container&) -> Stream<typename detail::ContainerTraits<Container>::ValueType, const Container&>;

    template<typename Container>
    Stream(const Container&) -> Stream<typename detail::ContainerTraits<Container>::ValueType, const Container&>;

    template<typename Container>
    Stream(const Container&&) -> Stream<typename detail::ContainerTraits<Container>::ValueType, const Container&>;


    // Generator guides
    template<typename Generator>
    Stream(Generator&&) -> Stream<detail::InvokeResultT<::std::decay_t<Generator>>, ::std::remove_reference_t<Generator>>;
}

#endif //CPP_STREAM_STREAMV2_H
