#ifndef CPP_STREAM_DETAIL_STREAM_FILTER_H
#define CPP_STREAM_DETAIL_STREAM_FILTER_H

#include "stream_base.h"        // StreamBase
#include "filter_impl.h"        // Filter
#include "traits_impl.h"        // InvokeResultT
#include "stream_traits_impl.h" // StreamValueT
#include <type_traits>          // ::std::decay_t, ::std::enable_if_t, ::std::is_same
#include <utility>              // ::std::move, ::std::forward
#include <optional>             // ::std::optional

namespace stream
{
    namespace detail
    {
        template<typename S, typename Callable, typename StreamImpl = void>
        class StreamFilter
        {};


        #define SFPARENT StreamBase<typename InvokeResultT<Filter<Callable, StreamFin>, \
                                                           const StreamValueT<S>&, const S&, bool&>::value_type, \
                                    StreamFinitenessV<S> | StreamFin, StreamImpl>


        template<typename S, typename Callable, bool StreamFin, typename StreamImpl>
        class StreamFilter<S, Filter<Callable, StreamFin>, StreamImpl> : public SFPARENT
        {
        public:
            template<typename F>
            StreamFilter(S &&stream, F &&filter)
                : stream(::std::move(stream)), filter(::std::forward<F>(filter))
            {}


            ::std::optional<typename SFPARENT::Type> getNext()
            {
                auto fromStream = stream.getNext();
                if (!fromStream)
                {
                    return { ::std::nullopt };
                }

                return filter(static_cast<const StreamValueT<S>&>(fromStream.value()), static_cast<const S&>(stream),
                              end);
            }

        private:
            friend class SFPARENT;


            S stream;
            Filter<Callable, StreamFin> filter;
            bool end = false;


            template<bool Fin = SFPARENT::IsFinite>
            ::std::enable_if_t<Fin, bool> isEndImpl() const
            {
                if constexpr (StreamFin)
                {
                    return end;
                }
                else
                {
                    return stream.isEnd();
                }
            }
        };


        #undef SFPARENT
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_FILTER_H
