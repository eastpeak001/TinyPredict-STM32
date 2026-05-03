#include "alarm_task.h"
#include "vibration_algo.h"
#include "main.h"

#define ALARM_RMS_THRESHOLD  0.15f

#define LED_ON_STATE         GPIO_PIN_RESET
#define LED_OFF_STATE        GPIO_PIN_SET

void AlarmTask_Init(void)
{
  HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, LED_OFF_STATE);
}

void AlarmTask_Update(float rms_value)
{
  if (VibrationAlgo_GetState() == VIBRATION_CALIBRATING)
  {
    HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, LED_OFF_STATE);
    return;
  }

  if (rms_value >= ALARM_RMS_THRESHOLD)
  {
    HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, LED_ON_STATE);
  }
  else
  {
    HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, LED_OFF_STATE);
  }
}

