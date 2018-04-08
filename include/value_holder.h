#ifndef CPP_STREAM_VALUE_HOLDER_H
#define CPP_STREAM_VALUE_HOLDER_H

#include <type_traits>  // ::std::is_rvalue_reference
#include <utility>      // ::std::forward
#include <functional>   // ::std::reference_wrapper

namespace stream
{
    template<typename T>
    class ValueHolder
    {
    public:
        static_assert(::std::is_rvalue_reference<T>::value, "RValue references isn't allowed");

        template<typename... Args>
        ValueHolder(Args&&... args)
            : value(::std::forward<Args>(args)...)
        {}


        T& get()
        {
            return value;
        }

        const T& get() const
        {
            return value;
        }


        operator T&()
        {
            return get();
        }

        operator const T&() const
        {
            return get();
        }

    private:
        T value;
    };


    template<typename T>
    class ValueHolder<T&>
    {
    public:
        template<typename Ref>
        ValueHolder(Ref &ref)
            : value(ref)
        {}


        T& get()
        {
            return value;
        }

        const T& get() const
        {
            return value;
        }


        operator T&()
        {
            return get();
        }

        operator const T&() const
        {
            return get();
        }

    private:
        ::std::reference_wrapper<T> value;
    };
}

#endif //CPP_STREAM_VALUE_HOLDER_H
