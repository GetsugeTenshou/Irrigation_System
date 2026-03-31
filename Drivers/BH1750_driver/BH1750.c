#include "BH1750.h"
#include "Light_sensor.h"
#include "semphr.h"
#include "stm32f1xx_hal.h"
#include <string.h>

#define I2C_TIMEOUT 5
#define I2C_TRIES 2

#define BH1750_PRIORITY 4
#define BH1750_STACK_SIZE 1024
#define QUEUE_LENGTH 1
#define ITEM_SIZE sizeof(uint16_t)

#define MAX_I2C_SEMAPHORE_TIMEOUT 120

static BH1750_t bh_init;

static const uint32_t RETRY_TIMEOUT_MS = 50;
static const uint32_t RETRY_INTERVAL_MS = 5;

Sensor_cmd_status status_cmd;

typedef struct {

  TaskHandle_t xHundle_BH1750;
  SemaphoreHandle_t xSem_I2C_Done;
  StaticTask_t XTask_BH1750_Buffer;
  StackType_t BH1750_Stack[BH1750_STACK_SIZE];

  QueueHandle_t xQueue_BH1750;
  StaticQueue_t xQueue_BH1750_Buffer;
  uint8_t ucQueueStorageArea[QUEUE_LENGTH * ITEM_SIZE];

} light_ctx_t;

light_ctx_t BH1750_ctx;

void BH1750_loop(void *pvParameters);

Sensor_cmd_status BH1750_Init(I2C_HandleTypeDef *hi2c) {
  if (hi2c != NULL) {
    bh_init.hi2c = hi2c;
    return SENSOR_INIT_OK;
  } else {
    return SENSOR_INIT_ERROR;
  }
}

static HAL_StatusTypeDef I2C_Send_CMD(Sensor_CMD_t cmd) {

  HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&bh_init.hi2c, SENSOR_ADDR,
                                                     &cmd, sizeof(uint8_t), 1);
  return status;
}

/**
 * @brief Sends command with retry logic.
 * 1. Checks I2C peripheral availability.
 * 2. Polls device presence within RETRY_TIMEOUT_MS.
 * 3. Transmits command byte upon device readiness.
 */
static Sensor_cmd_status Light_Sensor_Send(Sensor_CMD_t cmd) {

  if (HAL_I2C_GetState(bh_init.hi2c) != HAL_I2C_STATE_READY) {
    return SENSOR_BUS_BUSY;
  }

  uint32_t timer = HAL_GetTick();
  uint32_t last_attempt = 0;

  while ((HAL_GetTick() - timer) < RETRY_TIMEOUT_MS) {
    if ((HAL_GetTick() - last_attempt) >= RETRY_INTERVAL_MS) {
      last_attempt = HAL_GetTick();

      if (HAL_I2C_IsDeviceReady(bh_init.hi2c, SENSOR_ADDR, I2C_TRIES,
                                I2C_TIMEOUT) == HAL_OK) {

        if (I2C_Send_CMD(&cmd) != HAL_OK) {
          return SENSOR_SEND_COMMAND_FAIL;
        }

        return SENSOR_SEND_COMMAND_OK;
      }
    }
  }
  return SENSOR_NOT_FOUND;
}

Sensor_cmd_status Light_Sensor_ON(void) {

  if (HAL_I2C_IsDeviceReady(bh_init.hi2c, SENSOR_ADDR, I2C_TRIES,
                            I2C_TIMEOUT) != HAL_OK) {
    return SENSOR_NOT_FOUND;
  }

  if (HAL_I2C_GetState(bh_init.hi2c) != HAL_I2C_STATE_READY) {
    return SENSOR_BUS_BUSY;
  }

  if (I2C_Send_CMD(POWER_ON_CMD) != HAL_OK) {
    return SENSOR_SEND_COMMAND_FAIL;
  }

  return SENSOR_SEND_COMMAND_OK;
}

Sensor_cmd_status Light_Sensor_OFF(void) {
  if (HAL_I2C_IsDeviceReady(bh_init.hi2c, SENSOR_ADDR, I2C_TRIES,
                            I2C_TIMEOUT)) {
    return SENSOR_NOT_FOUND;
  }

  if (HAL_I2C_GetState(bh_init.hi2c) != HAL_I2C_STATE_READY) {
    return SENSOR_BUS_BUSY;
  }

  if (I2C_Send_CMD(POWER_DOWN_CMD) != HAL_OK) {
    return SENSOR_SEND_COMMAND_FAIL;
  }

  return SENSOR_SEND_COMMAND_OK;
}

Sensor_cmd_status Light_Sensor_One_L_Measurement(void) {

  if (xTaskNotify(BH1750_ctx.xHundle_BH1750, ONE_TIME_L_RES_MODE_CMD,
                  eSetValueWithOverwrite) != pdPASS) {
    return SENSOR_SEND_COMMAND_FAIL;
  }
  if (xQueueSemaphoreTake(BH1750_ctx.xSem_I2C_Done,
                          pdMS_TO_TICKS(MAX_I2C_SEMAPHORE_TIMEOUT)) == pdPASS) {
    return status_cmd;
  } else {

    return SENSOR_ERROR_TIMEOUT;
  }
}

