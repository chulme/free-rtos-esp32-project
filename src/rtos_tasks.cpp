
#include <Arduino.h>
#include "rtos_tasks.hpp"
#include "tasks.hpp"
#include "common_types.hpp"
namespace RtosTasks
{

    static auto data_to_log = ProtectedTypes::DataToLog();

    std::array<uint16_t, Tasks::NUMBER_OF_ANALOGUE_READINGS> analogue_readings = {0, 0, 0, 0};

    static constexpr size_t QUEUE_SIZE = 5;
    static QueueHandle_t avg_analogue_readings = xQueueCreate(QUEUE_SIZE,
                                                              sizeof(double));
    static QueueHandle_t square_wave_frequencies = xQueueCreate(QUEUE_SIZE,
                                                                sizeof(Hertz));

    void toggle_digital_out(void *params)
    {
    }
    void digital_read(void *params)
    {
        const auto p = *(RtosTaskParams *)params;
        constexpr auto WAIT_TIME = 10.0;
        for (;;)
        {
            const auto button_pressed = Tasks::digital_read(p.pin_id);
            data_to_log.set_digital_input_state(button_pressed, period_to_number_of_ticks_to_sleep(WAIT_TIME));
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void measure_square_wave_frequency(void *params)
    {
        const auto p = *(RtosTaskParams *)params;

        for (;;)
        {
            const auto freq = Tasks::measure_square_wave_frequency(p.pin_id);
            xQueueSend(square_wave_frequencies, (void *)&freq, 100);
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
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
        const auto p = *(RtosTaskParams *)params;
        constexpr auto NUMBER_OF_NOP_INSTRUCTIONS = 1000;

        for (;;)
        {
            Tasks::execute_no_op_instruction(NUMBER_OF_NOP_INSTRUCTIONS);
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void compute_error_code(void *params)
    {
        auto p = *(MailboxParams *)params;

        for (;;)
        {
            double filtered_analogue_signal_val;
            // Read without removing front of queue.
            xQueuePeek(avg_analogue_readings, (void *)&filtered_analogue_signal_val, 0);
            const auto err = Tasks::compute_error_code(filtered_analogue_signal_val);

            // Serial.printf("Sending error code %d\n", err);
            for (const auto &task : p.tasks)
                xTaskNotify(task, static_cast<uint32_t>(err), eSetValueWithOverwrite);

            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void visualise_error_code(void *params)
    {
        const auto p = *(RtosTaskParams *)params;

        for (;;)
        {
            uint32_t err_code;
            if (xTaskNotifyWait(0, 0, &err_code, 0))
            {
                Tasks::visualise_error_code(static_cast<uint8_t>(err_code), p.pin_id);
            }
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }

    void log(void *params)
    {
        const auto p = *(RtosTaskParams *)params;
        constexpr auto WAIT_TIME = 10.0;
        for (;;)
        {
            double filtered_analogue_signal_val, square_wave_freq;

            if (data_to_log.get_digital_input_state(period_to_number_of_ticks_to_sleep(WAIT_TIME)).was_able_to_access() &&
                xQueueReceive(avg_analogue_readings, (void *)&filtered_analogue_signal_val, period_to_number_of_ticks_to_sleep(WAIT_TIME)) &&
                xQueueReceive(square_wave_frequencies, (void *)&square_wave_freq, period_to_number_of_ticks_to_sleep(WAIT_TIME)))
            {
                Tasks::log(data_to_log.get_digital_input_state(period_to_number_of_ticks_to_sleep(WAIT_TIME)).get(),
                           square_wave_freq,
                           filtered_analogue_signal_val);
            }
            else
            {
                Serial.println("Unable to log, data not accessible.");
            }
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }

}
