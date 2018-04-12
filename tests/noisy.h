#ifndef CPP_STREAM_TESTS_NOISY_H
#define CPP_STREAM_TESTS_NOISY_H

#include <iostream>

struct Noisy
{
    int value;
    static int copyCount;


    Noisy(int value = 0) noexcept
        : value(value)
    {}

    Noisy(const Noisy &src) noexcept
    {
        std::cout << "NOISY COPY CONSTRUCTOR" << std::endl;

        ++copyCount;
        value = src.value;
    }

    Noisy(Noisy &&src) noexcept = default;


    Noisy& operator=(const Noisy &right) noexcept
    {
        std::cout << "NOISY COPY ASSIGNMENT" << std::endl;

        if (this != &right)
        {
            ++copyCount;
            value = right.value;
        }

        return *this;
    }

    Noisy& operator=(Noisy &&right) noexcept = default;
};


inline std::ostream& operator<<(std::ostream &stream, const Noisy &noisy)
{
    return stream << noisy.value;
}

inline Noisy operator+(const Noisy &left, const Noisy &right)
{
    return {left.value + right.value};
}


#endif //CPP_STREAM_TESTS_NOISY_H
