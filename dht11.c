#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"

#include "dhtlib.h"

const uint8_t LED_PIN = 15;
const uint8_t DHT_PIN = 18;

volatile bool led_state = false;
// const uint sda = 2;
// const uint scl = 3;

typedef struct UserData {
    uint16_t a_number;
    char hello[20];
} UserData;

// Create a repeating timer that calls repeating_timer_callback.
// If the delay is > 0 then this is the delay between the previous callback
// ending and the next starting. If the delay is negative (see below) then the
// next call to the callback will be exactly 500ms after the start of the call
// to the last callback
bool repeating_print(struct repeating_timer* t) {
    // casting void * to a Data *
    UserData* data = (UserData*)t->user_data;
    printf("%s %d\n%llu\n", data->hello, data->a_number, time_us_64());
    return true;
}

bool repeating_toggle_led(struct repeating_timer* t) {
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);
    return true;
}

bool initialize_pin(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, true);
}


int main() {
    stdio_init_all();

    sleep_ms(2000);

    printf("SETTING UP");

    initialize_pin(LED_PIN);
    initialize_pin(DHT_PIN);

    struct repeating_timer led_timer;
    add_repeating_timer_ms(500, repeating_toggle_led, NULL, &led_timer);

    // struct repeating_timer dht_timer;
    // bool alarm_set = add_repeating_timer_ms(3000, repeating_measure_dht, NULL, &dht_timer);

    while (true) {
        sleep_ms(3000);

        dht_init_sequence(DHT_PIN);

        gpio_set_dir(DHT_PIN, GPIO_OUT);
        gpio_put(DHT_PIN, true);
    }
}
