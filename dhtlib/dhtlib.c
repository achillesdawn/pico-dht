#include "dhtlib.h"

#include <stdint.h>
#include <stdio.h>

#include "pico/stdlib.h"


typedef struct Dht11Data {
    int8_t temperature;
    int8_t humidity; 
} Dht11Data;

typedef struct Dht22Data {
    int16_t temperature;
    int16_t humidity;
} Dht22Data;

bool dht22 = false;

void print_array(uint8_t arr[], uint8_t length) {
    printf("\n[ ");
    for (int i = 0; i < length; i++) {
        printf("%d, ", arr[i]);
    }
    printf("]\n");
}

void print_data(Dht22Data data) {
    printf("Temp: %d\n", data.temperature);
    printf("Humidity: %d\n", data.humidity);
}

bool validate(uint8_t arr[]) {
    uint8_t validation = arr[0] + arr[1] + arr[2] + arr[3];
    return arr[4] == validation;
}

Dht22Data dht22_convert(uint8_t arr[]) {

    Dht22Data data;

    data.humidity = arr[0];
    data.humidity = data.humidity << 8;
    data.humidity |= arr[1];

    data.temperature = arr[2];
    data.temperature = data.temperature << 8;
    data.temperature |= arr[3];
    return data;
}

bool wait_for_value(uint8_t dht, bool wait_value) {
    bool value = !wait_value;

    for (uint8_t i = 0; i < 30; i++) {
        if (value == wait_value) {
            return false;
        } else {
            sleep_us(3);
            value = gpio_get(dht);
        }
    }
    return true;
}

void dht_read_sequence(uint8_t DHT_PIN) {
    uint8_t data[5] = {0};
    bool value;
    uint8_t values_read = 0;

    for (uint8_t pos = 0; pos < 5; pos++) {
        // printf("\npos: %d \n", pos);

        for (uint8_t mask = 0; mask < 8; mask++) {
            sleep_us(30);

            value = gpio_get(DHT_PIN);

            values_read += 1;

            data[pos] |= value << (7 - mask);

            if (value == 0) {
                if (wait_for_value(DHT_PIN, 1)) {
                    printf("mask %d pos %d got 0 waited for 1, returned", mask,
                           pos);
                    return;
                };

            } else {
                if (wait_for_value(DHT_PIN, 0)) {
                    printf("mask %d pos %d got 1 waited for 0, returned", mask,
                           pos);
                    return;
                } else {
                    if (wait_for_value(DHT_PIN, 1)) {
                        printf("mask %d pos %d got 1 waited for 1, returned",
                               mask, pos);
                        return;
                    };
                }
            }
        }
    }

    printf("read done, values read: %d\n", values_read);

    if (validate(data)) {
        print_array(data, 5);
        Dht22Data struct_data = dht22_convert(data);
        print_data(struct_data);
    };
}

void dht_init_sequence(uint8_t DHT_PIN) {
    printf("\n\n");
    printf("initializing dht sequence\n");

    gpio_put(DHT_PIN, false);
    sleep_ms(18);

    gpio_set_dir(DHT_PIN, GPIO_IN);
    gpio_pull_up(DHT_PIN);
    sleep_us(20);

    bool value;

    for (uint8_t i = 0; i < 4; i++) {

        value = gpio_get(DHT_PIN);

        if (!value) {
            printf("got 0, starting read sequence\n");

            if (wait_for_value(DHT_PIN, 1)) {
                printf("initialiaziation failed");
                return;
            }

            if (wait_for_value(DHT_PIN, 0)) {
                printf("initialiaziation failed");
                return;
            };

            if (wait_for_value(DHT_PIN, 1)) {
                printf("initialiaziation failed");
                return;
            };

            dht_read_sequence(DHT_PIN);
            break;
        }
        sleep_us(20);
    }
    printf("dht done, returning\n");
}