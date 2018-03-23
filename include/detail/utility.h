#ifndef CPP_STREAM_DETAIL_UTILITY_H
#define CPP_STREAM_DETAIL_UTILITY_H

namespace stream
{
    namespace detail
    {
        // CWG issue #1558 workaround
        template<typename... Args>
        struct MakeVoid
        {
            using Type = void;
        };

        template<typename... Args>
        using VoidT = typename MakeVoid<Args...>::Type;
    }
}

#endif //CPP_STREAM_DETAIL_UTILITY_H
