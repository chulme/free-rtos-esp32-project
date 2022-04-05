#ifndef TASK_PARAMS
#define TASK_PARAMS

#include <Arduino.h>
#include <set>

#include "common.hpp"

namespace TaskParams
{
    struct TaskParams
    {
        const int8_t pin_id;
        const Milliseconds task_period;

        constexpr TaskParams(const int8_t pin_id, const Milliseconds task_period)
            : pin_id(pin_id), task_period(task_period) {}

        constexpr TaskParams(const Milliseconds task_period)
            : pin_id(0), task_period(task_period) {}

        TaskParams() = delete;
    };

    struct TaskParamsWithMailbox : public TaskParams
    {
        const std::set<TaskHandle_t> tasks;

        TaskParamsWithMailbox(const uint8_t pin_id,
                              const Milliseconds task_period,
                              const std::set<TaskHandle_t> tasks)
            : TaskParams(pin_id, task_period),
              tasks(tasks) {}
    };

    struct TaskParamsWithPulseDuration : public TaskParams
    {
        const Milliseconds pulse_duration;

        constexpr TaskParamsWithPulseDuration(const uint8_t pin_id,
                                              const Milliseconds task_period,
                                              const Milliseconds pulse_duration)
            : TaskParams(pin_id, task_period),
              pulse_duration(pulse_duration) {}
    };
}
#endif