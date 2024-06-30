#include <stdint.h>
#include <stdbool.h>

typedef struct DhtData {
    float humidity;
    float temperature;
} DhtData;

DhtData* dht_init_sequence();
