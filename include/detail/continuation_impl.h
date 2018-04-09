#ifndef CPP_STREAM_DETAIL_CONTINUATION_IMPL_H
#define CPP_STREAM_DETAIL_CONTINUATION_IMPL_H

#include <type_traits>  // ::std::decay_t
#include <utility>      // ::std::move, ::std::forward

namespace stream
{
    namespace detail
    {
        template<typename C, bool MakesFinite>
        class Continuation
        {
        public:
            template<typename F>
            Continuation(F &&continuation)
                : continuation(::std::forward<F>(continuation))
            {}

            template<typename Args>
            decltype(auto) operator()(Args&&... args)
            {
                return continuation(::std::forward<Args>(args)...);
            }

        private:
            ::std::decay_t<C> continuation;
        };
    }
}

#endif //CPP_STREAM_DETAIL_CONTINUATION_IMPL_H
