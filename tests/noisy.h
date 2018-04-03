#ifndef CPP_STREAM_TESTS_NOISY_H
#define CPP_STREAM_TESTS_NOISY_H

#include <iostream>

struct Noisy
{
    int value;
    int copyCount = 0;


    Noisy(int value = 0) noexcept
        : value(value)
    {}

    Noisy(const Noisy &src) noexcept
    {
        value = src.value;
        copyCount = src.copyCount + 1;

        std::cout << "NOISY COPY CONSTRUCTOR" << std::endl;
    }

    Noisy(Noisy &&src) noexcept
    {
        value = src.value;
    }


    Noisy& operator=(const Noisy &right) noexcept
    {
        if (this != &right)
        {
            value = right.value;
            copyCount = right.copyCount;

            std::cout << "NOISY COPY ASSIGNMENT" << std::endl;
        }

        return *this;
    }

    Noisy& operator=(Noisy &&right) noexcept
    {
        if (this != &right)
        {
            value = right.value;
        }

        return *this;
    }
};


std::ostream& operator<<(std::ostream &stream, const Noisy &noisy)
{
    return stream << noisy.value;
}

Noisy operator+(const Noisy &left, const Noisy &right)
{
    return {left.value + right.value};
}


#endif //CPP_STREAM_NOISY_H
