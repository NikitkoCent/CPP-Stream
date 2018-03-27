#ifndef CPP_STREAM_FILTER_H
#define CPP_STREAM_FILTER_H

#include "detail/filter_impl.h" // detail::Filter
#include <type_traits>          // ::std::decay_t

namespace stream
{
    template<bool StreamFiniteness, typename Callable>
    auto makeFilter(Callable &&callable)
    {
        return detail::Filter<::std::decay_t<Callable>, StreamFiniteness>(::std::forward<Callable>(callable));
    };
}

#endif //CPP_STREAM_FILTER_H
