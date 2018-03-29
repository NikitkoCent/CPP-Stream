#ifndef CPP_STREAM_STREAM_H
#define CPP_STREAM_STREAM_H

#include "detail/stream_base.h"         // StreamBase
#include "detail/stream_impl.h"         // StreamImpl
#include "detail/stream_filter_impl.h"  // StreamFilter
#include "traits.h"                     // ContainerTraits
#include <initializer_list>             // ::std::initializer_list
#include <vector>                       // ::std::vector
#include <iterator>                     // ::std::iterator_traits


namespace stream
{
    template<typename T, typename Source = ::std::vector<::std::remove_cv_t<T>>>
    struct Stream : detail::StreamImpl<T, Source, Stream<T, Source>>
    {
        using detail::StreamImpl<T, Source, Stream<T, Source>>::StreamImpl;

        template<typename, bool, typename>
        friend class detail::StreamBase;

        template<typename, typename, typename, typename>
        friend class detail::StreamImpl;

        template<typename, typename, typename>
        friend class detail::StreamFilter;
    };


    // Container guides
    template<typename Container>
    Stream(Container&&) -> Stream<typename ContainerTraits<Container>::ValueType, Container>;

    template<typename T>
    Stream(::std::initializer_list<T>) -> Stream<T>;

    template<typename T, typename... Ts>
    Stream(T&&, Ts&&...) -> Stream<::std::remove_reference_t<T>>;


    // Container cref guides
    template<typename Container>
    Stream(Container&) -> Stream<typename ContainerTraits<Container>::ValueType, const Container&>;

    template<typename Container>
    Stream(const Container&) -> Stream<typename ContainerTraits<Container>::ValueType, const Container&>;

    template<typename Container>
    Stream(const Container&&) -> Stream<typename ContainerTraits<Container>::ValueType, const Container&>;


    // Generator guides
    template<typename Generator>
    Stream(Generator&&) -> Stream<detail::InvokeResultT<::std::decay_t<Generator>>, ::std::remove_reference_t<Generator>>;


    // Range guides
    template<typename Iterator1, typename Iterator2>
    Stream(Iterator1&&, Iterator2&&) -> Stream<typename ::std::iterator_traits<RemoveCRefT<Iterator1>>::value_type,
                                               RemoveCRefT<Iterator1>>;
}

#endif //CPP_STREAM_STREAM_H
