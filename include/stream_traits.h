#ifndef CPP_STREAM_STREAM_TRAITS_H
#define CPP_STREAM_STREAM_TRAITS_H

#include "detail/stream_traits_impl.h"

namespace stream
{
    using detail::StreamTraits;
    using detail::IsStreamV;
    using detail::StreamSourceT;
    using detail::StreamTypeT;
    using detail::StreamValueT;
    using detail::StreamFinitenessV;

    using detail::IsStreamFilterFor;
}

#endif //CPP_STREAM_STREAM_TRAITS_H
