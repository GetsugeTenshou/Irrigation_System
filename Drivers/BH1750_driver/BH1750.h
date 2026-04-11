#ifndef BH1750_H_
#define BH1750_H_

#include "FreeRTOS.h"
#include "main.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>

#define SENSOR_ADDR 0x46

typedef struct {
  I2C_HandleTypeDef *hi2c;
} BH1750_t;

typedef enum {
  CONTINUOUSLY_H_RES_MODE2_CMD = 0x11,
  CONTINUOUSLY_H_RES_MODE_CMD = 0x10,
  CONTINUOUSLY_L_RES_MODE_CMD = 0x13,
  ONE_TIME_H_RES_MODE2_CMD = 0x21,
  ONE_TIME_H_RES_MODE_CMD = 0x20,
  ONE_TIME_L_RES_MODE_CMD = 0x23,
  POWER_DOWN_CMD = 0x00,
  POWER_ON_CMD = 0x01,
  RESET_CMD = 0x07,
} Sensor_CMD_t;

typedef enum {
  SENSOR_SEND_COMMAND_OK,
  SENSOR_SEND_COMMAND_FAIL,
  SENSOR_NOT_FOUND,
  SENSOR_BUS_BUSY,
  SENSOR_ERROR_TIMEOUT,
  SENSOR_ERROR_ADDR_NACK,
  SENSOR_INIT_ERROR,
  SENSOR_INIT_OK
} Sensor_cmd_status;

/**
 * @brief Initializes the BH1750 driver structure.
 * @param hi2c Pointer to the I2C hardware handle.
 * @return Sensor_cmd_status Returns OK if initialization succeeds.
 */
Sensor_cmd_status BH1750_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief Function for ON sensor
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Light_Sensor_ON(void);

/**
 * @brief Function for OFF sensor
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Light_Sensor_OFF(void);

/**
 * @brief Function that reset data register value. Reset command is not
 * acceptable in Power Down mode
 *
 * Resets the sensor's internal data register (Synchronous).
 * 1. Sends the RESET_CMD to the driver task via task notification.
 * 2. Blocks the calling task until the driver confirms execution via semaphore.
 * 3. Note: Reset command is only valid when the sensor is in Power ON state.
 */
Sensor_cmd_status Light_Sensor_Reset_Reg(void);

/**
 * @brief Function for make measurement at 1lx resolution.
 * Measurement Time is typically 120ms.
 * It is automatically set to Power Down mode after measurement
 *
 * Triggers One-Time High Resolution measurement (Synchronous).
 * 1. Sends H-Resolution command to the driver task.
 * 2. Blocks the caller until the driver signals completion.
 * 3. Returns the actual hardware status or a timeout error.
 */
Sensor_cmd_status Light_Sensor_One_H_Measurement(void);

/**
 * @brief Function for make measurement at 0.5lx resolution.
 * Measurement Time is typically 120ms.
 * It is automatically set to Power Down mode after measurement
 *
 * Triggers Continuous High Resolution Mode 2 measurement (Synchronous).
 * 1. Sends continuous H-Res Mode 2 command (0.5 lux precision).
 * 2. Blocks calling task until the driver signals completion.
 * 3. Returns the resulting status or a timeout error.
 */
Sensor_cmd_status Light_Sensor_One_H2_Measurement(void);

/**
 * @brief Function for make measurement at 4lx resolution.
 * Measurement Time is typically 16ms.
 * It is automatically set to Power Down mode after measurement
 *
 * Triggers One-Time Low Resolution measurement .
 * 1. Notifies driver task with the specific command.
 * 2. Blocks calling task until the driver signals completion via semaphore.
 * 3. Returns driver execution status or timeout error.
 */
Sensor_cmd_status Light_Sensor_One_L_Measurement(void);

/**
 * @brief Function for make measurement at 1lx resolution.
 * Measurement Time is typically 120ms
 *
 * Triggers Continuous High Resolution measurement (Synchronous).
 * 1. Sends continuous H-Res mode command to the driver task.
 * 2. Blocks calling task until the driver signals completion.
 * 3. Returns the resulting status or a timeout error.
 */

Sensor_cmd_status Light_Sensor_Contin_H_Measurement(void);

/**
 * @brief Function for make measurement at 0.5lx resolution.
 * Measurement Time is typically 120ms.
 *
 * Triggers One-Time High Resolution Mode 2 measurement (Synchronous).
 * 1. Notifies driver task to start H-Res Mode 2 (0.5 lux precision).
 * 2. Blocks the caller until the driver signals completion via semaphore.
 * 3. Returns the resulting status from the driver or a timeout error.
 */
Sensor_cmd_status Light_Sensor_Contin_H2_Measurement(void);

/**
 * @brief Function for make measurement at 4lx resolution.
 * Measurement Time is typically 16ms.
 *
 * Triggers Continuous Low Resolution measurement (Synchronous).
 * 1. Sends continuous L-Res mode command to the driver task.
 * 2. Blocks calling task until the driver signals completion.
 * 3. Returns the resulting status or a timeout error.
 */
Sensor_cmd_status Light_Sensor_Contin_L_Measurement(void);

/**
 * @brief Function for change  measurement time.
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Change_Measurement_Time(void);

#endif /**BH1750_H_*/