#include "Light_sensor.h"
#include "BH1750.h"
#include "Watering.h"
#include <string.h>

#define LIGHT_PRIORITY 4
#define LIGHT_STACK_SIZE 1024
#define QUEUE_LENGTH 1
#define ITEM_SIZE sizeof(uint16_t)
#define CLOCK_TIME 1

extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
RTC_AlarmTypeDef TimeToLight;
RTC_TimeTypeDef Time;

typedef struct {

  TaskHandle_t xHundle_Light;
  StaticTask_t XTask_Light_Buffer;
  StackType_t Light_Stack[LIGHT_STACK_SIZE];

  QueueHandle_t xQueue_Light;
  StaticQueue_t xQueue_Light_Buffer;
  uint8_t ucQueueStorageArea[QUEUE_LENGTH * ITEM_SIZE];

} light_ctx_t;

light_ctx_t light_ctx;
static uint16_t data;

void light_loop(void *pvParameters);

static HAL_StatusTypeDef Rtc_Init(void) {

  if (HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN) != HAL_OK) {
    return HAL_ERROR;
  }

  TimeToLight.AlarmTime.Hours = Time.Hours;
  TimeToLight.AlarmTime.Minutes = Time.Minutes + CLOCK_TIME;
  TimeToLight.AlarmTime.Seconds = Time.Seconds;
  TimeToLight.Alarm = RTC_ALARM_A;

  HAL_RTC_SetAlarm_IT(&hrtc, &TimeToLight, RTC_FORMAT_BIN);

  return HAL_OK;
}

Light_Sensor_Status light_init(void) {

  light_ctx.xHundle_Light = xTaskCreateStatic(
      light_loop, "LIGHT_SENSOR", LIGHT_STACK_SIZE, NULL, LIGHT_PRIORITY,
      light_ctx.Light_Stack, &light_ctx.XTask_Light_Buffer);

  light_ctx.xQueue_Light =
      xQueueCreateStatic(QUEUE_LENGTH, ITEM_SIZE, light_ctx.ucQueueStorageArea,
                         &light_ctx.xQueue_Light_Buffer);

  if (BH1750_Init(&hi2c1) != SENSOR_INIT_OK) {
    return LIGHT_SENSOR_INIT_FAIL;
  }

  if (Rtc_Init() != HAL_OK) {
    return LIGHT_SENSOR_INIT_FAIL;
  }

  if (Light_Sensor_ON() != SENSOR_SEND_COMMAND_OK) {
    return LIGHT_SENSOR_INIT_FAIL;
  }

  return LIGHT_SENSOR_INIT_OK;
}

void light_loop(void *pvParameters) {

  for (;;) {

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

#ifdef MEASUREMENT_MODE_ONE_120MS
    if (Light_Sensor_One_H_Measurement() == SENSOR_SEND_COMMAND_OK) {
      vTaskDelay(120);
      if (HAL_I2C_Master_Receive_DMA(&hi2c1, SENSOR_ADDR, data, sizeof(data)) !=
          HAL_OK) {
      }
    } else {
      Light_Sensor_OFF();
      vTaskDelay(5);
      Light_Sensor_ON();
      Light_Sensor_One_H_Measurement();
    }
#endif

    if (Light_Sensor_One_L_Measurement() == SENSOR_SEND_COMMAND_OK) {
      vTaskDelay(16);
      if (HAL_I2C_Master_Receive(&hi2c1, SENSOR_ADDR, &data, sizeof(data),
                                 16) != HAL_OK) {
      }

    } else {
      Light_Sensor_OFF();
      vTaskDelay(5);
      Light_Sensor_ON();
      if (Light_Sensor_One_L_Measurement() == SENSOR_SEND_COMMAND_OK) {
        vTaskDelay(16);
        if (HAL_I2C_Master_Receive(&hi2c1, SENSOR_ADDR, &data, sizeof(data),
                                   1) != HAL_OK) {
        }
      }
    }

    // #ifdef MEASUREMENT_MODE_ACCURATE_CONTINUOUS_120MS
    //     if (Light_Sensor_Contin_H_Measurement() == SENSOR_SEND_COMMAND_OK) {
    //       vTaskDelay(120);
    //       if (HAL_I2C_Master_Receive_DMA(&hi2c1, SENSOR_ADDR, data,
    //       sizeof(data)) !=
    //           HAL_OK) {
    //       }

    //     } else {
    //       Light_Sensor_OFF();
    //       vTaskDelay(5);
    //       Light_Sensor_ON();
    //       Light_Sensor_Contin_H_Measurement()
    //     }
    // #endif

    // #ifdef MEASUREMENT_MODE_LESS_ACCURATE_CONTINUOUS_16MS
    //     if (Light_Sensor_Contin_L_Measurement() == SENSOR_SEND_COMMAND_OK) {
    //       vTaskDelay(16);
    //       if (HAL_I2C_Master_Receive_DMA(&hi2c1, SENSOR_ADDR, data,
    //       sizeof(data)) !=
    //           HAL_OK) {
    //       }

    //     } else {
    //       Light_Sensor_OFF();
    //       vTaskDelay(5);
    //       Light_Sensor_ON();
    //       Light_Sensor_Contin_L_Measurement()
    //     }
    // #endif

    HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
    TimeToLight.AlarmTime.Minutes = Time.Minutes + CLOCK_TIME;
    HAL_RTC_SetAlarm_IT(&hrtc, &TimeToLight, RTC_FORMAT_BIN);
  }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(light_ctx.xHundle_Light, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
