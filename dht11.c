#include <inttypes.h>
#include <stdio.h>

#include "dhtlib.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

const uint8_t led = 15;
const uint8_t button = 13;
const uint8_t dht = 16;

// const uint sda = 2;
// const uint scl = 3;


int64_t alarm_callback(alarm_id_t alarm_id, void *user_data) {
    for (int i = 0; i < 5; i++) {
        gpio_put(led, 1);
        busy_wait_ms(50);
        gpio_put(led, 0);
        busy_wait_ms(50);
    }
    gpio_set_irq_enabled(button, GPIO_IRQ_EDGE_FALL, true);
    return 0;
}

void gpio_callback(uint gpio, uint32_t events) {
    switch (gpio) {
        case button:
            gpio_set_irq_enabled(button, GPIO_IRQ_EDGE_FALL, false);
            for (int i = 1; i < 4; i++) {
                gpio_put(led, 1);
                busy_wait_ms(50);
                gpio_put(led, 0);
                busy_wait_ms(50);
            }
            gpio_set_irq_enabled(button, GPIO_IRQ_EDGE_FALL, true);
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

int main() {
    stdio_init_all();

    printf("ready to go");

    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);

    gpio_init(dht);
    gpio_set_dir(dht, GPIO_OUT);
    gpio_put(dht, true);

    init_button(button);

    gpio_set_irq_enabled_with_callback(
        button, GPIO_IRQ_EDGE_FALL, true,
        &gpio_callback
    );

    // gpio_set_irq_enabled(
    //     button2,
    //     GPIO_IRQ_EDGE_FALL,
    //     true);

    absolute_time_t start;
    int64_t diff;

    sleep_ms(1000);

    while (true) {
        start = get_absolute_time();

        dht_init_sequence(dht);

        gpio_set_dir(dht, GPIO_OUT);
        gpio_put(dht, true);

        diff = absolute_time_diff_us(start, get_absolute_time());
        printf("time microseconds: %d", diff);

        sleep_ms(10 * 1000);
    };
}
