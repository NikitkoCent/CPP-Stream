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
            using Type = typename stream::Stream<T1, T2>::value_type;
            using Source = T2;
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
        using StreamValueT = typename StreamTraits<Stream>::Type;

        template<typename Stream>
        using StreamSourceT = typename StreamTraits<Stream>::Source;
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_VISITORS_H
