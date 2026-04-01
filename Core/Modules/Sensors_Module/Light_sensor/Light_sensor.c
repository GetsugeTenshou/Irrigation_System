#include "Light_sensor.h"
#include "BH1750.h"
#include "Watering.h"
#include <string.h>

#define LIGHT_PRIORITY 4
#define LIGHT_STACK_SIZE 1024
#define QUEUE_LENGTH 1
#define ITEM_SIZE sizeof(uint16_t)

extern I2C_HandleTypeDef hi2c1;

typedef struct {

  TaskHandle_t xHundle_Light;
  StaticTask_t XTask_Light_Buffer;
  StackType_t Light_Stack[LIGHT_STACK_SIZE];

  QueueHandle_t xQueue_Light;
  StaticQueue_t xQueue_Light_Buffer;
  uint8_t ucQueueStorageArea[QUEUE_LENGTH * ITEM_SIZE];

} light_ctx_t;

light_ctx_t light_ctx;
static uint8_t data;

void light_loop(void *pvParameters);

Light_Sensor_Status light_init(void) {
  light_ctx.xHundle_Light = xTaskCreateStatic(
      light_loop, "LIGHT_SENSOR", LIGHT_STACK_SIZE, NULL, LIGHT_PRIORITY,
      light_ctx.Light_Stack, &light_ctx.XTask_Light_Buffer);

  light_ctx.xQueue_Light =
      xQueueCreateStatic(QUEUE_LENGTH, ITEM_SIZE, light_ctx.ucQueueStorageArea,
                         &light_ctx.xQueue_Light_Buffer);
  BH1750_Init(&hi2c1);

  HAL_I2C_Master_Receive_DMA(&hi2c1, SENSOR_ADDR, data, sizeof(data));
}

void light_loop(void *pvParameters) {

  for(;;){
     ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

     


  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(light_ctx.xHundle_Light, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
