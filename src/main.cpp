#include <Arduino.h>
#include <Ticker.h>

#include "common_types.hpp"
#include "tasks.hpp"

// Function forward declarations
void callback();
const Milliseconds roundToTheNearestMultiple(const Milliseconds value_to_round, const Milliseconds multiple);

// Constant expressions
constexpr Milliseconds SLOT_WIDTH = 10;
// Pins
constexpr int8_t ANALOGUE_INPUT = 4;
constexpr int8_t WATCHDOG_OUTPUT = 8;
constexpr int8_t DIGITAL_INPUT = 7;
constexpr int8_t ERROR_CODE_LED = 5;
constexpr int8_t PWM_PIN = 6;
// Task Frequency
constexpr Hertz DEBUG_RATE_AMPLIFIER = 0.01;
constexpr Hertz TASK_2_RATE = 5.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_3_RATE = 1.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_4_RATE = 24.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_5_RATE = 24.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_6_RATE = 10.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_7_RATE = 3.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_8_RATE = 3.0 * DEBUG_RATE_AMPLIFIER;
constexpr Hertz TASK_9_RATE = 0.2 * DEBUG_RATE_AMPLIFIER;

Milliseconds TASK_1_PERIOD = 5.0;
Milliseconds WATCHDOG_LENGTH = 5.0;
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
    slot_ticker.attach_ms(SLOT_WIDTH, callback);
    Serial.begin(115200);
    Serial.println("Cyclic Executive setup 2 complete!");
    Serial.printf("=====Task Periods=====\n 2)%f\n3)%f\n4)%f\n5)%f\n6)%f\n7)%f\n8)%f\n9)%f\n",
                  TASK_2_PERIOD,
                  TASK_3_PERIOD,
                  TASK_4_PERIOD,
                  TASK_5_PERIOD,
                  TASK_6_PERIOD,
                  TASK_7_PERIOD,
                  TASK_8_PERIOD,
                  TASK_9_PERIOD);

    Serial.printf("Slot Width: %f\tDebug Amplifier: %f\n", SLOT_WIDTH, DEBUG_RATE_AMPLIFIER);
    TASK_2_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_2_RATE), SLOT_WIDTH); // 20s
    TASK_3_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_3_RATE), SLOT_WIDTH); // 100
    TASK_4_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_4_RATE), SLOT_WIDTH); // 4.16s
    TASK_5_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_5_RATE), SLOT_WIDTH); // 4.16s
    TASK_6_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_6_RATE), SLOT_WIDTH); // 10s
    TASK_7_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_7_RATE), SLOT_WIDTH); // 33.33s
    TASK_8_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_8_RATE), SLOT_WIDTH); // 33.33s
    TASK_9_PERIOD = roundToTheNearestMultiple(calculateCyclePeriod(TASK_9_RATE), SLOT_WIDTH); // 5s
}

bool is_task_slot(const Milliseconds slot_start_time, const Milliseconds task_period)
{
    return (static_cast<int64_t>((slot_start_time)) % static_cast<int64_t>((task_period)) == 0);
}

void callback()
{
    static int64_t slot_counter = 0;
    static Milliseconds slot_start_time = 0;
    static std::array<uint16_t, Tasks::NUMBER_OF_ANALOGUE_READINGS> analogue_readings = {0, 0, 0, 0};
    static size_t analogue_index = 0;
    static double average_analogue_reading = 0;
    static bool digital_input_state = 0;
    static Hertz square_wave_frequency = 0;
    static uint8_t error_code = 0;

    // if (is_task_slot(slot_start_time, TASK_1_PERIOD))
    // {
    //     Tasks::start_pulse(WATCHDOG_OUTPUT);
    // }
    // if (is_task_slot(slot_start_time, TASK_1_PERIOD + WATCHDOG_LENGTH))
    // {
    //     Tasks::stop_pulse(WATCHDOG_OUTPUT);
    // }
    // if (is_task_slot(slot_start_time, TASK_2_PERIOD))
    // {
    //     digital_input_state = Tasks::digital_read(DIGITAL_INPUT);
    // }
    // if (is_task_slot(slot_start_time, TASK_3_PERIOD))
    // {
    //     square_wave_frequency = Tasks::measure_square_wave_frequency(PWM_PIN);
    // }
    // if (is_task_slot(slot_start_time, TASK_4_PERIOD))
    // {
    //     analogue_readings[analogue_index] = Tasks::analogue_read(ANALOGUE_INPUT);
    //     analogue_index = (analogue_index + 1) % Tasks::NUMBER_OF_ANALOGUE_READINGS; // cyclically increment counter
    // }
    // if (is_task_slot(slot_start_time, TASK_5_PERIOD))
    // {
    //     average_analogue_reading = Tasks::compute_filtered_analogue_signal(analogue_readings);
    // }
    // if (is_task_slot(slot_start_time, TASK_6_PERIOD))
    // {
    //     Tasks::execute_no_op_instruction(1000);
    // }
    // if (is_task_slot(slot_start_time, TASK_7_PERIOD))
    // {
    //     uint16_t maximum_reading = *std::max_element(analogue_readings.begin(), analogue_readings.end());
    //     error_code = Tasks::compute_error_code(average_analogue_reading, maximum_reading);
    // }
    // if (is_task_slot(slot_start_time, TASK_8_PERIOD))
    // {
    //     Tasks::visualise_error_code(error_code, ERROR_CODE_LED);
    // }
    // if (is_task_slot(slot_start_time, TASK_9_PERIOD))
    // {
    //     Tasks::log(digital_input_state, square_wave_frequency, average_analogue_reading);
    // }
    slot_start_time = (++slot_counter * SLOT_WIDTH);
}

const Milliseconds roundToTheNearestMultiple(const Milliseconds value_to_round, const Milliseconds multiple)
{
    int64_t remainder = static_cast<int64_t>(round(value_to_round)) % static_cast<int64_t>(round(multiple));
    Milliseconds rounded_value = ((remainder == 0)
                                      ? value_to_round
                                      : static_cast<Milliseconds>(static_cast<int64_t>(round(value_to_round)) + static_cast<int64_t>(round(multiple)) - remainder));
    Serial.printf("%f => %f\n", value_to_round, rounded_value);
    return rounded_value;
}

void loop()
{
    // put your main code here, to run repeatedly:
}