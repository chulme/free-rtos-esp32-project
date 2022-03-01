#include <Arduino.h>
#include "tasks.hpp"
#include "common_types.hpp"
#include <numeric>
namespace Tasks
{

    uint16_t analogue_read(const int8_t input_pin_id)
    {
        uint16_t signal_value = analogRead(input_pin_id);
        Serial.printf("%lu\t Analogue signal value: %d.\n", millis(), signal_value);
        return signal_value;
    }

    double compute_filtered_analogue_signal(const std::array<uint16_t, NUMBER_OF_ANALOGUE_READINGS> analogue_readings)
    {
        double average = std::accumulate(analogue_readings.begin(), analogue_readings.end(), 0.0) / NUMBER_OF_ANALOGUE_READINGS;
        Serial.printf("%lu\t Filtered analogue signal value: %f.\n", millis(), average);
        return average;
    }

    void execute_no_op_instruction(const size_t number_of_times)
    {
        for (size_t i = 0; i < number_of_times; i++)
        {
            __asm__ __volatile__("nop");
        }
    }

    void visualise_error_code(const double average_analogue_in,
                              const uint16_t half_of_maximum_analogue_input_range,
                              const int8_t output_pin_id)
    {

        uint8_t error_code = (average_analogue_in > half_of_maximum_analogue_input_range);
        Serial.printf("Error code: %d\n", error_code);
        digitalWrite(output_pin_id, error_code);
    }

    void log(const bool digital_input_state,
             const Hertz square_wave_frequency,
             const double filtered_analogue_signal)
    {
        Serial.printf("%i,%f,%f\n",
                      digital_input_state,
                      square_wave_frequency,
                      filtered_analogue_signal);
    }
}
