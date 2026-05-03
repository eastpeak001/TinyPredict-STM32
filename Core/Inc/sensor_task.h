#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void SensorTask_Init(void);
void SensorTask_Update(void);
uint8_t SensorTask_IsReady(void);
float SensorTask_GetAccelX(void);
float SensorTask_GetAccelY(void);
float SensorTask_GetAccelZ(void);
float SensorTask_GetLatestSample(void);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_TASK_H */
