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
}Light_Sensor_Status;

/**
 * @brief 
 * 
 * @return Light_Sensor_Status 
 */
Light_Sensor_Status light_init(void);

#endif /**LIGHT_SENSOR_H_ */
