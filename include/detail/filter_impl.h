#ifndef CPP_STREAM_DETAIL_FILTER_IMPL_H
#define CPP_STREAM_DETAIL_FILTER_IMPL_H

#include <utility>      // ::std::forward
#include <type_traits>  // ::std::remove_reference_t

namespace stream
{
    namespace detail
    {
        template<typename Callable, bool StreamFiniteness>
        class Filter
        {
        public:
            static constexpr bool IsStreamFinite = StreamFiniteness;


            template<typename F>
            Filter(F &&f)
                : callable(::std::forward<F>(f))
            {}


            template<typename E, typename S>
            decltype(auto) operator()(E &&element, S &&stream, bool &isEnd)
            {
                return callable(::std::forward<E>(element), ::std::forward<S>(stream), isEnd);
            }

        private:
            ::std::decay_t<Callable> callable;
        };
    }
}

#endif //CPP_STREAM_DETAIL_FILTER_IMPL_H
