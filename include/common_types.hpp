#ifndef COMMON_TYPES
#define COMMON_TYPES

#include <cstdint> // fixed-width integer types

typedef int Microseconds;
typedef int Milliseconds;

typedef float Seconds;

/**
 * @brief Convert seconds to microseconds.
 * @param[in] seconds Seconds to convert.
 * @return @p seconds converted to microseconds.
 */
Microseconds secondsToMicroSeconds(const Seconds seconds);

#endif