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

        bool set_digital_input_state(const bool digital_in_state, const TickType_t semaphore_ticks_to_wait)
        {
            if (xSemaphoreTake(semaphore, semaphore_ticks_to_wait))
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

        bool set_square_wave_frequency(const Hertz square_wave_freq, const TickType_t semaphore_ticks_to_wait)
        {
            if (xSemaphoreTake(semaphore, semaphore_ticks_to_wait))
            {
                square_wave_frequency = square_wave_freq;
                xSemaphoreGive(semaphore);
                return true;
            }
            else
            {
                return false;
            }
        }

        bool set_filtered_analogue_signal(const double filtered_analogue_sig, const TickType_t semaphore_ticks_to_wait)
        {
            if (xSemaphoreTake(semaphore, semaphore_ticks_to_wait))
            {
                filtered_analogue_signal = filtered_analogue_sig;
                xSemaphoreGive(semaphore);
                return true;
            }
            else
            {
                return false;
            }
        }

        [[no_discard]] bool get_data(bool &digital_in_state,
                                     Hertz &square_wave_freq,
                                     double &filtered_analogue_sig,
                                     const TickType_t semaphore_ticks_to_wait) const
        {
            if (xSemaphoreTake(semaphore, semaphore_ticks_to_wait))
            {
                digital_in_state = digital_input_state;
                square_wave_freq = square_wave_frequency;
                filtered_analogue_sig = filtered_analogue_signal;
                xSemaphoreGive(semaphore);
                return true;
            }
            return false;
        }

    private:
        bool digital_input_state;
        Hertz square_wave_frequency;
        double filtered_analogue_signal;

        xSemaphoreHandle semaphore = xSemaphoreCreateMutex();
    };
}

#endif