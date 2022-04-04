
#include <Arduino.h>
#include "rtos_tasks.hpp"
#include "tasks.hpp"

namespace RtosTasks
{

    static constexpr TickType_t period_to_number_of_ticks_to_sleep(Milliseconds period);
    static auto data_to_log = ProtectedTypes::DataToLog();
    static uint8_t error_code = 0;
    std::array<uint16_t, Tasks::NUMBER_OF_ANALOGUE_READINGS> analogue_readings = {0, 0, 0, 0};

    static constexpr size_t QUEUE_SIZE = 5;
    static QueueHandle_t avg_analogue_readings = xQueueCreate(QUEUE_SIZE,
                                                              sizeof(double));

    void toggle_digital_out(void *params)
    {
    }
    void digital_read(void *params)
    {
        const auto p = *(RtosTaskParams *)params;
        for (;;)
        {
            const auto button_pressed = Tasks::digital_read(p.pin_id);
            data_to_log.set_digital_input_state(button_pressed);
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void measure_square_wave_frequency(void *params)
    {
    }
    void analogue_read(void *params)
    {
        const auto p = *(RtosTaskParams *)params;
        static size_t analogue_index = 0;
        for (;;)
        {
            analogue_readings[analogue_index] = Tasks::analogue_read(p.pin_id);
            analogue_index = (analogue_index + 1) % Tasks::NUMBER_OF_ANALOGUE_READINGS; // circually increment counter
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void compute_filtered_analogue_signal(void *params)
    {
        const auto p = *(RtosTaskParams *)params;

        for (;;)
        {
            auto average_analogue_reading = Tasks::compute_filtered_analogue_signal(analogue_readings);
            xQueueSend(avg_analogue_readings, (void *)&average_analogue_reading, 100);
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void execute_no_op_instruction(void *params)
    {
    }
    void compute_error_code(void *params)
    {
        const auto p = *(RtosTaskParams *)params;

        for (;;)
        {
            double filtered_analogue_signal_val;
            // Read without removing front of queue.
            xQueuePeek(avg_analogue_readings,
                       (void *)&filtered_analogue_signal_val, 0);
            error_code = Tasks::compute_error_code(filtered_analogue_signal_val);

            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void visualise_error_code(void *params)
    {
        const auto p = *(RtosTaskParams *)params;

        for (;;)
        {
            Tasks::visualise_error_code(error_code, p.pin_id);
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void log(void *params)
    {
        const auto p = *(RtosTaskParams *)params;

        for (;;)
        {
            double filtered_analogue_signal_val;
            if (data_to_log.get_digital_input_state().was_able_to_access() &&
                xQueueReceive(avg_analogue_readings,
                              (void *)&filtered_analogue_signal_val, 0))
            {
                Tasks::log(data_to_log.get_digital_input_state().get(),
                           15.0,
                           filtered_analogue_signal_val);
            }
            else
            {
                Serial.println("Unable to log, data not accessible.");
            }
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }

    static constexpr TickType_t period_to_number_of_ticks_to_sleep(Milliseconds period)
    {
        return static_cast<TickType_t>(period / portTICK_PERIOD_MS);
    }
}