Sensor_cmd_status Light_Sensor_One_H_Measurement(void) {
  if (xTaskNotify(BH1750_ctx.xHundle_BH1750, ONE_TIME_H_RES_MODE_CMD,
                  eSetValueWithOverwrite) != pdPASS) {
    return SENSOR_SEND_COMMAND_FAIL;
  }
  if (xQueueSemaphoreTake(BH1750_ctx.xSem_I2C_Done,
                          pdMS_TO_TICKS(MAX_I2C_SEMAPHORE_TIMEOUT)) == pdPASS) {
    return status_cmd;
  } else {

    return SENSOR_ERROR_TIMEOUT;
  }
}

Sensor_cmd_status Light_Sensor_Contin_H2_Measurement(void) {
  if (xTaskNotify(BH1750_ctx.xHundle_BH1750, ONE_TIME_H_RES_MODE2_CMD,
                  eSetValueWithOverwrite) != pdPASS) {
    return SENSOR_SEND_COMMAND_FAIL;
  }
  if (xQueueSemaphoreTake(BH1750_ctx.xSem_I2C_Done,
                          pdMS_TO_TICKS(MAX_I2C_SEMAPHORE_TIMEOUT)) == pdPASS) {
    return status_cmd;
  } else {

    return SENSOR_ERROR_TIMEOUT;
  }
}

Sensor_cmd_status Light_Sensor_Contin_L_Measurement(void) {
  if (xTaskNotify(BH1750_ctx.xHundle_BH1750, CONTINUOUSLY_L_RES_MODE_CMD,
                  eSetValueWithOverwrite) != pdPASS) {
    return SENSOR_SEND_COMMAND_FAIL;
  }
  if (xQueueSemaphoreTake(BH1750_ctx.xSem_I2C_Done,
                          pdMS_TO_TICKS(MAX_I2C_SEMAPHORE_TIMEOUT)) == pdPASS) {
    return status_cmd;
  } else {

    return SENSOR_ERROR_TIMEOUT;
  }
}

Sensor_cmd_status Light_Sensor_Contin_H_Measurement(void) {
  if (xTaskNotify(BH1750_ctx.xHundle_BH1750, CONTINUOUSLY_H_RES_MODE_CMD,
                  eSetValueWithOverwrite) != pdPASS) {
    return SENSOR_SEND_COMMAND_FAIL;
  }
  if (xQueueSemaphoreTake(BH1750_ctx.xSem_I2C_Done,
                          pdMS_TO_TICKS(MAX_I2C_SEMAPHORE_TIMEOUT)) == pdPASS) {
    return status_cmd;
  } else {

    return SENSOR_ERROR_TIMEOUT;
  }
}

Sensor_cmd_status Light_Sensor_Contin_H2_Measurement(void) {
  if (xTaskNotify(BH1750_ctx.xHundle_BH1750, CONTINUOUSLY_H_RES_MODE2_CMD,
                  eSetValueWithOverwrite) != pdPASS) {
    return SENSOR_SEND_COMMAND_FAIL;
  }
  if (xQueueSemaphoreTake(BH1750_ctx.xSem_I2C_Done,
                          pdMS_TO_TICKS(MAX_I2C_SEMAPHORE_TIMEOUT)) == pdPASS) {
    return status_cmd;
  } else {

    return SENSOR_ERROR_TIMEOUT;
  }
}

Sensor_cmd_status Light_Sensor_Reset_Reg(void) {
  if (xTaskNotify(BH1750_ctx.xHundle_BH1750, RESET_CMD,
                  eSetValueWithOverwrite) != pdPASS) {
    return SENSOR_SEND_COMMAND_FAIL;
  }
  if (xQueueSemaphoreTake(BH1750_ctx.xSem_I2C_Done,
                          pdMS_TO_TICKS(MAX_I2C_SEMAPHORE_TIMEOUT)) == pdPASS) {
    return status_cmd;
  } else {

    return SENSOR_ERROR_TIMEOUT;
  }
}

void BH1750_loop(void *pvParameters) {
  static uint32_t cmd;
  for (;;) {

    xTaskNotifyWait(0x00, 0xFFFFFFFF, &cmd, portMAX_DELAY);

    switch (cmd) {
    case ONE_TIME_L_RES_MODE_CMD:
      status_cmd = Light_Sensor_Send(ONE_TIME_L_RES_MODE_CMD);
      break;

    case ONE_TIME_H_RES_MODE_CMD:
      status_cmd = Light_Sensor_Send(ONE_TIME_H_RES_MODE_CMD);
      break;

    case ONE_TIME_H_RES_MODE2_CMD:
      status_cmd = Light_Sensor_Send(ONE_TIME_H_RES_MODE2_CMD);
      break;

    case CONTINUOUSLY_L_RES_MODE_CMD:
      status_cmd = Light_Sensor_Send(CONTINUOUSLY_L_RES_MODE_CMD);
      break;

    case CONTINUOUSLY_H_RES_MODE_CMD:
      status_cmd = Light_Sensor_Send(CONTINUOUSLY_H_RES_MODE_CMD);
      break;

    case CONTINUOUSLY_H_RES_MODE2_CMD:
      status_cmd = Light_Sensor_Send(CONTINUOUSLY_H_RES_MODE2_CMD);
      break;

    case RESET_CMD:
      status_cmd = Light_Sensor_Send(RESET_CMD);
      break;

    default:
      break;
    }
    xSemaphoreGive(BH1750_ctx.xSem_I2C_Done);
  }
}
