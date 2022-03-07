#ifndef TASKS
#define TASKS

#include <array>

#include "common_types.hpp"

namespace Tasks
{
    constexpr size_t NUMBER_OF_ANALOGUE_READINGS = 4;
    static constexpr Milliseconds TIMEOUT = 5;
    void start_pulse(const int8_t output_pin_id);

    void stop_pulse(const int8_t output_pin_id);

    void toggle_digital_out(const int8_t output_pin_id);                                      // Task 1
    bool digital_read(const int8_t input_pin_id);                                             // Task 2
    Hertz measure_square_wave_frequency(const int8_t input_pin_id);                           // Task 3
    uint16_t analogue_read(const int8_t input_pin_id);                                        // Task 4
    double compute_filtered_analogue_signal(const std::array<uint16_t, 4> analogue_readings); // Task 5
    void execute_no_op_instruction(const size_t number_of_times);                             // Task 6

    uint8_t compute_error_code(const double average_analogue_in, const uint16_t maximum_analogue_input_range);

    void visualise_error_code(const uint8_t error_code,
                              const int8_t output_pin_id);
    void log(const bool digital_input_state, // Task 9
             const Hertz square_wave_frequency,
             const double filtered_analogue_signal);
}
#endif