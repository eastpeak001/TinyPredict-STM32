#ifndef SYSTEM_STATUS_H
#define SYSTEM_STATUS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  SYSTEM_STATUS_NORMAL = 0,
  SYSTEM_STATUS_WARNING,
  SYSTEM_STATUS_ALARM
} SystemStatus_t;

void SystemStatus_Init(void);
SystemStatus_t SystemStatus_UpdateFromRms(float rms);
SystemStatus_t SystemStatus_GetCurrent(void);
const char *SystemStatus_ToString(SystemStatus_t status);
const char *SystemStatus_GetCurrentString(void);
uint8_t SystemStatus_IsAlarm(SystemStatus_t status);
uint8_t SystemStatus_GetEnterCount(void);
uint8_t SystemStatus_GetExitCount(void);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_STATUS_H */
