
#include <stdio.h>
#include <time.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif


void vTaskFunction(void* pvParameters) {
    char* pcTaskName;
    pcTaskName = (char*)pvParameters;
    for (;; ) {
        for (int i = 0; i < 1000; i++)
          printf("%s: %ld\n", pcTaskName, clock());
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

void vTaskFunction1(void* pvParameters) {
    char* pcTaskName;
    pcTaskName = (char*)pvParameters;
    for (;; ) {
      for(unsigned int i = 0; i < 1000; i++) {
        printf("%s: %d\n", pcTaskName, i);
      }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

static const char* pcTextForPreemptiveTask = "Preemptive";
static const char* pcTextForTask = "Task is running";

void app_main(void) {
    printf("TIMME SLICE: 1\n");
    printf("PREEMPTION: 0\n");
    xTaskCreatePinnedToCore(vTaskFunction, "Task 1", 4096, (void*)pcTextForPreemptiveTask, 3, NULL, app_cpu);
    xTaskCreatePinnedToCore(vTaskFunction1, "Task 2", 4096, (void*)pcTextForTask, 2, NULL, app_cpu);
 }
