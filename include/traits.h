#ifndef CPP_STREAM_TRAITS_H
#define CPP_STREAM_TRAITS_H

#include "detail/traits_impl.h"

namespace stream
{
    using detail::RemoveCRefT;
    using detail::RemoveCVRefT;

    using detail::RangeTraits;
    using detail::IsRangeV;

    using detail::GeneratorTraits;
    using detail::IsGeneratorV;

    using detail::ContainerTraits;
    using detail::IsContainerV;

    using detail::StreamTraits;
}

#endif //CPP_STREAM_TRAITS_H
