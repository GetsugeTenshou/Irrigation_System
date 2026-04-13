#ifndef LIGHT_SENSOR_H_
#define LIGHT_SENSOR_H_

#include "FreeRTOS.h"
#include "main.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>

typedef enum {
  LIGHT_SENSOR_INIT_OK,
  LIGHT_SENSOR_INIT_FAIL,
  LIGHT_SENSOR_RTC_INIT_FAIL,
  LIGHT_SENSOR_DRIVER_INIT_FAIL,
  LIGHT_SENSOR_NOT_FOUND
} Light_Sensor_Status;

/**
 * @brief A function that initializes a timer with the current time and
 * immediately adds to it our specified time in the definition
 *
 * @return LIGHT_SENSOR_INIT_OK if everything is okay and initialization went
 * well.
 * LIGHT_SENSOR_DRIVER_INIT_FAIL if the driver was not
 * initialized.
 * LIGHT_SENSOR_RTC_INIT_FAIL if the module was not
 * initialized.
 * LIGHT_SENSOR_NOT_FOUND if the sensor was not found when sending
 * the power-on command
 */
Light_Sensor_Status light_init(void);

#endif /**LIGHT_SENSOR_H_ */
