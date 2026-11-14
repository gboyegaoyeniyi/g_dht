#include "g_dht.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include <esp_rom_sys.h>
#include <freertos/FreeRTOS.h>
#include <stdint.h>

#define DHT_TIMER_INTERVAL 2
#define DHT_DATA_BITS 40
#define DHT_DATA_BYTES (DHT_DATA_BITS / 8)

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux);
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux);

/**
 * Wait for specified time in timeout for the pin signal to reach the expected target state
 * If state is not reached until after time is exhausted, returns a timeout error
 * If state is reached, return ESP_OK
 * If duration pointer is provided, load the time taking for the state to reach to the duration.
 */
static esp_err_t dht_await_pin_state(gpio_num_t pin, uint32_t timeout,
                                     int target_pin_state, uint32_t *duration) {
  // Prepare pin to receive signal
  gpio_set_direction(pin, GPIO_MODE_DEF_INPUT);
  for (uint32_t i = 0; i < timeout; i += DHT_TIMER_INTERVAL) {

    esp_rom_delay_us(
        DHT_TIMER_INTERVAL); // delay process for timer interval @ us
    // to avoid measuring jitters or electronic noise
    // get current pin level at time step and compare to target level or state
    if (gpio_get_level(pin) == target_pin_state) {
      if (duration)
        *duration = i;
      return ESP_OK;
    }
  }

  return ESP_ERR_TIMEOUT;
};

static esp_err_t fetch_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                            uint8_t data[DHT_DATA_BYTES]) {};

static esp_err_t convert_data(dht_sensor_type_t sensor_type, uint8_t msb,
                              uint8_t lsb) {};

esp_err_t dht_read_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                        int16_t *humidity, int16_t *temperature) {};

esp_err_t dht_read_float_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                              int16_t *humidity, int16_t *temperature) {};
