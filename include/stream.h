#ifndef CPP_STREAM_STREAM_H
#define CPP_STREAM_STREAM_H

#include "detail/stream_impl.h"
#include <vector>               // ::std::vector
#include <type_traits>          // ::std::remove_cv_t

namespace stream
{
    template<typename T, typename Source = ::std::vector<::std::remove_cv_t<T>>>
    struct Stream : detail::StreamImpl<T, Source, Stream<T, Source>>
    {
        template<typename, bool, typename>
        friend class detail::StreamBase;

        template<typename, typename, typename, typename>
        friend class detail::StreamImpl;


        using detail::StreamImpl<T, Source, Stream<T, Source>>::StreamImpl;
    };
}

#endif //CPP_STREAM_STREAM_H
