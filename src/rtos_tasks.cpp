
#include <Arduino.h>

#include "rtos_tasks.hpp"
#include "task_params.hpp"
#include "tasks.hpp"
#include "common.hpp"
namespace RtosTasks
{

    static auto data_to_log = ProtectedTypes::DataToLog();

    static constexpr size_t QUEUE_SIZE = 5;
    static QueueHandle_t avg_analogue_readings = xQueueCreate(QUEUE_SIZE,
                                                              sizeof(double));
    static QueueHandle_t analogue_readings_queue = xQueueCreate(1,
                                                                sizeof(std::array<uint16_t, Tasks::NUMBER_OF_ANALOGUE_READINGS>));

    void transmit_watchdog_waveform(void *params)
    {
        const auto p = *(TaskParams::TaskParamsWithPulseDuration *)params;

        for (;;)
        {
            Tasks::start_pulse(p.pin_id);
            delayMicroseconds(static_cast<uint32_t>(p.pulse_duration)); // investigate delaying for microseconds
            Tasks::stop_pulse(p.pin_id);
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void digital_read(void *params)
    {
        const auto p = *(TaskParams::TaskParams *)params;
        constexpr auto WAIT_TIME = 10.0;
        for (;;)
        {
            constexpr int8_t MONITOR_DISPLAY_PIN = 22;
            digitalWrite(MONITOR_DISPLAY_PIN, HIGH);
            const auto button_pressed = Tasks::digital_read(p.pin_id);
            data_to_log.set_digital_input_state(button_pressed, period_to_number_of_ticks_to_sleep(WAIT_TIME));

            digitalWrite(MONITOR_DISPLAY_PIN, LOW);

            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }

    void measure_square_wave_frequency(void *params)
    {
        const auto p = *(TaskParams::TaskParams *)params;
        constexpr auto ticks_to_wait = period_to_number_of_ticks_to_sleep(1000.0);

        for (;;)
        {
            const auto freq = Tasks::measure_square_wave_frequency(p.pin_id);
            data_to_log.set_square_wave_frequency(freq, ticks_to_wait);

            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void analogue_read(void *params)
    {
        const auto p = *(TaskParams::TaskParams *)params;
        static size_t analogue_index = 0;
        std::array<uint16_t, Tasks::NUMBER_OF_ANALOGUE_READINGS> analogue_readings = {0, 0, 0, 0};

        for (;;)
        {
            analogue_readings[analogue_index] = Tasks::analogue_read(p.pin_id);
            analogue_index = (analogue_index + 1) % Tasks::NUMBER_OF_ANALOGUE_READINGS; // circually increment counter
            xQueueOverwrite(analogue_readings_queue, (void *)&analogue_readings);

            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }
    void compute_filtered_analogue_signal(void *params)
    {
        const auto p = *(TaskParams::TaskParams *)params;
        constexpr auto ticks_to_wait = period_to_number_of_ticks_to_sleep(100.0);
        bool isQueueFull = false;
        for (;;)
        {
            std::array<uint16_t, Tasks::NUMBER_OF_ANALOGUE_READINGS> analogue_readings;

            if (xQueueReceive(analogue_readings_queue, (void *)&analogue_readings, ticks_to_wait))
            {
                auto average_analogue_reading = Tasks::compute_filtered_analogue_signal(analogue_readings);
                /**
                 * Circularly send data to queue (if full, remove oldest/front of queue and append element to back)
                 * This decouples sending-receiving of data - does not require receiving task to pop the front of the queue,
                 * the receiver only needs to know that the data exists, and shall not be responsible for managing it too.
                 */
                if (isQueueFull)
                {
                    double _ignored;
                    xQueueReceive(avg_analogue_readings, (void *)&_ignored, 0);
                    xQueueSend(avg_analogue_readings, (void *)&average_analogue_reading, 0);
                }
                else
                {
                    xQueueSend(avg_analogue_readings, (void *)&average_analogue_reading, 0);
                    isQueueFull = static_cast<bool>(!uxQueueSpacesAvailable(avg_analogue_readings));
                }

                data_to_log.set_filtered_analogue_signal(average_analogue_reading, ticks_to_wait);
            }
            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }

    void execute_no_op_instruction(void *params)
    {
        const auto p = *(TaskParams::TaskParams *)params;
        constexpr auto NUMBER_OF_NOP_INSTRUCTIONS = 1000;

        for (;;)
        {
            Tasks::execute_no_op_instruction(NUMBER_OF_NOP_INSTRUCTIONS);

            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }

    void compute_error_code(void *params)
    {
        auto p = *(TaskParams::TaskParamsWithMailbox *)params;

        for (;;)
        {
            double filtered_analogue_signal_val;
            if (xQueuePeek(avg_analogue_readings, (void *)&filtered_analogue_signal_val, 0))
            {
                const auto err = Tasks::compute_error_code(filtered_analogue_signal_val);

                for (const auto &task : p.tasks)
                    xTaskNotify(task, static_cast<uint32_t>(err), eSetValueWithOverwrite);
            }

            vTaskDelay(period_to_number_of_ticks_to_sleep(p.task_period));
        }
    }

    void visualise_error_code(void *params)
    {
        const auto p = *(TaskParams::TaskParams *)params;

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
        const auto p = *(TaskParams::TaskParams *)params;
        constexpr auto WAIT_TIME = 10.0;
        for (;;)
        {
            bool digital_input_state;
            Hertz square_wave_freq;
            double filtered_analogue_signal_val;

            if (data_to_log.get_data(digital_input_state,
                                     square_wave_freq,
                                     filtered_analogue_signal_val,
                                     period_to_number_of_ticks_to_sleep(WAIT_TIME)))
            {
                if (digital_input_state)
                    Tasks::log(digital_input_state,
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
