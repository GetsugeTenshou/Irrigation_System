#ifndef HUMIDITY_SENSOR_H_
#define HUMIDITY_SENSOR_H_

#include "FreeRTOS.h"
#include "main.h"
#include "queue.h"
#include "task.h"
#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE 10
#define MAX_SENSORS 4

typedef struct {
  uint32_t buffer[10];
  uint8_t index;
  uint32_t sum;
  uint16_t average;
} SMA_filter;

typedef struct {
  uint8_t ID;
  uint8_t sum;
} circular_buf_t;

typedef struct {
  uint32_t buffer[BUFFER_SIZE];
  uint8_t indx;
  SMA_filter filter;
} sensor_t;
 
typedef struct {
  sensor_t sensors[MAX_SENSORS];
  uint8_t count;
}sensor_cxt_t;

/**
 * @brief Initializes the humidity sensing module.
 * Calibrates the ADC, starts DMA transfer for continuous ADC data
 * acquisition into the ADC buffer, creates the humidity processing
 * FreeRTOS task and initializes the humidity data queue.
 *
 */
void Humidity_sensors_init(void);

#endif /* HUMIDITY_SENSOR_H_ */