#ifndef CPP_STREAM_DETAIL_STREAM_VISITORS_H
#define CPP_STREAM_DETAIL_STREAM_VISITORS_H

#include "../stream.h"
#include <type_traits>  // ::std::remove_reference_t

namespace stream
{
    namespace detail
    {
        template<typename Stream>
        struct StreamTraits;

        template<typename T1, typename T2>
        struct StreamTraits<stream::Stream<T1, T2>>
        {
            using StreamType = stream::Stream<T1, T2>;
            using ValueType = typename StreamType::value_type;
            using Source = T2;
            using Iterator = typename StreamType::const_iterator;
            static constexpr bool IsFinite = StreamType::IsFinite;
        };

        template<typename Stream>
        struct StreamTraits<const Stream> : StreamTraits<Stream>
        {};

        template<typename Stream>
        struct StreamTraits<Stream&> : StreamTraits<Stream>
        {};

        template<typename Stream>
        struct StreamTraits<Stream&&> : StreamTraits<Stream>
        {};

        /*template<typename Stream>
        struct StreamTraits<Stream*> : StreamTraits<Stream>
        {};

        template<typename Stream>
        struct StreamTraits<Stream* const> : StreamTraits<Stream>
        {};*/


        template<typename Stream>
        using StreamValueT = typename StreamTraits<Stream>::ValueType;

        template<typename Stream>
        using StreamSourceT = typename StreamTraits<Stream>::Source;

        template<typename Stream>
        constexpr static bool StreamFinitenessV = StreamTraits<Stream>::IsFinite;
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_VISITORS_H
