#include <Arduino.h>
#include <Ticker.h>

#include "common.hpp"
#include "tasks.hpp"
#include "rtos_tasks.hpp"

// Function forward declarations
void create_rtos_tasks();

// Constant expressions
// Pins
constexpr int8_t ANALOGUE_INPUT = 4;
constexpr int8_t WATCHDOG_OUTPUT = 21;
constexpr int8_t DIGITAL_INPUT = 19;
constexpr int8_t ERROR_CODE_LED = 15;
constexpr int8_t PWM_PIN = 16;
constexpr int8_t ANALOGUE_MONITOR_DISPLAY_PIN = 22;
// Task Frequency
constexpr Hertz DEBUG_RATE_AMPLIFIER = 10;
constexpr Hertz TASK_2_RATE = 5.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_3_RATE = 1.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_4_RATE = 24.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_5_RATE = 24.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_6_RATE = 10.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_7_RATE = 3.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_8_RATE = 3.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_9_RATE = 0.2 * DEBUG_RATE_AMPLIFIER;

constexpr int8_t LOW_PRIORITY = 1;
constexpr int8_t MEDIUM_PRIORITY = 2;
constexpr int8_t HIGH_PRIORITY = 3;

constexpr Milliseconds TASK_1_PERIOD = 24.4 * DEBUG_RATE_AMPLIFIER;
constexpr Milliseconds TASK_2_PERIOD = calculateCyclePeriodMs(TASK_2_RATE);
constexpr Milliseconds TASK_3_PERIOD = calculateCyclePeriodMs(TASK_3_RATE);
constexpr Milliseconds TASK_4_PERIOD = calculateCyclePeriodMs(TASK_4_RATE);
constexpr Milliseconds TASK_5_PERIOD = calculateCyclePeriodMs(TASK_5_RATE);
constexpr Milliseconds TASK_6_PERIOD = calculateCyclePeriodMs(TASK_6_RATE);
constexpr Milliseconds TASK_7_PERIOD = calculateCyclePeriodMs(TASK_7_RATE);
constexpr Milliseconds TASK_8_PERIOD = calculateCyclePeriodMs(TASK_8_RATE);
constexpr Milliseconds TASK_9_PERIOD = calculateCyclePeriodMs(TASK_9_RATE);

void setup()
{
    pinMode(ANALOGUE_INPUT, INPUT);
    pinMode(WATCHDOG_OUTPUT, OUTPUT);
    pinMode(DIGITAL_INPUT, INPUT);
    pinMode(PWM_PIN, INPUT);
    pinMode(ERROR_CODE_LED, OUTPUT);
    pinMode(ANALOGUE_MONITOR_DISPLAY_PIN, OUTPUT);

    Serial.begin(115200);
    Serial.println("Cyclic Executive setup complete!");

    create_rtos_tasks();
}

void create_rtos_tasks()
{
    constexpr RtosTasks::TaskParamsWithPulseDuration watchdog_params = {WATCHDOG_OUTPUT, TASK_1_PERIOD, 50};
    xTaskCreate(RtosTasks::transmit_watchdog_waveform,
                "Task 1",
                4000,
                (void *)&watchdog_params,
                LOW_PRIORITY,
                NULL);

    constexpr RtosTasks::TaskParams button_read_params = {DIGITAL_INPUT, TASK_2_PERIOD};
    xTaskCreate(RtosTasks::digital_read,
                "Task 2",
                4000,
                (void *)&button_read_params,
                LOW_PRIORITY,
                NULL);

    constexpr RtosTasks::TaskParams measure_square_wave_freq_params = {PWM_PIN, TASK_3_PERIOD};
    xTaskCreate(RtosTasks::measure_square_wave_frequency,
                "Task 3",
                4000,
                (void *)&measure_square_wave_freq_params,
                LOW_PRIORITY,
                NULL);

    constexpr RtosTasks::TaskParams analogue_read_params = {ANALOGUE_INPUT, TASK_4_PERIOD};
    xTaskCreate(RtosTasks::analogue_read,
                "Task 4",
                100000,
                (void *)&analogue_read_params,
                LOW_PRIORITY,
                NULL);

    constexpr RtosTasks::TaskParams filter_analogue_signal_params = {TASK_5_PERIOD};
    xTaskCreate(RtosTasks::compute_filtered_analogue_signal,
                "Task 5",
                100000,
                (void *)&filter_analogue_signal_params,
                LOW_PRIORITY,
                NULL);

    constexpr RtosTasks::TaskParams no_op_params = {TASK_6_PERIOD};
    xTaskCreate(RtosTasks::execute_no_op_instruction,
                "Task 6",
                100000,
                (void *)&no_op_params,
                LOW_PRIORITY,
                NULL);

    constexpr RtosTasks::TaskParams visualise_error_code_params = {ERROR_CODE_LED, TASK_8_PERIOD};
    TaskHandle_t visualiseHandle = NULL;
    xTaskCreate(RtosTasks::visualise_error_code,
                "Task 8",
                10000,
                (void *)&visualise_error_code_params,
                4,
                &visualiseHandle);

    RtosTasks::TaskParamsWithMailbox compute_error_code_params = {ERROR_CODE_LED, TASK_7_PERIOD, {visualiseHandle}};
    xTaskCreate(RtosTasks::compute_error_code,
                "Task 7",
                10000,
                (void *)&compute_error_code_params,
                5,
                NULL);

    constexpr RtosTasks::TaskParams log_params = {TASK_9_PERIOD};
    xTaskCreate(RtosTasks::log,
                "Task 9",
                4000,
                (void *)&log_params,
                MEDIUM_PRIORITY,
                NULL);
}

void loop()
{
    // put your main code here, to run repeatedly:
}