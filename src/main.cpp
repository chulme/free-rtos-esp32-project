#include <Arduino.h>
#include <Ticker.h>

#include "common_types.hpp"
#include "tasks.hpp"

// Function forward declarations
void callback();
const Milliseconds roundToTheNearestMultiple(const Milliseconds value_to_round, const Milliseconds multiple);

// Constant expressions
constexpr Milliseconds SLOT_WIDTH = 5;
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

Milliseconds TASK_1_PERIOD = 24.4;
Milliseconds WATCHDOG_LENGTH = 0.05;
Milliseconds TASK_2_PERIOD;
Milliseconds TASK_3_PERIOD;
Milliseconds TASK_4_PERIOD;
Milliseconds TASK_5_PERIOD;
Milliseconds TASK_6_PERIOD;
Milliseconds TASK_7_PERIOD;
Milliseconds TASK_8_PERIOD;
Milliseconds TASK_9_PERIOD;
Ticker slot_ticker;

void setup()
{
    pinMode(ANALOGUE_INPUT, INPUT);
    pinMode(WATCHDOG_OUTPUT, OUTPUT);
    pinMode(DIGITAL_INPUT, INPUT);
    pinMode(PWM_PIN, INPUT);
    pinMode(ERROR_CODE_LED, OUTPUT);
    pinMode(ANALOGUE_MONITOR_DISPLAY_PIN, OUTPUT);

    Serial.begin(115200);
    Serial.println("Cyclic Executive setup 2 complete!");

    Serial.printf("Slot Width: %f\tDebug Amplifier: %f\n", SLOT_WIDTH, DEBUG_RATE_AMPLIFIER);

    /**
     * Calculate the period for each task (in milliseconds), and then round to the
     * nearest multiple of the SLOT_WIDTH. This ensures that tasks are executed at
     * approximately the right time, while preventing tasks from being missed due to
     * a mismatch between SLOT_WIDTH and the task's period ie.
     *
     * SLOT_WIDTH = 2ms, TASK_PERIOD=3ms, would result in the task only being executed
     * every 30ms (slot time increments by 2ms, only time 3ms is a multiple of 2ms is every
     * 30ms.)
     *
     * By rounding to the nearest multiple, this issue is resolved:
     * SLOT_WIDTH = 2ms, TASK_PERIOD = 3ms => 2ms
     */
    TASK_1_PERIOD = roundToTheNearestMultiple(TASK_1_PERIOD, SLOT_WIDTH);
    TASK_2_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_2_RATE), SLOT_WIDTH);
    TASK_3_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_3_RATE), SLOT_WIDTH);
    TASK_4_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_4_RATE), SLOT_WIDTH);
    TASK_5_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_5_RATE), SLOT_WIDTH);
    TASK_6_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_6_RATE), SLOT_WIDTH);
    TASK_7_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_7_RATE), SLOT_WIDTH);
    TASK_8_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_8_RATE), SLOT_WIDTH);
    TASK_9_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_9_RATE), SLOT_WIDTH);
    slot_ticker.attach_ms(SLOT_WIDTH, callback);
}

bool is_task_slot(const Milliseconds slot_start_time, const Milliseconds task_period)
{
    return (static_cast<int64_t>((slot_start_time)) % static_cast<int64_t>((task_period)) == 0);
}

void callback()
{
    // Static variables have continous lifetime in scope of function (ie. values persist across
    // function calls). Using static over global variables helps to keep global variables be
    // configuration related.
    static int64_t slot_counter = 0;
    static Milliseconds slot_start_time = 0;
    static std::array<uint16_t, Tasks::NUMBER_OF_ANALOGUE_READINGS> analogue_readings = {0, 0, 0, 0};
    static size_t analogue_index = 0;
    static double average_analogue_reading = 0;
    static bool digital_input_state = 0;
    static Hertz square_wave_frequency = 0;
    static uint8_t error_code = 0;

    /**
     * The implemented cyclic executive works by calculating the slot_start_time (ms).
     * Each task is given an associated period to be executed, and then checked to see
     * if each period is a multiple for the current slot_start_time. If so, the task
     * is executed, otherwise the task is skipped this cycle.
     */
    if (is_task_slot(slot_start_time, TASK_1_PERIOD)) // Task 1: Watchdog Waveform
    {
        Tasks::start_pulse(WATCHDOG_OUTPUT);
        delayMicroseconds(50);
        Tasks::stop_pulse(WATCHDOG_OUTPUT);
    }
    if (is_task_slot(slot_start_time, TASK_2_PERIOD)) // Task 2: Monitor button
    {
        digital_input_state = Tasks::digital_read(DIGITAL_INPUT);
    }
    if (is_task_slot(slot_start_time, TASK_3_PERIOD)) // Task 3: Measure square wave frequency
    {
        square_wave_frequency = Tasks::measure_square_wave_frequency(PWM_PIN);
    }
    if (is_task_slot(slot_start_time, TASK_4_PERIOD)) // Task4: Measure potentiometer voltage
    {
        digitalWrite(ANALOGUE_MONITOR_DISPLAY_PIN, HIGH);
        analogue_readings[analogue_index] = Tasks::analogue_read(ANALOGUE_INPUT);
        digitalWrite(ANALOGUE_MONITOR_DISPLAY_PIN, LOW);
        analogue_index = (analogue_index + 1) % Tasks::NUMBER_OF_ANALOGUE_READINGS; // circually increment counter
                                                                                    // ie. 0,1,2,3,0,1,2,3...
    }
    if (is_task_slot(slot_start_time, TASK_5_PERIOD)) // Task 5: Filter analogue signal
    {

        average_analogue_reading = Tasks::compute_filtered_analogue_signal(analogue_readings);
    }
    if (is_task_slot(slot_start_time, TASK_6_PERIOD)) // Task 6: Execute no operation 1000 times
    {
        Tasks::execute_no_op_instruction(1000);
    }
    if (is_task_slot(slot_start_time, TASK_7_PERIOD)) // Task 7: Compute error code
    {

        error_code = Tasks::compute_error_code(average_analogue_reading);
    }
    if (is_task_slot(slot_start_time, TASK_8_PERIOD)) // Task 8: Visualise error code on LED
    {
        Tasks::visualise_error_code(error_code, ERROR_CODE_LED);
    }
    if (is_task_slot(slot_start_time, TASK_9_PERIOD)) // TAsk 9: Log data to serial port (in CSV format)
    {
        Tasks::log(digital_input_state, square_wave_frequency, average_analogue_reading);
    }
    slot_start_time = (++slot_counter * SLOT_WIDTH);
}

const Milliseconds roundToTheNearestMultiple(const Milliseconds value_to_round, const Milliseconds multiple)
{
    const int64_t int_value = static_cast<int64_t>(round(value_to_round));
    const int64_t int_multiple = static_cast<int64_t>(multiple);

    Milliseconds rounded = ((int_value + int_multiple / 2) / int_multiple) * int_multiple;
    Serial.printf("%f => %f\n", value_to_round, rounded);
    return rounded;
}

void loop()
{
    // put your main code here, to run repeatedly:
}