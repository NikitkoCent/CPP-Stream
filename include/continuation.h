#ifndef CPP_STREAM_CONTINUATION_H
#define CPP_STREAM_CONTINUATION_H

#include "detail/continuation_impl.h"

namespace stream
{
    using detail::Continuation;

    template<bool MakesFinite, typename F>
    Continuation<F, MakesFinite> makeContinuation(F &&f)
    {
        return {::std::forward<F>(f)};
    };
}

#endif //CPP_STREAM_CONTINUATION_H
