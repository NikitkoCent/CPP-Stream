#ifndef CPP_STREAM_FILTER_H
#define CPP_STREAM_FILTER_H

#include "detail/filter_impl.h" // detail::Filter

namespace stream
{
    template<bool StreamFiniteness, typename Callable>
    auto makeFilter(Callable &&callable)
    {
        return detail::Filter<Callable, StreamFiniteness>(::std::forward<Callable>(callable));
    };
}

#endif //CPP_STREAM_FILTER_H
