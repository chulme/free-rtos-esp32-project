#ifndef RTOS_TASKS
#define RTOS_TASKS

#include <Arduino.h>

#include "common.hpp"
#include "protected_types.hpp"
namespace RtosTasks
{
    void transmit_watchdog_waveform(void *params);       // Task 1
    void digital_read(void *params);                     // Task 2
    void measure_square_wave_frequency(void *params);    // Task 3
    void analogue_read(void *params);                    // Task 4
    void compute_filtered_analogue_signal(void *params); // Task 5
    void execute_no_op_instruction(void *params);        // Task 6
    void compute_error_code(void *params);               // Task 7
    void visualise_error_code(void *params);             // Task 8
    void log(void *params);                              // Task 9
}
#endif