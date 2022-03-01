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
}
