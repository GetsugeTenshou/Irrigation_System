#ifndef BH1750_H_
#define BH1750_H_

#include "FreeRTOS.h"
#include "main.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>

#define SENSOR_ADDR 0x23

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
  SENSOR_ERROR_BUSY,
  SENSOR_ERROR_TIMEOUT,
  SENSOR_ERROR_ADDR_NACK
} Sensor_cmd_status;

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
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Light_Sensor_Reset_Reg(void);

/**
 * @brief Function for make measurement at 1lx resolution.
 * Measurement Time is typically 120ms.
 * It is automatically set to Power Down mode after measurement
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Light_Sensor_One_H_Measurement(void);

/**
 * @brief Function for make measurement at 0.5lx resolution.
 * Measurement Time is typically 120ms.
 * It is automatically set to Power Down mode after measurement
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Light_Sensor_One_H2_Measurement(void);

/**
 * @brief Function for make measurement at 4lx resolution.
 * Measurement Time is typically 16ms. 
 * It is automatically set to Power Down mode after measurement
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Light_Sensor_One_L_Measurement(void);

/**
 * @brief Function for make measurement at 1lx resolution.
 * Measurement Time is typically 120ms
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Light_Sensor_Contin_H_Measurement(void);

/**
 * @brief Function for make measurement at 0.5lx resolution.
 * Measurement Time is typically 120ms. 
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Light_Sensor_Contin_H2_Measurement(void);

/**
 * @brief Function for make measurement at 4lx resolution.
 * Measurement Time is typically 16ms. 
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Light_Sensor_Contin_L_Measurement(void);

/**
 * @brief Function for change  measurement time. 
 *
 * @return Sensor_cmd_status
 */
Sensor_cmd_status Change_Measurement_Time(void);

#endif /**BH1750_H_*/