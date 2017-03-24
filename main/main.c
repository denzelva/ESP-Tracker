/****************************************
 * Wifi Tracker				*
 * Sparkfun ESP32 Thing			*
 * (c) 2017				*
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

#define	LED_GPIO_PIN					GPIO_NUM_5
#define	WIFI_CHANNEL_MAX				(13)
#define	WIFI_CHANNEL_SWITCH_INTERVAL	(1000)

struct snif {
	char mac[18];
	int rssi;
	bool AP;
} list[250], APlist[250];


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

void scan_ap_task(void *pvParameters)
{
	uint16_t n = 0;
	int i = 0, j = 0, x = 0, d = 0;
	char APadres[18] = "";	
		wifi_ap_record_t *al;
		esp_wifi_scan_start(NULL, true);
		esp_wifi_scan_get_ap_num(&n);

		printf("%d AP's found\r\n", n);
		al = malloc(n * sizeof(wifi_ap_record_t));
		if (al == NULL) {
			return;
		}
		if (esp_wifi_scan_get_ap_records(&n, al) == ESP_OK) {
			for (i = 0; i < n; i++) {
				uint8_t *bi = al[i].bssid;
				printf
					("%32s (%02x:%02x:%02x:%02x:%02x:%02x) rssi: %02d\r\n",
					 al[i].ssid, MAC2STR(bi), al[i].rssi);

					sprintf(APadres, "%02x:%02x:%02x:%02x:%02x:%02x", MAC2STR(bi));
					
					while(APlist[x].mac[0] != 'R'){
					    x++;
					}
					
					//Check duplicates in AP list
					d = 0;
					for(j = 1; j < x; j++){
					    if(strcmp(APlist[j].mac, APadres) == 0){
							d++;
					    }
					}
					printf("%d\n", d);
					if(d == 0){
						strcpy(&APlist[x].mac[0], APadres);				
						APlist[x].rssi = al[i].rssi;
						APlist[x].AP = true;	
					}

			}
			
			//Print APlist
			for(i = 1; i <= x; i++){
				for(j = 0; j < 18; j++){
					printf("%c", APlist[i].mac[j]);
				}
				printf(" %d %d \n", APlist[i].rssi, APlist[i].AP);
			}
		
		}

		free(al);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
}


void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{
	const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buff;
	const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
	const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;

	char temp[3];
	char adres[18] = "";
	int i = 0, j = 0, x = 0, y = 0, d = 0, APcount = 0;

	//MAC: hex to string
	for(int a = 0; a < 6; a++){
	    sprintf(temp, "%02x", hdr->addr2[a]);
	    if(a == 5){
		strcat(adres, temp);
	    }else{
		strcat(adres, temp);
		strcat(adres, ":");
	    }
	}

	while(list[x].mac[0] != 'M'){
	    x++;
	    if(x == 250)
			return;
	}
	
	//Check duplicates
	d = 0;
	for(y = 0; y < x; y++){
	    if(strcmp(list[y].mac, adres) == 0){
			list[y].rssi = ppkt->rx_ctrl.rssi;
			d++;
	    }
	}
	
	//AP flag
	for(y = 0; y < 250; y++){
	    if(strcmp(APlist[y].mac, adres) == 0){
			for(i = 0; i < x; i++){
	    		if(strcmp(list[i].mac, adres) == 0){
					list[i].AP = true;
	    		}
			}
	    }
	}

	if(d != 0)
		return;
	
	
	//Copy MAC and RSSI in tables
	strcpy(&list[x].mac[0], adres);
	list[x].rssi = ppkt->rx_ctrl.rssi;

	//Clear Terminal
	//printf("\033c");
	
	//Print APlist
	for(i = 1; i <= x; i++){
		for(j = 0; j < 18; j++){
			printf("%c", list[i].mac[j]);
		}
		printf(" %d %d \n", list[i].rssi, list[i].AP);
		if(list[i].AP == true)
			APcount++;
	}

	printf("\n%d devices including %d AP's.\n", x, APcount);
}

void app_main(void) {

	uint8_t level = 0, channel = 1;

	esp_event_loop_init(event_handler, NULL);
	esp_wifi_set_country(WIFI_COUNTRY_EU);
	gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

	//Fill MAC and AP table
	int i = 0, j = 0;
	for(i = 1; i < 250; i++){
	    for(j = 0; j < 18; j++){
        	list[i].mac[j] = 'M';
			APlist[i].mac[j] = 'R';
        }
    }

	printf("\033c");
	
	while (true) {
		wifi_scan_init();
	
		for(i = 0; i < 3; i++)
			scan_ap_task(0);
	
		wifi_sniffer_init();
	
		for(i = 0; i < 13; i++){
			gpio_set_level(LED_GPIO_PIN, level ^= 1);
			vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
			wifi_sniffer_set_channel(channel);
			channel = (channel % WIFI_CHANNEL_MAX) + 1;
			esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
			printf("\nChannel %d\n", i);
		}
		vTaskDelay(5000 / portTICK_PERIOD_MS);
   	}
}
