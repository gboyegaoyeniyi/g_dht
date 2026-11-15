# G_DHT

esp-idf driver for dht sensor - dht11

```c
typedef enum {
  DHT_TYPE_DHT11 = 0,
} dht_sensor_type_t;

esp_err_t dht_read_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                        int16_t *humidity, int16_t *temperature);

esp_err_t dht_read_float_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                              float *humidity, float *temperature);
```

Love from me