#ifndef CPP_STREAM_CONTINUATION_H
#define CPP_STREAM_CONTINUATION_H

#include "detail/continuation_impl.h"

namespace stream
{
    template<bool MakesFinite, typename F>
    detail::Continuation<F, MakesFinite> makeContinuation(F &&f)
    {
        return {::std::forward<F>(f)};
    }
}

#endif //CPP_STREAM_CONTINUATION_H
