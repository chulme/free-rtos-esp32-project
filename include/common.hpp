#ifndef COMMON
#define COMMON

#include <Arduino.h>
#include <cstdint> // fixed-width integer types

typedef double Microseconds;
typedef double Milliseconds;
typedef double Seconds;
typedef double Hertz;

constexpr Seconds microsecondsToSeconds(const Microseconds microseconds)
{
    return (microseconds / 1000000.0);
}

constexpr Milliseconds calculateCyclePeriod(const Hertz frequency)
{
    return ((1.0 / frequency) * 1000.0);
}

static constexpr TickType_t period_to_number_of_ticks_to_sleep(Milliseconds period)
{
    return static_cast<TickType_t>(period / portTICK_PERIOD_MS);
}

#endif