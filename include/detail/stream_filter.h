#ifndef CPP_STREAM_DETAIL_STREAM_FILTER_H
#define CPP_STREAM_DETAIL_STREAM_FILTER_H

#include "stream_base.h"
#include "utility.h"
#include "traits.h"
#include "stream_traits.h"
#include <type_traits>      // ::std::decay_t, ::std::enable_if_t, ::std::is_same
#include <utility>          // ::std::move, ::std::forward
#include <optional>         // ::std::optional

namespace stream
{
    namespace detail
    {
        template<typename S, typename Filter, typename StreamImpl>
        class StreamFilter : public StreamBase<typename InvokeResultT<::std::decay_t<Filter>, const StreamValueT<S>&>::value_type,
                                               StreamFinitenessV<S>, StreamImpl>
        {
        private:
            using Reference = const StreamValueT<S>&;
            using Parent = StreamBase<typename InvokeResultT<::std::decay_t<Filter>, Reference>::value_type,
                                      StreamFinitenessV<S>, StreamImpl>;

        public:
            template<typename Callable>
            StreamFilter(S &&stream, Callable &&filter)
                : stream(::std::move(stream)), filter(::std::forward<Callable>(filter))
            {}

        protected:
            ::std::optional<typename Parent::Type> getNext()
            {
                auto fromStream = stream.getNext();
                if (!fromStream)
                {
                    return { ::std::nullopt };
                }

                return filter(static_cast<Reference>(fromStream.value()));
            }

        private:
            friend class StreamBase<typename InvokeResultT<::std::decay_t<Filter>, Reference>::value_type,
                                    StreamFinitenessV<S>, StreamImpl>;


            S stream;
            ::std::decay_t<Filter> filter;


            template<bool Fin = Parent::IsFinite>
            ::std::enable_if_t<Fin, bool> isEndImpl() const
            {
                return stream.isEnd();
            };
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_FILTER_H
