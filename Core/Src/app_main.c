#include "app_main.h"
#include "sensor_task.h"
#include "vibration_algo.h"
#include "alarm_task.h"
#include "oled_ui.h"
#include "main.h"
#include <stdio.h>

#define APP_UPDATE_PERIOD_MS   100U
#define APP_UART_PERIOD_MS     500U
#define APP_OLED_PERIOD_MS     300U
#define APP_WARNING_RMS        0.05f
#define APP_ALARM_RMS          0.15f
#define APP_UART_TIMEOUT_MS    100U

extern UART_HandleTypeDef huart1;

static uint32_t s_last_update_tick = 0U;
static uint32_t s_last_uart_tick = 0U;
static uint32_t s_last_oled_tick = 0U;
static float s_current_rms = 0.0f;

static const char *App_GetStatusText(float rms_value)
{
  if (VibrationAlgo_GetState() == VIBRATION_CALIBRATING)
  {
    return "CALIBRATING";
  }

  if (rms_value >= APP_ALARM_RMS)
  {
    return "ALARM";
  }

  if (rms_value >= APP_WARNING_RMS)
  {
    return "WARNING";
  }

  return "NORMAL";
}

static int App_FloatToX1000(float value)
{
  if (value >= 0.0f)
  {
    return (int)((value * 1000.0f) + 0.5f);
  }

  return (int)((value * 1000.0f) - 0.5f);
}

static void App_FormatSignedX1000(char *buffer, uint16_t buffer_size, int value_x1000)
{
  if (value_x1000 < 0)
  {
    value_x1000 = -value_x1000;
    (void)snprintf(buffer, buffer_size, "-%d.%03d", value_x1000 / 1000, value_x1000 % 1000);
  }
  else
  {
    (void)snprintf(buffer, buffer_size, "%d.%03d", value_x1000 / 1000, value_x1000 % 1000);
  }
}

static void App_SendText(const char *text)
{
  uint16_t length = 0U;

  while (text[length] != '\0')
  {
    length++;
  }

  HAL_UART_Transmit(&huart1, (uint8_t *)text, length, APP_UART_TIMEOUT_MS);
}

static void App_SendStatusMessage(float rms_value)
{
  char tx_buffer[176];
  char ax_text[12];
  char ay_text[12];
  char az_text[12];
  char vx_text[12];
  char vy_text[12];
  char vz_text[12];
  char rms_text[12];
  int length;

  if (SensorTask_IsReady() == 0U)
  {
    App_SendText("MPU6050 init failed\r\n");
    return;
  }

  App_FormatSignedX1000(ax_text, sizeof(ax_text), App_FloatToX1000(SensorTask_GetAccelX()));
  App_FormatSignedX1000(ay_text, sizeof(ay_text), App_FloatToX1000(SensorTask_GetAccelY()));
  App_FormatSignedX1000(az_text, sizeof(az_text), App_FloatToX1000(SensorTask_GetAccelZ()));
  App_FormatSignedX1000(rms_text, sizeof(rms_text), App_FloatToX1000(rms_value));

  if (VibrationAlgo_GetState() == VIBRATION_CALIBRATING)
  {
    length = snprintf(tx_buffer,
                      sizeof(tx_buffer),
                      "Calibrating... ax=%s, ay=%s, az=%s, rms=%s, status=%s\r\n",
                      ax_text,
                      ay_text,
                      az_text,
                      rms_text,
                      App_GetStatusText(rms_value));
  }
  else
  {
    App_FormatSignedX1000(vx_text, sizeof(vx_text), App_FloatToX1000(VibrationAlgo_GetVibX()));
    App_FormatSignedX1000(vy_text, sizeof(vy_text), App_FloatToX1000(VibrationAlgo_GetVibY()));
    App_FormatSignedX1000(vz_text, sizeof(vz_text), App_FloatToX1000(VibrationAlgo_GetVibZ()));

    length = snprintf(tx_buffer,
                      sizeof(tx_buffer),
                      "ax=%s, ay=%s, az=%s, vx=%s, vy=%s, vz=%s, rms=%s, status=%s\r\n",
                      ax_text,
                      ay_text,
                      az_text,
                      vx_text,
                      vy_text,
                      vz_text,
                      rms_text,
                      App_GetStatusText(rms_value));
  }

  if ((length > 0) && (length < (int)sizeof(tx_buffer)))
  {
    HAL_UART_Transmit(&huart1,
                      (uint8_t *)tx_buffer,
                      (uint16_t)length,
                      APP_UART_TIMEOUT_MS);
  }
}

void App_Init(void)
{
  SensorTask_Init();
  VibrationAlgo_Init();
  AlarmTask_Init();

  if (OledUI_Init() != 0U)
  {
    App_SendText("OLED init ok\r\n");
  }
  else
  {
    App_SendText("OLED init failed\r\n");
  }

  s_last_update_tick = HAL_GetTick();
  s_last_uart_tick = s_last_update_tick;
  s_last_oled_tick = s_last_update_tick;
  s_current_rms = 0.0f;
}

void App_MainLoop(void)
{
  uint32_t now_tick = HAL_GetTick();

  if ((now_tick - s_last_update_tick) >= APP_UPDATE_PERIOD_MS)
  {
    s_last_update_tick = now_tick;

    SensorTask_Update();
    s_current_rms = VibrationAlgo_Update();
    AlarmTask_Update(s_current_rms);
  }

  if ((now_tick - s_last_oled_tick) >= APP_OLED_PERIOD_MS)
  {
    s_last_oled_tick = now_tick;
    OledUI_Update(s_current_rms, App_GetStatusText(s_current_rms));
  }

  if ((now_tick - s_last_uart_tick) >= APP_UART_PERIOD_MS)
  {
    s_last_uart_tick = now_tick;
    App_SendStatusMessage(s_current_rms);
  }
}
