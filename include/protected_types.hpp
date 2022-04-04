#ifndef PROTECTED_TYPES
#define PROTECTED_TYPES

#include "Arduino.h"

#include "common_types.hpp"

namespace ProtectedTypes
{
    /**
     * Mimics C++17 Optional class, to be used to optionally return some type T.
     */
    template <typename T>
    class ProtectedOptional
    {
    public:
        // Have value, therefore set access to true.
        ProtectedOptional(const T t) : t(t), access(true){};
        // Do not value, therefore set access to false.
        ProtectedOptional() : t(nullptr), access(false){};

        const bool was_able_to_access() const
        {
            return access;
        }

        const T get() const
        {
            return t;
        }

    private:
        T t;
        bool access;
    };

    class DataToLog
    {
    public:
        DataToLog(const bool digital_input_state,
                  const Hertz square_wave_frequency,
                  const double filtered_analogue_signal) : digital_input_state(digital_input_state),
                                                           square_wave_frequency(square_wave_frequency),
                                                           filtered_analogue_signal(filtered_analogue_signal) {}
        DataToLog() : digital_input_state(false),
                      square_wave_frequency(0.0),
                      filtered_analogue_signal(0.0) {}

        bool set_digital_input_state(const bool digital_in_state)
        {
            if (xSemaphoreTake(semaphore, semaphore_block_time))
            {
                digital_input_state = digital_in_state;
                xSemaphoreGive(semaphore);
                return true;
            }
            else
            {
                return false;
            }
        }

        ProtectedOptional<bool> get_digital_input_state() const
        {
            if (xSemaphoreTake(semaphore, semaphore_block_time))
            {
                const auto digital_in_state = ProtectedOptional<bool>(digital_input_state);
                xSemaphoreGive(semaphore);
                return digital_in_state;
            }
            return ProtectedOptional<bool>();
        }

    private:
        bool digital_input_state;
        Hertz square_wave_frequency;
        double filtered_analogue_signal;
        static constexpr Milliseconds semaphore_block_time = 100.0;

        xSemaphoreHandle semaphore = xSemaphoreCreateMutex();
    };
}

#endif