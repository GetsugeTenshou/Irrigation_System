#include "BH1750.h"
#include "stm32f1xx_hal.h"
#include <string.h>

#define I2C_TIMEOUT 5
#define I2C_TRIES 2

extern I2C_HandleTypeDef hi2c1;

static const uint8_t RETRY_TIMEOUT_MS = 50;
static const uint8_t RETRY_INTERVAL_MS = 5;

static HAL_StatusTypeDef I2C_Send_CMD(Sensor_CMD_t *cmd) {

  HAL_StatusTypeDef status =
      HAL_I2C_Master_Transmit(&hi2c1, SENSOR_ADDR, &cmd, sizeof(uint8_t), 1);
  return status;
}

static Sensor_cmd_status Light_Sensor_Send(Sensor_CMD_t cmd) {

  if (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
    return SENSOR_BUS_BUSY;
  }

  uint32_t timer = HAL_GetTick();
  uint32_t last_attempt = 0;

  while ((HAL_GetTick() - timer) < RETRY_TIMEOUT_MS) {
    if ((HAL_GetTick() - last_attempt) >= RETRY_INTERVAL_MS) {
      last_attempt = HAL_GetTick();

      if (HAL_I2C_IsDeviceReady(&hi2c1, SENSOR_ADDR, I2C_TRIES, I2C_TIMEOUT) ==
          HAL_OK) {

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

  if (HAL_I2C_IsDeviceReady(&hi2c1, SENSOR_ADDR, I2C_TRIES, I2C_TIMEOUT)) {
    return SENSOR_NOT_FOUND;
  }

  if (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
    return SENSOR_BUS_BUSY;
  }

  if (I2C_Send_CMD(POWER_ON_CMD) != HAL_OK) {
    return SENSOR_SEND_COMMAND_FAIL;
  }

  return SENSOR_SEND_COMMAND_OK;
}

Sensor_cmd_status Light_Sensor_OFF(void) { 
    if (HAL_I2C_IsDeviceReady(&hi2c1, SENSOR_ADDR, I2C_TRIES, I2C_TIMEOUT)) {
    return SENSOR_NOT_FOUND;
  }

  if (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {
    return SENSOR_BUS_BUSY;
  }

  if (I2C_Send_CMD(POWER_DOWN_CMD) != HAL_OK) {
    return SENSOR_SEND_COMMAND_FAIL;
  }

  return SENSOR_SEND_COMMAND_OK;
}

Sensor_cmd_status Light_Sensor_One_L_Measurement(void) {
  Light_Sensor_Send(ONE_TIME_L_RES_MODE_CMD);
}

Sensor_cmd_status Light_Sensor_One_H_Measurement(void) {
  Light_Sensor_Send(ONE_TIME_H_RES_MODE_CMD);
}

Sensor_cmd_status Light_Sensor_Contin_H2_Measurement(void) {
  Light_Sensor_Send(ONE_TIME_H_RES_MODE2_CMD);
}

Sensor_cmd_status Light_Sensor_Contin_L_Measurement(void) {
  Light_Sensor_Send(CONTINUOUSLY_L_RES_MODE_CMD);
}

Sensor_cmd_status Light_Sensor_Contin_H_Measurement(void) {
  Light_Sensor_Send(CONTINUOUSLY_H_RES_MODE_CMD);
}

Sensor_cmd_status Light_Sensor_Contin_H2_Measurement(void) {
  Light_Sensor_Send(CONTINUOUSLY_H_RES_MODE2_CMD);
}

Sensor_cmd_status Light_Sensor_Reset_Reg(void){
    Light_Sensor_Send(RESET_CMD);

}