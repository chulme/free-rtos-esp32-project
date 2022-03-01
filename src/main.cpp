#include <Arduino.h>
#include <Ticker.h>

#include "common_types.hpp"

constexpr Milliseconds slot_width = 1000;
Ticker slot_ticker;

void callback()
{
    static int counter = 0;
    if (counter % 2 == 0)
    {
        Serial.printf("%ld\tPerforming task every 2s!\n", millis());
    }
    counter++;
}

void setup()
{
    slot_ticker.attach_ms(slot_width, callback);
    Serial.begin(115200);
    Serial.println("Cyclic Executive setup 2 complete!");
}

void loop()
{
    // put your main code here, to run repeatedly:
}