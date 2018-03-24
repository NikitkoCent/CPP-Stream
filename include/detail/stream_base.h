#ifndef CPP_STREAM_DETAIL_STREAM_BASE_H
#define CPP_STREAM_DETAIL_STREAM_BASE_H

namespace stream
{
    template<typename, typename>
    struct Stream;

    namespace detail
    {
        template<typename S, typename Filter, typename StreamImpl>
        class StreamFilter;


        template<typename T, bool Finiteness, typename StreamImpl>
        class StreamBase
        {
        public:
            using Type = T;
            static constexpr bool IsFinite = Finiteness;


            template<typename Filter>
            auto operator|(Filter &&)
            {
                return 10;
            }

        protected:
            bool isEnd() const
            {
                static_assert(IsFinite, "This stream is infinite");
                return static_cast<const StreamImpl *>(this)->isEndImpl();
            }
        };
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_BASE_H
