/****************************************
 * Wifi Tracker	- main function			*
 * Sparkfun ESP32 Thing					*
 * (c) 2017								*
 ****************************************/

#include "inits.h"
#include "scan.h"
#include "snif.h"
#include "scansnif.h"

#define	LED_GPIO_PIN					GPIO_NUM_5
#define	WIFI_CHANNEL_MAX				(13)
#define	WIFI_CHANNEL_SWITCH_INTERVAL	(1000)


void app_main(void) {

	uint8_t level = 0, channel = 1;

	esp_event_loop_init(event_handler, NULL);
	esp_wifi_set_country(WIFI_COUNTRY_EU);
	gpio_set_direction(LED_GPIO_PIN, GPIO_MODE_OUTPUT);

	//Fill MAC and AP table
	int i = 0, j = 0;
	for(i = 1; i < 250; i++){
	    for(j = 0; j < 18; j++){
        	macTable[i][j] = 'M';
        	APTable[i][j] = 'A';
        }
        //Fill RSSI table
        for(j = 0; j < 4; j++){
        	rssiTable[i][j] = 'r';
        }
        //Fill AP flag table
        APflag[i][0] = '0';
    }

	wifi_scan_init();
	scan_ap_task(0);
	wifi_sniffer_init();	

	while (true) {
		for(i = 0; i < 13; i++){
			gpio_set_level(LED_GPIO_PIN, level ^= 1);
			vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
			wifi_sniffer_set_channel(channel);
			channel = (channel % WIFI_CHANNEL_MAX) + 1;
			esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
		}
		
		printf("\nuit de for loop\n");
		wifi_scan_init();
		scan_ap_snif(0);
		wifi_sniffer_init();
		vTaskDelay(5000 / portTICK_PERIOD_MS);
   	}
}
