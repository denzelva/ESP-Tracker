/****************************************
 * Wifi Tracker	- inits & includes		*
 * Sparkfun ESP32 Thing					*
 * (c) 2017								*
 ****************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

char macTable[250][18];
char rssiTable[250][4];
char APTable[250][18];
char APflag[250][1];

int e = 0;

typedef struct {
	unsigned frame_ctrl:16;
	unsigned duration_id:16;
	uint8_t addr1[6]; /* receiver address */
	uint8_t addr2[6]; /* sender address */
	uint8_t addr3[6]; /* filtering address */
	unsigned sequence_ctrl:16;
	uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
	wifi_ieee80211_mac_hdr_t hdr;
	uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
   return ESP_OK;
}

void wifi_sniffer_set_channel(uint8_t channel)
{
	esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

void wifi_sniffer_init(void)
{
	nvs_flash_init();
   	tcpip_adapter_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
    ESP_ERROR_CHECK( esp_wifi_start() );
	esp_wifi_set_promiscuous(true);
}

void wifi_scan_init(void){
	nvs_flash_init();
	tcpip_adapter_init();
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
	esp_wifi_set_promiscuous(false);

}
