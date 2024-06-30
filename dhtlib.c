#include "dhtlib.h"

#include <stdint.h>
#include <stdio.h>

#include "pico/stdlib.h"


typedef struct Dht11Data {
    float humidity;
    float temperature;
} Dht11Data;

bool dht22 = false;

void print_array(uint8_t arr[], uint8_t length) {
    printf("\n[ ");
    for (int i = 0; i < length; i++) {
        printf("%d, ", arr[i]);
    }
    printf("]\n");
}

void print_data(Dht11Data data) {
    printf("Temp: %f\n", data.temperature);
    printf("Humidity: %f\n", data.humidity);
}

bool validate(uint8_t arr[]) {
    uint8_t validation = arr[0] + arr[1] + arr[2] + arr[3];
    return arr[4] == validation;
}

Dht11Data dht11_convert(uint8_t arr[]) {

    Dht11Data data;

    data.humidity = ((float)arr[1] / 10) + arr[0];
    data.temperature = ((float)arr[3] / 10) + arr[2];

    return data;
}

/// @brief waits for a value 
/// @param dht pin to read from 
/// @param wait_value 
/// @return returns true if value was read within time limit, else returns false
inline bool wait_for_value(uint8_t dht, bool wait_value) {
    bool value;

    for (uint8_t i = 0; i < 30; i++) {
        value = gpio_get(dht);

        if (value == wait_value) {
            return true;
        }
        sleep_us(3);
    }
    return false;
}

void dht_read_sequence(uint8_t DHT_PIN) {
    uint8_t data[5] = { 0, 0, 0, 0 ,0 };

    uint8_t bits_read = 0;
    bool signal = true;

    for (uint8_t byte = 0; byte < 5; byte++) {
        for (uint8_t bit = 0; bit < 8; bit++) {
            
            // if we are at a high bit (1), then wait for a low bit (0)
            if (signal) {
                if (!wait_for_value(DHT_PIN, 0)) {
                    printf("waiting for 0 took too long\n");
                    printf("reading byte %d\n", byte);
                    return;
                }
            }
            // when we get to a low bit, wait for high a bit
            if (!wait_for_value(DHT_PIN, 1)) {
                printf("waiting for 1 took too long\n");
                printf("reading byte %d\n", byte);
                return;
            }

            // if signal is high for more than 30us, then its a 1 bit, else its a 0 bit
            sleep_us(30);

            signal = gpio_get(DHT_PIN);

            bits_read += 1;

            data[byte] |= signal << (7 - bit);
        }
    }

    printf("read done, values read: %d\n", bits_read);

    if (validate(data)) {
        print_array(data, 5);
        Dht11Data struct_data = dht11_convert(data);
        print_data(struct_data);
    }
    else {
        printf("Validation Failed");
        print_array(data, 5);
    }
}

void dht_init_sequence(uint8_t DHT_PIN) {
    printf("initializing dht sequence\n");

    gpio_put(DHT_PIN, false);
    sleep_ms(19);

    gpio_set_dir(DHT_PIN, GPIO_IN);
    gpio_pull_up(DHT_PIN);
    sleep_us(20);

    if (!wait_for_value(DHT_PIN, 0)) {
        printf("Initiation failed");
        return;
    }

    if (!wait_for_value(DHT_PIN, 1)) {
        printf("initialiaziation failed");
        return;
    }

    dht_read_sequence(DHT_PIN);

    printf("dht done, returning\n");
}