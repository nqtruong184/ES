	#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include <time.h>

#include "freertos/timers.h"
#include "freertos/FreeRTOSconfig.h"
#include "driver/gpio.h"

#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

volatile uint8_t scanned = 0;

esp_err_t event_handler(void *ctx, system_event_t* event) {
    printf("Event occurred\n");
    switch (event->event_id) {
        case SYSTEM_EVENT_SCAN_DONE:
            printf("Event 1 occurred\n");
            printf("Number of access points found : %d\n",
            event->event_info.scan_done.number);
            uint16_t apCount = event->event_info.scan_done.number;
            if (apCount == 0) {
                printf("apCount: %d\n", apCount);
                return ESP_OK;
            }
            wifi_ap_record_t * list = (wifi_ap_record_t *) malloc(sizeof(wifi_ap_record_t) * apCount );
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(& apCount, list));
            int i;
            printf("apCount: %d\n", apCount);
            for (i = 0; i < apCount; i++) {
                char * authmode ;
                switch (list[i].authmode) {
                    case WIFI_AUTH_OPEN :
                        authmode = "WIFI_AUTH_OPEN";
                        break;
                    case WIFI_AUTH_WEP:
                        authmode = "WIFI_AUTH_WEP";
                        break;
                    case WIFI_AUTH_WPA_PSK :
                        authmode = " WIFI_AUTH_WPA_PSK ";
                        break ;
                    case WIFI_AUTH_WPA2_PSK:
                        authmode = "WIFI_AUTH_WPA2_PSK";
                        break;
                    case WIFI_AUTH_WPA_WPA2_PSK:
                        authmode = "WIFI_AUTH_WPA_WPA2_PSK";
                        break;
                    default:
                        authmode = "Unknown";
                        break;
                }
            printf("ssid =%s, rssi =%d, authmode =%s\n", list[i].ssid, list[i].rssi, authmode);
            }
            free(list);
            scanned = 1;
            ESP_ERROR_CHECK(esp_wifi_stop());
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            printf("Event 2 occurred\n");
            printf("Our IP address is " IPSTR "\n", IP2STR(&event->event_info.got_ip.ip_info.ip));
            printf("We have now connected to a station and can do things...\n");
            break;
        case SYSTEM_EVENT_STA_START:
            printf("Event 3 occurred\n");
            if (scanned == 1) {
                ESP_ERROR_CHECK(esp_wifi_connect());
            }
            break;
        default:
            printf("Event x occurred\n");
            break;
    }
    if (event->event_id == SYSTEM_EVENT_SCAN_DONE) {

    }
    if (event->event_id == SYSTEM_EVENT_STA_GOT_IP) {

    }
    if (event->event_id == SYSTEM_EVENT_STA_START) {

    }
    return ESP_OK;
}

int app_main(void) {
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(& cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_scan_config_t scanConf = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = 1
    };
    wifi_config_t sta_config = {
        .sta = {
            .ssid = "netfpga",
            .password = "ktmt201_c5",
            .bssid_set = 0
        }
    };
    ESP_ERROR_CHECK(esp_wifi_start());
    // Let us test a WiFi scan ...
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConf, 0));
    while (scanned == 0);
    // -----------------------------------------------------------------------
    // ESP_ERROR_CHECK(esp_wifi_init(& cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    return 0;
}
