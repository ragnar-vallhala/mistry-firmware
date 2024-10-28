#include "FreeRTOS.h"
#include "connection/TCPClient.h"
#include "connection/WiFiManager.h"
#include "display/UIHandler.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "projdefs.h"
#include "task.h"
#include <hardware/gpio.h>
#include <stdio.h>

#define LED_PIN 17
UIHandler *hndl;
WiFiManager *wifi;

void toggleLED(void *pvParameters) {
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  while (!wifi->establish())
    ;
 
  for (;;) {
    TCPClient client(TCP_SERVER_IP, TCP_PORT);
    printf("Hello from Core %d!, Status: %d\n", get_core_num(),
           wifi->establish());
    if (client.connect()) {
      printf("Connected to server.\n");
      const char *message = "Hello, Server!";
      client.send(message, strlen(message));

      char buffer[1024];
      size_t received_length;
      if (client.receive(buffer, sizeof(buffer), received_length)) {
        printf("Received from server: %.*s\n", (int)received_length, buffer);
      }
    } else {
      printf("Failed to connect to server.\n");
    }
    vTaskDelay(pdMS_TO_TICKS(3000));
    gpio_put(LED_PIN, !gpio_get(LED_PIN));
    client.disconnect();
  }
}

// Function to print a message
void printMessage(void *pvParameters) {
  for (;;) {
    printf("Hello from Core %d!\n", get_core_num());
    hndl->start();
    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000 ms
  }
}

int main() {
  stdio_init_all(); // Initialize stdio for console output
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  hndl = UIHandler::get_ui_handler();
  wifi = WiFiManager::get_wifi_manager();

  // Create FreeRTOS tasks
  TaskHandle_t handleToggleLED;
  TaskHandle_t handlePrintMessage;
  printf("Hello from Core %d!\n", get_core_num());
  // Create the LED toggle task pinned to Core 0
  xTaskCreate(toggleLED, "Toggle LED Task", 2048, NULL, 1, &handleToggleLED);

  // Create the print message task pinned to Core 1
  xTaskCreate(printMessage, "Print Message Task", 1024, NULL, 1,
              &handlePrintMessage);
  vTaskCoreAffinitySet(handleToggleLED, (1 << 0));
  vTaskCoreAffinitySet(handlePrintMessage, (1 << 1));
  // Start the FreeRTOS scheduler
  vTaskStartScheduler();

  /*     // This point should never be reached unless there is insufficient
   * memory
   */
  while (1) {
    // Empty loop
  }

  return 0; // This line will never be reached
}
