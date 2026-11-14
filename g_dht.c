#include "g_dht.h"
#include "esp_err.h"
#include "soc/gpio_num.h"
#include <freertos/FreeRTOS.h>
#include <stdint.h>

#define DHT_TIMER_INTERVAL 2
#define DHT_DATA_BITS 40
#define DHT_DATA_BYTES (DHT_DATA_BITS / 8)

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux);
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux);

static esp_err_t dht_aware_pin_state(gpio_num_t pin, uint32_t timeout,
                                     int target_pin_state,
                                     uint32_t *duration) {};
static esp_err_t fetch_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                            uint8_t data[DHT_DATA_BYTES]) {};

static esp_err_t convert_data(dht_sensor_type_t sensor_type, uint8_t msb,
                              uint8_t lsb) {};
esp_err_t dht_read_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                        int16_t *humidity, int16_t *temperature) {};

esp_err_t dht_read_float_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                              int16_t *humidity, int16_t *temperature) {};
