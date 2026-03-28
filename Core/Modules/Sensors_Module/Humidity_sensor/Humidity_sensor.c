#include "Humidity_sensor.h"
#include "Watering.h"
#include <string.h>


#define HUMIDITY_PRIORITY 3
#define HUMIDITY_STACK_SIZE 1024
#define QUEUE_LENGTH 10
#define ITEM_SIZE sizeof(uint16_t)

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

typedef struct {

  TaskHandle_t xHundle_Humidity;
  StaticTask_t xTask_Humidity_Buffer;
  StackType_t Humidity_Stack[HUMIDITY_STACK_SIZE];

  QueueHandle_t xQueue_Humidity;
  StaticQueue_t xQueue_Humidity_Buffer;
  uint8_t ucQueueStorageArea[QUEUE_LENGTH * ITEM_SIZE];

} humididty_ctx_t;

humididty_ctx_t humididty_ctx;
circular_buf_t buffer;
uint32_t adc_buffer[MAX_SENSORS];
sensor_cxt_t sensors_ctx = {.count = MAX_SENSORS};

void humidity_loop(void *pvParameters);
/**
 * @brief Function for calibrate ADC module
 *
 * @param hadc
 */
void ADC_Calibrate(ADC_HandleTypeDef *hadc) {
  HAL_ADC_Stop(hadc);
  HAL_ADCEx_Calibration_Start(hadc);
  // TODO Check what calibrate exactly do and check if that actual do
}

void Humidity_sensors_init(void) {

  humididty_ctx.xHundle_Humidity = xTaskCreateStatic(
      humidity_loop, "HUMIDITY", HUMIDITY_STACK_SIZE, NULL, HUMIDITY_PRIORITY,
      humididty_ctx.Humidity_Stack, &humididty_ctx.xTask_Humidity_Buffer);
  humididty_ctx.xQueue_Humidity =
      xQueueCreateStatic(QUEUE_LENGTH, ITEM_SIZE, humididty_ctx.ucQueueStorageArea,
                         &humididty_ctx.xQueue_Humidity_Buffer);
  ADC_Calibrate(&hadc1);

  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, MAX_SENSORS);
  
}

/**
 * @brief Function  that calculate moving average without cycle "for" that can
 * freeze FreeRTOS task
 *
 * @param filter struct that contain parameters about cycle buffer
 * @param value New value that we get from "HAL_ADC_ConvHalfCpltCallback"
 * @return uint16_t  calculated average
 */
uint16_t Moving_AVG(SMA_filter *filter, uint16_t value) {
  filter->sum -= filter->buffer[filter->index];
  filter->sum += value;
  filter->buffer[filter->index] = value;
  filter->index++;
  if (filter->index >= BUFFER_SIZE) {
    filter->index = 0;
  }
  return filter->average = filter->sum / BUFFER_SIZE;
}

void humidity_loop(void *pvParameters) {

  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /**If we use IDWG timer we mast set
                                           TickType_t xTicksToWait in 1  */

    for (uint8_t i = 0; i < sensors_ctx.count; i++) {
      sensor_t *s = &sensors_ctx.sensors[i];
      s->buffer[s->indx] = adc_buffer[i];
      s->indx = (s->indx + 1) % BUFFER_SIZE;
      Moving_AVG(&s->filter, adc_buffer[i]);
    }

    // if (filter.average == NULL || Watering_GetQueue() == NULL) {
    //   if (xQueueSend(Watering_GetQueue(), &filter.average, 0) != pdTRUE) {
    //     /**TODO write that we do if queue is NULL and avrg is NULL */
    //   }
    //   /**TODO: Write processing that we get pdFALSE */
    // }
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc1) {
  if (humididty_ctx.xHundle_Humidity == NULL) {
    return;
  }
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(humididty_ctx.xHundle_Humidity, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc1) {
  HAL_ADC_Stop_DMA(hadc1);
  HAL_ADC_Start_DMA(hadc1, (uint32_t *)adc_buffer, MAX_SENSORS);
}