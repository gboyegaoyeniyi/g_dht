#include "g_dht.h"
#include <ets_sys.h>
#include <freertos/FreeRTOS.h>

#define DHT_TIMER_INTERVAL 2
#define DHT_DATA_BITS 40
#define DHT_DATA_GYTES (DHT_DATA_BITS / 8)

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux);
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux);

void func(void) {}
