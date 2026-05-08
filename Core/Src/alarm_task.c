#include "alarm_task.h"
#include "vibration_algo.h"
#include "system_status.h"
#include "main.h"

#define LED_ON_STATE              GPIO_PIN_RESET
#define LED_OFF_STATE             GPIO_PIN_SET

#define BUZZER_GPIO_Port          GPIOB
#define BUZZER_Pin                GPIO_PIN_12
#define BUZZER_ON_STATE           GPIO_PIN_RESET
#define BUZZER_OFF_STATE          GPIO_PIN_SET
#define BUZZER_BEEP_DURATION_MS   100U
#define BUZZER_WARNING_PERIOD_MS  1000U
#define BUZZER_ALARM_PERIOD_MS    300U

static uint32_t s_buzzer_cycle_start_tick = 0U;
static SystemStatus_t s_last_buzzer_status = SYSTEM_STATUS_NORMAL;

static void AlarmTask_BuzzerOff(void)
{
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, BUZZER_OFF_STATE);
}

static void AlarmTask_BuzzerOn(void)
{
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, BUZZER_ON_STATE);
}

static void AlarmTask_InitBuzzerGpio(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, BUZZER_OFF_STATE);

  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);
}

static void AlarmTask_UpdateBuzzer(SystemStatus_t status)
{
  uint32_t now_tick = HAL_GetTick();
  uint32_t period_ms;

  if ((VibrationAlgo_GetState() == VIBRATION_CALIBRATING) || (status == SYSTEM_STATUS_NORMAL))
  {
    AlarmTask_BuzzerOff();
    s_last_buzzer_status = status;
    s_buzzer_cycle_start_tick = now_tick;
    return;
  }

  if (status == SYSTEM_STATUS_ALARM)
  {
    period_ms = BUZZER_ALARM_PERIOD_MS;
  }
  else
  {
    period_ms = BUZZER_WARNING_PERIOD_MS;
  }

  if (s_last_buzzer_status != status)
  {
    s_last_buzzer_status = status;
    s_buzzer_cycle_start_tick = now_tick;
  }

  if ((now_tick - s_buzzer_cycle_start_tick) >= period_ms)
  {
    s_buzzer_cycle_start_tick = now_tick;
  }

  if ((now_tick - s_buzzer_cycle_start_tick) < BUZZER_BEEP_DURATION_MS)
  {
    AlarmTask_BuzzerOn();
  }
  else
  {
    AlarmTask_BuzzerOff();
  }
}

void AlarmTask_Init(void)
{
  HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, LED_OFF_STATE);
  AlarmTask_InitBuzzerGpio();
  AlarmTask_BuzzerOff();
  s_buzzer_cycle_start_tick = HAL_GetTick();
  s_last_buzzer_status = SYSTEM_STATUS_NORMAL;
}

void AlarmTask_Update(float rms_value)
{
  SystemStatus_t current_status;

  (void)rms_value;
  current_status = SystemStatus_GetCurrent();

  if (VibrationAlgo_GetState() == VIBRATION_CALIBRATING)
  {
    HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, LED_OFF_STATE);
    AlarmTask_UpdateBuzzer(SYSTEM_STATUS_NORMAL);
    return;
  }

  if (SystemStatus_IsAlarm(current_status) != 0U)
  {
    HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, LED_ON_STATE);
  }
  else
  {
    HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, LED_OFF_STATE);
  }

  AlarmTask_UpdateBuzzer(current_status);
}
