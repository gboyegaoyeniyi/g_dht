#ifndef G_DHT_H
#define G_DHT_H

#include "soc/gpio_num.h"
#include <driver/gpio.h>
#include <esp_err.h>
#include <stdint.h>

typedef enum {
  DHT_TYPE_DHT11 = 0,
} dht_sensor_type_t;

esp_err_t dht_read_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                        int16_t *humidity, int16_t *temperature);

esp_err_t dht_read_float_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                              float *humidity, float *temperature);

#endif
