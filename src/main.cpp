#include <Arduino.h>
#include <Ticker.h>

#include "common_types.hpp"
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
constexpr Hertz DEBUG_RATE_AMPLIFIER = 1;
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

constexpr Milliseconds TASK_1_PERIOD = 24.4;
constexpr Milliseconds TASK_2_PERIOD = calculateCyclePeriod(TASK_2_RATE);
constexpr Milliseconds TASK_3_PERIOD = calculateCyclePeriod(TASK_3_RATE);
constexpr Milliseconds TASK_4_PERIOD = calculateCyclePeriod(TASK_4_RATE);
constexpr Milliseconds TASK_5_PERIOD = calculateCyclePeriod(TASK_5_RATE);
constexpr Milliseconds TASK_6_PERIOD = calculateCyclePeriod(TASK_6_RATE);
constexpr Milliseconds TASK_7_PERIOD = calculateCyclePeriod(TASK_7_RATE);
constexpr Milliseconds TASK_8_PERIOD = calculateCyclePeriod(TASK_8_RATE);
constexpr Milliseconds TASK_9_PERIOD = calculateCyclePeriod(TASK_9_RATE);

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
    constexpr RtosTasks::RtosTaskParams buttonReadParams = {DIGITAL_INPUT, 500.0};
    xTaskCreate(RtosTasks::digital_read,
                "Task 2",
                4000,
                (void *)&buttonReadParams,
                LOW_PRIORITY,
                NULL);

    constexpr RtosTasks::RtosTaskParams analogueReadParams = {ANALOGUE_INPUT, 400.0};
    xTaskCreate(RtosTasks::analogue_read,
                "Task 4",
                100000,
                (void *)&analogueReadParams,
                LOW_PRIORITY,
                NULL);

    constexpr RtosTasks::RtosTaskParams filterAnalogueSignalParams = {1000.0};
    xTaskCreate(RtosTasks::compute_filtered_analogue_signal,
                "Task 5",
                100000,
                (void *)&filterAnalogueSignalParams,
                LOW_PRIORITY,
                NULL);

    constexpr RtosTasks::RtosTaskParams visualiseErrorCodeParams = {ERROR_CODE_LED, 550.0};
    TaskHandle_t visualiseHandle = NULL;
    xTaskCreate(RtosTasks::visualise_error_code,
                "Task 8",
                10000,
                (void *)&visualiseErrorCodeParams,
                4,
                &visualiseHandle);

    RtosTasks::MailboxParams computeErrorCodeParams = {ERROR_CODE_LED, 120.0, {visualiseHandle}};
    xTaskCreate(RtosTasks::compute_error_code,
                "Task 7",
                10000,
                (void *)&computeErrorCodeParams,
                5,
                NULL);

    constexpr RtosTasks::RtosTaskParams logParams = {500.0};
    xTaskCreate(RtosTasks::log,
                "Task 9",
                4000,
                (void *)&logParams,
                MEDIUM_PRIORITY,
                NULL);
}

void loop()
{
    // put your main code here, to run repeatedly:
}