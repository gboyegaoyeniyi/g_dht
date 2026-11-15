#include "g_dht.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include <esp_rom_sys.h>
#include <freertos/FreeRTOS.h>
#include <stdint.h>
#include <stdlib.h>

#define DHT_TIMER_INTERVAL 2
#define DHT_DATA_BITS 40
#define DHT_DATA_BYTES (DHT_DATA_BITS / 8)

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux)
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux)

static const char *TAG = "dht";

/**
 * Wait for specified time in timeout for the pin signal to reach the expected
 * target state If state is not reached until after time is exhausted, returns a
 * timeout error If state is reached, return ESP_OK If duration pointer is
 * provided, load the time taking for the state to reach to the duration.
 */
static esp_err_t dht_await_pin_state(gpio_num_t pin, uint32_t timeout,
                                     int target_pin_state, uint32_t *duration) {
  // Prepare pin to receive signal
  gpio_set_direction(pin, GPIO_MODE_INPUT);
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
}

/**
 * Steps through the phases of inititalizing the dht sensor
 * Though I feel like this switch statements probably wastes time
 * since am using a loop to call it later on
 */
static esp_err_t step_dht_init_phase(gpio_num_t pin, char phase) {
  switch (phase) {
  case 'B':
    return dht_await_pin_state(pin, 40, 0, NULL);
  case 'C':
    return dht_await_pin_state(pin, 88, 1, NULL);
  case 'D':
    return dht_await_pin_state(pin, 88, 0, NULL);
  }

  return ESP_ERR_INVALID_ARG;
}


/**
 * Initialized communication with the dht sensor and retrive the 40 bits data
 */
static esp_err_t dht_fetch_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                                uint8_t data[DHT_DATA_BYTES]) {
  uint32_t low_duration = 0;
  uint32_t high_duration = 0;
  esp_err_t err = ESP_OK;

  // Phase A
  gpio_set_direction(pin,
                     GPIO_MODE_OUTPUT_OD); // set pin direction to open-drain to
                                           // prepare for LOW pull
  gpio_set_level(pin, 0);
  esp_rom_delay_us(20000); // delaying the process for 20mu as in 20
                           // microseconds, while have set level to low this
                           // keeps pulling for at least 18ms until i release
  // the delay also gurantees no background process interupt the pulling
  gpio_set_level(pin, 1);

  // port critical to ensure absoluitely that the cpu interuupt do not interrup
  // this time critical operation
  PORT_ENTER_CRITICAL();

  const char phases[] = {'B', 'C', 'D'};
  for (int i = 0; i < 3; i++) {
    err = step_dht_init_phase(pin, phases[i]);
    if (err != ESP_OK) {
      PORT_EXIT_CRITICAL();
      ESP_LOGE(TAG, "Initialization error at phase '%c'", phases[i]);
      return err;
    }
  }

  for (int i = 0; i < DHT_DATA_BITS; i++) {
    // Read LOW
    if (dht_await_pin_state(pin, 65, 1, &low_duration) != ESP_OK) {
      PORT_EXIT_CRITICAL();
      ESP_LOGE(TAG, "LOW bit timeout");
      return ESP_ERR_TIMEOUT;
    }

    // Read HIGH
    if (dht_await_pin_state(pin, 75, 0, &high_duration) != ESP_OK) {
      PORT_EXIT_CRITICAL();
      ESP_LOGE(TAG, "HIGH bit timeout");
      return ESP_ERR_TIMEOUT;
    }

    uint8_t b = i / 8;
    uint8_t m = i % 8;

    if (!m)
      data[b] = 0;

    data[b] |= (high_duration > low_duration) << (7 - m);
  }

  PORT_EXIT_CRITICAL();
  return ESP_OK;
}

static int16_t dht_convert_data(dht_sensor_type_t sensor_type, uint8_t msb,
                                uint8_t lsb) {

  (void)sensor_type;
  (void)lsb;
  return (int16_t)(msb * 10);
};

esp_err_t dht_read_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                        int16_t *humidity, int16_t *temperature) {
  if (!humidity && !temperature)
    return ESP_ERR_INVALID_ARG;

  if (sensor_type != DHT_TYPE_DHT11)
    return ESP_ERR_NOT_SUPPORTED;

  uint8_t data[DHT_DATA_BYTES] = {0};

  gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
  gpio_set_level(pin, 1);

  esp_err_t result = dht_fetch_data(sensor_type, pin, data);
  // reseting pin direction as it would have been changed while fetching the
  // data
  gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD);
  gpio_set_level(pin, 1);

  if (result != ESP_OK)
    return result;

  // Verify checksum, checksum is on the 5th byte
  if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xff)) {
    ESP_LOGE(TAG, "Checksum failed, invalid data from sensor");
    return ESP_ERR_INVALID_CRC;
  }

  if (humidity)
    *humidity = dht_convert_data(sensor_type, data[0], data[1]);

  if (temperature)
    *temperature = dht_convert_data(sensor_type, data[2], data[3]);

  return ESP_OK;
}

esp_err_t dht_read_float_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                              float *humidity, float *temperature) {
  if (!humidity && !temperature)
    return ESP_ERR_INVALID_ARG;

  int16_t i_humidity = 0, i_temp = 0;
  esp_err_t result =
      dht_read_data(sensor_type, pin, humidity ? &i_humidity : NULL,
                    temperature ? &i_temp : NULL);
  if (result != ESP_OK)
    return result;

  if (humidity)
    *humidity = i_humidity / 10.0f;

  if (temperature)
    *temperature = i_temp / 10.0f;

  return ESP_OK;
}
