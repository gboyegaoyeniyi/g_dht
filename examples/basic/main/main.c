#include "esp_err.h"
#include "esp_log.h"
#include "g_dht.h"

#define DHT_GPIO_PIN 4

static const char *TAG = "DHT11";

void app_main(void) {
  dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;

  float humidity = 0.0;
  float temperature = 0.0;

  esp_err_t err =
      dht_read_float_data(sensor_type, DHT_GPIO_PIN, &humidity, &temperature);
  if (err != ESP_OK)
    ESP_LOGE(TAG, "Error reading sensor data");

  ESP_LOGI(TAG, "Humidity: %.1f%%, Temp: %.1f C%", humidity, temperature);
}
