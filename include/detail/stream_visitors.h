#ifndef CPP_STREAM_DETAIL_STREAM_VISITORS_H
#define CPP_STREAM_DETAIL_STREAM_VISITORS_H

#include "../stream.h"
#include <type_traits>  // ::std::remove_reference_t

namespace stream
{
    namespace detail
    {
        template<typename Stream>
        struct StreamValue;

        template<typename T1, typename T2>
        struct StreamValue<stream::Stream<T1, T2>>
        {
            using Type = typename stream::Stream<T1, T2>::value_type;
        };

        template<typename Stream>
        struct StreamValue<const Stream> : StreamValue<Stream>
        {};

        template<typename Stream>
        struct StreamValue<Stream&> : StreamValue<Stream>
        {};

        template<typename Stream>
        struct StreamValue<Stream&&> : StreamValue<Stream>
        {};

        /*template<typename Stream>
        struct StreamValue<Stream*> : StreamValue<Stream>
        {};

        template<typename Stream>
        struct StreamValue<Stream* const> : StreamValue<Stream>
        {};*/


        template<typename Stream>
        using StreamValueT = typename StreamValue<Stream>::Type;
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_VISITORS_H
