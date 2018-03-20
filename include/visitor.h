#ifndef CPP_STREAM_VISITOR_H
#define CPP_STREAM_VISITOR_H

#include "detail/utility.h"
#include <type_traits>

namespace stream
{
    template<typename Callable>
    class Visitor
    {
    public:
        Visitor(const Callable &callable)
            : callable(callable)
        {}

        Visitor(Callable &&callable)
            : callable(::std::move(callable))
        {}


        template<typename Stream>
        detail::InvokeResultT<Callable&, const Stream&> operator()(const Stream &stream)
        {
            return callable(stream);
        }

    private:
        ::std::decay_t<Callable> callable;
    };
}

#endif //CPP_STREAM_VISITOR_H
