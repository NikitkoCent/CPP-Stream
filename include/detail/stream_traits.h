#ifndef CPP_STREAM_DETAIL_STREAM_TRAITS_H
#define CPP_STREAM_DETAIL_STREAM_TRAITS_H

#include <type_traits>  // ::std::remove_reference_t

namespace stream
{
    template<typename T1, typename T2>
    struct Stream;

    namespace detail
    {
        template<typename Stream>
        struct StreamTraits;

        template<typename T1, typename T2>
        struct StreamTraits<stream::Stream<T1, T2>>
        {
            using StreamType = stream::Stream<T1, T2>;
            using ValueType = typename StreamType::Type;
            using Source = T2;
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


        template<typename Stream>
        using StreamValueT = typename StreamTraits<Stream>::ValueType;

        template<typename Stream>
        using StreamSourceT = typename StreamTraits<Stream>::Source;

        template<typename Stream>
        constexpr static bool StreamFinitenessV = StreamTraits<Stream>::IsFinite;
    }
}

#endif //CPP_STREAM_DETAIL_STREAM_TRAITS_H
