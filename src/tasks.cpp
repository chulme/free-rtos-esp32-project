#include <Arduino.h>
#include "tasks.hpp"
#include "common.hpp"
#include <numeric>
namespace Tasks
{
    bool digital_read(const int8_t input_pin_id)
    {
        return (digitalRead(input_pin_id) == 1);
    }

    Hertz measure_square_wave_frequency(const int8_t input_pin_id)
    {

        Seconds pulse_duration = microsecondsToSeconds(static_cast<double>(pulseIn(input_pin_id,
                                                                                   HIGH,
                                                                                   static_cast<unsigned long>(TIMEOUT))));
        return (pulse_duration == 0)
                   ? 0.0
                   : static_cast<Hertz>(1.0 / (pulse_duration * 2.0)); // Multiply by 2 to get period from pulse duration.
    }

    uint16_t analogue_read(const int8_t input_pin_id)
    {
        uint16_t signal_value = analogRead(input_pin_id);
        return signal_value;
    }

    double compute_filtered_analogue_signal(const std::array<uint16_t, NUMBER_OF_ANALOGUE_READINGS> analogue_readings)
    {
        double average = std::accumulate(analogue_readings.begin(), analogue_readings.end(), 0.0) / NUMBER_OF_ANALOGUE_READINGS;
        return average;
    }

    void execute_no_op_instruction(const size_t number_of_times)
    {
        for (size_t i = 0; i < number_of_times; i++)
        {
            __asm__ __volatile__("nop");
        }
    }
    uint8_t compute_error_code(const double average_analogue_in)
    {
        constexpr auto ESP32_MAXIMUM_ANALOGUE_INPUT = 4095;
        return static_cast<uint8_t>(
            average_analogue_in >
            static_cast<double>(ESP32_MAXIMUM_ANALOGUE_INPUT / 2));
    }

    void visualise_error_code(const uint8_t error_code,
                              const int8_t output_pin_id)
    {
        digitalWrite(output_pin_id, error_code);
    }

    void start_pulse(const int8_t output_pin_id)
    {
        digitalWrite(output_pin_id, HIGH);
    }

    void stop_pulse(const int8_t output_pin_id)
    {
        digitalWrite(output_pin_id, LOW);
    }

    void log(const bool digital_input_state,
             const Hertz square_wave_frequency,
             const double filtered_analogue_signal)
    {
        Serial.printf("%d,%d,%d\n",
                      digital_input_state,
                      static_cast<int>(square_wave_frequency),
                      static_cast<int>(filtered_analogue_signal));
    }
}
