#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "g_dht.h"

#define DHT_GPIO_PIN 4

static const char *TAG = "DHT11";

void app_main(void) {
  float humidity = 0.0;
  float temperature = 0.0;

  while (1) {
    if (dht_read_float_data(DHT_TYPE_DHT11, DHT_GPIO_PIN, &humidity, &temperature) != ESP_OK) {
      ESP_LOGE(TAG, "Error reading sensor data");
    } else {
      ESP_LOGI(TAG, "Humidity: %.1f%%, Temp: %.1f C", humidity, temperature);
    }
  
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
