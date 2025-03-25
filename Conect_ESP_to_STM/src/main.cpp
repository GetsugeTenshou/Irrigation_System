#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

#define TX_PIN (GPIO_NUM_17)//Этот вывод будет использоваться для передачи данных
#define RX_PIN (GPIO_NUM_16)//Этот вывод будет использоваться для приема данных 



// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
const uart_port_t uart_num =UART_NUM_2;
uart_config_t uart_config={// Создает структурудля хранения настроек UART.
  .baud_rate=115200,
  .data_bits=UART_DATA_8_BITS,
  .parity=UART_PARITY_DISABLE,
  .stop_bits=UART_STOP_BITS_1,
  .flow_ctrl=UART_HW_FLOWCTRL_CTS_RTS,
  .rx_flow_ctrl_thresh=122,
};
 
ESP_ERROR_CHECK(uart_param_config(uart_num,&uart_config));
ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2,TX_PIN,RX_PIN,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));
const int uart_bufer_size= (1024*2);//Создает очередь для обработки событий UART
QueueHandle_t uart_queue;//Создает очередь для обработки событий UART
ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2,uart_bufer_size,uart_bufer_size,10,&uart_queue,0));//Устанавливает драйвер UART2 с указанным размером буфера и очередью событий.
}

void loop() {
  // put your main code here, to run repeatedly:
const uart_port_t uart_num= UART_NUM_2;
char data [128];
int length = 0;
ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num,(size_t*)&length));
length = uart_read_bytes(uart_num, (uint8_t*)data, length, 100); // Читаем байты в char массив
if(length>0){
  
  data[length] = '\0'; // Добавляем завершающий нулевой символ для строки
  Serial.println(data);
}

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}