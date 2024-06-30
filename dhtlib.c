#include "dhtlib.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "pico/stdlib.h"

#define DHT_PIN 18


volatile bool ready = false;

void print_array(uint8_t arr[], uint8_t length) {
    printf("[ ");
    for (int i = 0; i < length; i++) {
        printf("%d, ", arr[i]);
    }
    printf("]\n");
}

void print_data(DhtData data) {
    printf("Temp: %f\n", data.temperature);
    printf("Humidity: %f\n", data.humidity);
}

bool validate(uint8_t arr[]) {
    uint8_t validation = arr[0] + arr[1] + arr[2] + arr[3];
    return arr[4] == validation;
}

DhtData dht11_convert(uint8_t arr[]) {

    DhtData data;

    data.humidity = ((float)arr[1] / 10) + arr[0];
    data.temperature = ((float)arr[3] / 10) + arr[2];

    return data;
}

DhtData* dht22_convert(uint8_t arr[]) {

    DhtData* dht_data = (DhtData*)malloc(sizeof(DhtData));

    dht_data->humidity = (arr[0] << 8) + arr[1];
    dht_data->humidity /= 10;
    dht_data->temperature = (arr[2] << 8) + arr[3];
    dht_data->temperature /= 10;

    return dht_data;
}

/// @brief waits for a value read at the gpio pin within a time limit
/// @param dht pin to read from 
/// @param wait_value 
/// @return returns true if value was read within time limit, else returns false
bool wait_for_value(bool wait_value) {
    bool value;

    for (uint8_t i = 0; i < 30; i++) {
        value = gpio_get(DHT_PIN);

        if (value == wait_value) {
            return true;
        }
        sleep_us(3);
    }
    return false;
}

void gpio_callback(uint gpio, uint32_t event_mask) {

    if (gpio == 18) {
        ready = true;
    }
}

void start_sequence() {
    gpio_put(DHT_PIN, false);
    sleep_ms(19);

    gpio_set_dir(DHT_PIN, GPIO_IN);
    gpio_pull_up(DHT_PIN);
    sleep_us(20);

    if (!wait_for_value(0)) {
        printf("Initiation failed");
        return;
    }

    if (!wait_for_value(1)) {
        printf("initialiaziation failed");
        return;
    }
}

void set_irq(bool enable) {
    if (enable) {
        gpio_set_irq_enabled(DHT_PIN, GPIO_IRQ_EDGE_RISE, true);
        irq_set_enabled(IO_IRQ_BANK0, true);
        gpio_set_irq_callback(gpio_callback);
    }
    else {
        gpio_set_irq_enabled(DHT_PIN, GPIO_IRQ_EDGE_RISE, false);
        irq_set_enabled(IO_IRQ_BANK0, false);
    }
}

void end_sequence() {
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, true);
}

DhtData* dht_init_sequence() {

    printf("initializing dht sequence\n");

    start_sequence();

    set_irq(true);

    uint8_t data[5] = { 0, 0, 0, 0 ,0 };
    uint8_t bits_read = 0;
    bool signal = true;
    ready = false;

    while (bits_read < 40) {
        if (ready) {
            sleep_us(30);
            signal = gpio_get(DHT_PIN);

            data[bits_read / 8] |= signal << (7 - (bits_read % 8));
            bits_read += 1;
            ready = false;
        }
        else {
            sleep_us(3);
        }
    }

    set_irq(false);
    end_sequence();

    if (validate(data)) {
        print_array(data, 5);
 
        DhtData* dht_data = dht22_convert(data);
        print_data(*dht_data);

        return dht_data;
    }
    else {
        printf("Validation Failed\n");
        print_array(data, 5);
        return NULL;
    }
}