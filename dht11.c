#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "dhtlib.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const uint8_t LED = 15;
const uint8_t BUTTON = 13;
const uint8_t DHT = 16;

volatile bool led_state = false;
// const uint sda = 2;
// const uint scl = 3;


int64_t alarm_callback(alarm_id_t alarm_id, void *user_data) {
    for (int i = 0; i < 5; i++) {
        gpio_put(LED, 1);
        busy_wait_ms(50);
        gpio_put(LED, 0);
        busy_wait_ms(50);
    }
    gpio_set_irq_enabled(BUTTON, GPIO_IRQ_EDGE_FALL, true);
    return 0;
}

void gpio_callback(uint gpio, uint32_t events) {
    switch (gpio) {
        case BUTTON:
            gpio_set_irq_enabled(BUTTON, GPIO_IRQ_EDGE_FALL, false);
            for (int i = 1; i < 4; i++) {
                gpio_put(LED, 1);
                busy_wait_ms(50);
                gpio_put(LED, 0);
                busy_wait_ms(50);
            }
            gpio_set_irq_enabled(BUTTON, GPIO_IRQ_EDGE_FALL, true);
            break;
        default:
            break;
    }
}

void init_display() {
    // i2c_init(i2c1, 400000);
    // gpio_set_function(sda, GPIO_FUNC_I2C);
    // gpio_set_function(scl, GPIO_FUNC_I2C);
    // gpio_pull_up(sda);
    // gpio_pull_up(scl);

    // ssd1306_t display;
    // display.external_vcc = false;

    // ssd1306_init(&display, 128, 32, 0x3C, i2c1);
    // ssd1306_clear(&display);
}

void init_button(uint button) {
    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_down(button);
}

typedef struct UserData {
    uint16_t a_number;
    char hello[20];
} UserData;

// Create a repeating timer that calls repeating_timer_callback.
// If the delay is > 0 then this is the delay between the previous callback ending and the next starting.
// If the delay is negative (see below) then the next call to the callback will be exactly 500ms after the
// start of the call to the last callback
bool repeating_timer_callback(struct repeating_timer *t) {
    // casting void * to a Data *
    UserData *data = (UserData *)t->user_data;
    printf("%s %d\n%llu\n", data->hello, data->a_number, time_us_64());
    return true;
}

bool toggle_led_repeating_callback(struct repeating_timer *t) {
    led_state = !led_state;
    gpio_put(LED, led_state);
    return true;
}

int main() {
    stdio_init_all();

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);
    gpio_put(LED, false);

    printf("SETTING UP");

    struct repeating_timer timer;
    UserData data;
    data.a_number = 42;
    strcpy(data.hello, "Hello World<<>>");

    add_repeating_timer_ms(500, repeating_timer_callback, &data, &timer);
    
    struct repeating_timer led_timer;
    add_repeating_timer_ms(500, toggle_led_repeating_callback, NULL, &led_timer);

    while (true) {
        tight_loop_contents();
    }
}
