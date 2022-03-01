#ifndef COMMON_TYPES
#define COMMON_TYPES

#include <cstdint> // fixed-width integer types

typedef double Microseconds;
typedef double Milliseconds;
typedef double Seconds;
typedef double Hertz;

constexpr Milliseconds calculateCyclePeriod(const Hertz frequency)
{
    return ((1.0 / frequency) * 1000.0);
}

#endif