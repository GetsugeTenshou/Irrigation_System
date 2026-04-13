#include "Light_sensor.h"
#include "BH1750.h"
#include "Watering.h"
#include <string.h>

#define LIGHT_PRIORITY 4
#define LIGHT_STACK_SIZE 1024
#define QUEUE_LENGTH 1
#define ITEM_SIZE sizeof(uint16_t)
#define ONE_MINUTE 1
#define MAX_MINUTES 59
#define MAX_HOURS 23
#define ONE_HOUR 1
#define MEASUREMENT_INTERVAL_MS 120

extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;

typedef struct {

  RTC_AlarmTypeDef TimeToLight;
  RTC_TimeTypeDef Time;
  uint16_t data;

} rtc_ctx_t;

typedef struct {

  TaskHandle_t xHundle_Light;
  StaticTask_t XTask_Light_Buffer;
  StackType_t Light_Stack[LIGHT_STACK_SIZE];

  QueueHandle_t xQueue_Light;
  StaticQueue_t xQueue_Light_Buffer;
  uint8_t ucQueueStorageArea[QUEUE_LENGTH * ITEM_SIZE];

} light_ctx_t;

light_ctx_t light_ctx;
rtc_ctx_t rtc_ctx;

void light_loop(void *pvParameters);

static HAL_StatusTypeDef Rtc_Init(void) {

  if (HAL_RTC_GetTime(&hrtc, &rtc_ctx.Time, RTC_FORMAT_BIN) != HAL_OK) {
    return HAL_ERROR;
  }
/**Immediately add a specific time for the alarm clock */
  rtc_ctx.TimeToLight.AlarmTime.Hours = rtc_ctx.Time.Hours;
  rtc_ctx.TimeToLight.AlarmTime.Minutes = rtc_ctx.Time.Minutes + ONE_MINUTE;
  rtc_ctx.TimeToLight.AlarmTime.Seconds = rtc_ctx.Time.Seconds;
  rtc_ctx.TimeToLight.Alarm = RTC_ALARM_A;

  HAL_RTC_SetAlarm_IT(&hrtc, &rtc_ctx.TimeToLight, RTC_FORMAT_BIN);

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
    return LIGHT_SENSOR_DRIVER_INIT_FAIL;
  }

  if (Rtc_Init() != HAL_OK) {
    return LIGHT_SENSOR_RTC_INIT_FAIL;
  }

  if (Light_Sensor_ON() != SENSOR_SEND_COMMAND_OK) {
    return LIGHT_SENSOR_NOT_FOUND;
  }

  return LIGHT_SENSOR_INIT_OK;
}

void light_loop(void *pvParameters) {

  for (;;) {

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

#ifdef MEASUREMENT_MODE_CONTIN_120MS
    if (Light_Sensor_Contin_H_Measurement() == SENSOR_SEND_COMMAND_OK) {
      vTaskDelay(MEASUREMENT_INTERVAL_MS);
      if (HAL_I2C_Master_Receive(&hi2c1, SENSOR_ADDR, (uint8_t *)&rtc_ctx.data,
                                 sizeof(rtc_ctx.data), 1) != HAL_OK) {
      }
    } else {
      Light_Sensor_OFF();
      vTaskDelay(5);
      Light_Sensor_ON();
      Light_Sensor_Contin_H_Measurement();
    }
#endif

#ifdef MEASUREMENT_MODE_ONE_120MS
    if (Light_Sensor_One_H_Measurement() == SENSOR_SEND_COMMAND_OK) {
      vTaskDelay(MEASUREMENT_INTERVAL_MS);
      if (HAL_I2C_Master_Receive(&hi2c1, SENSOR_ADDR, (uint8_t *)&rtc_ctx.data,
                                 sizeof(rtc_ctx.data), 1) != HAL_OK) {
      }
    } else {
      Light_Sensor_OFF();
      vTaskDelay(5);
      Light_Sensor_ON();
      if (Light_Sensor_One_L_Measurement() == SENSOR_SEND_COMMAND_OK) {
        vTaskDelay(MEASUREMENT_INTERVAL_MS);
        if (HAL_I2C_Master_Receive(&hi2c1, SENSOR_ADDR,
                                   (uint8_t *)&rtc_ctx.data,
                                   sizeof(rtc_ctx.data), 1) != HAL_OK) {
        }
      }
    }
#endif

    HAL_RTC_GetTime(&hrtc, &rtc_ctx.Time, RTC_FORMAT_BIN);
    rtc_ctx.TimeToLight.AlarmTime.Minutes = rtc_ctx.Time.Minutes + ONE_MINUTE;
    HAL_RTC_SetAlarm_IT(&hrtc, &rtc_ctx.TimeToLight, RTC_FORMAT_BIN);
    /**Here if we cross the 60 minute mark then we reset the minutes. Crossing
     * the hour mark, namely 24 hours then we also reset the hours */
    if (rtc_ctx.Time.Minutes > MAX_MINUTES) {
      rtc_ctx.TimeToLight.AlarmTime.Minutes = 0;
      rtc_ctx.TimeToLight.AlarmTime.Hours = rtc_ctx.Time.Hours + ONE_HOUR;
      if (rtc_ctx.TimeToLight.AlarmTime.Hours > MAX_HOURS) {
        rtc_ctx.TimeToLight.AlarmTime.Hours = 0;
      }
    }
  }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR(light_ctx.xHundle_Light, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
