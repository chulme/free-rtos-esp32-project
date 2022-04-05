#ifndef RTOS_TASKS
#define RTOS_TASKS

#include <Arduino.h>
#include <memory>

#include "common_types.hpp"
#include "protected_types.hpp"
#include <set>
namespace RtosTasks
{

    struct RtosTaskParams
    {
        const int8_t pin_id;
        const Milliseconds task_period;

        constexpr RtosTaskParams(const int8_t pin_id, const Milliseconds task_period)
            : pin_id(pin_id), task_period(task_period) {}

        constexpr RtosTaskParams(const Milliseconds task_period)
            : pin_id(0), task_period(task_period) {}
        RtosTaskParams() = delete;
    };

    struct MailboxParams : public RtosTaskParams
    {
        MailboxParams(const uint8_t pin_id,
                      const Milliseconds task_period,
                      const std::set<TaskHandle_t> tasks)
            : RtosTaskParams(pin_id, task_period),
              tasks(tasks)
        {
        }
        const std::set<TaskHandle_t> tasks;
    };
    struct WatchdogTaskParams : public RtosTaskParams
    {
        constexpr WatchdogTaskParams(const uint8_t pin_id,
                                     const Milliseconds task_period,
                                     const Milliseconds pulse_duration)
            : RtosTaskParams(pin_id, task_period),
              pulse_duration(pulse_duration) {}

        const Milliseconds pulse_duration;
    };

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