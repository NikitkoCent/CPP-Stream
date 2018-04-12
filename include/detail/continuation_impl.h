#ifndef CPP_STREAM_DETAIL_CONTINUATION_IMPL_H
#define CPP_STREAM_DETAIL_CONTINUATION_IMPL_H

#include <type_traits>  // ::std::decay_t
#include <utility>      // ::std::move, ::std::forward

namespace stream
{
    namespace detail
    {
        template<typename C, bool ManagesFiniteness>
        class Continuation
        {
        public:
            template<typename F>
            Continuation(F &&continuation)
                : continuation(::std::forward<F>(continuation))
            {}

            template<typename V, typename S, bool Fin = ManagesFiniteness,
                     ::std::enable_if_t<Fin>* = nullptr>
            decltype(auto) operator()(V &&value, const S& stream, bool &end)
            {
                return continuation(::std::forward<V>(value), stream, end);
            }

            template<typename V, typename S, bool Fin = ManagesFiniteness,
                     ::std::enable_if_t<!Fin>* = nullptr>
            decltype(auto) operator()(V &&value, const S& stream)
            {
                return continuation(::std::forward<V>(value), stream);
            }

        private:
            ::std::decay_t<C> continuation;
        };
    }
}

#endif //CPP_STREAM_DETAIL_CONTINUATION_IMPL_H
