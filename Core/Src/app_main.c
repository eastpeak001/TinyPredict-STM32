#include "app_main.h"

/* #define APP_BRINGUP_TEST */
#include "sensor_task.h"
#include "vibration_algo.h"
#include "alarm_task.h"
#include "oled_ui.h"
#include "ssd1306.h"
#include "debug_uart.h"
#include "system_status.h"
#include "main.h"

#ifdef APP_BRINGUP_TEST
#include <stdio.h>
#define APP_BRINGUP_LED_PERIOD_MS     500U
#define APP_BRINGUP_SAMPLE_PERIOD_MS  500U
#else
#define APP_UPDATE_PERIOD_MS          100U
#define APP_UART_PERIOD_MS            500U
#define APP_OLED_PERIOD_MS            300U
#endif

extern UART_HandleTypeDef huart1;
#ifdef APP_BRINGUP_TEST
extern I2C_HandleTypeDef hi2c1;
#endif

#ifdef APP_BRINGUP_TEST
static uint32_t s_last_bringup_led_tick = 0U;
static uint32_t s_last_bringup_sample_tick = 0U;
static uint8_t s_bringup_oled_ready = 0U;

static int App_BringupFloatToX1000(float value)
{
  if (value >= 0.0f)
  {
    return (int)((value * 1000.0f) + 0.5f);
  }

  return (int)((value * 1000.0f) - 0.5f);
}

static void App_BringupFormatSignedX1000(char *buffer, uint16_t buffer_size, int value_x1000)
{
  if ((buffer == 0) || (buffer_size == 0U))
  {
    return;
  }

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

static void App_BringupDrawOledMessage(const char *line3, const char *line4)
{
  if (s_bringup_oled_ready == 0U)
  {
    return;
  }

  SSD1306_Fill(0U);
  SSD1306_DrawString(0U, 0U, "TinyPredict");
  SSD1306_DrawString(0U, 16U, "MPU6050 Test");
  SSD1306_DrawString(0U, 32U, line3);
  SSD1306_DrawString(0U, 48U, line4);
  SSD1306_UpdateScreen();
}

static void App_BringupUpdateMpu6050(void)
{
  char ax_text[12];
  char ay_text[12];
  char az_text[12];
  char uart_line[64];
  char oled_line3[32];
  char oled_line4[32];
  int length;

  if (SensorTask_IsReady() == 0U)
  {
    App_BringupDrawOledMessage("MPU6050 FAIL", "Check I2C/Wiring");
    DebugUart_SendLine("MPU6050 init failed");
    return;
  }

  SensorTask_Update();

  if (SensorTask_IsReady() == 0U)
  {
    App_BringupDrawOledMessage("MPU6050 FAIL", "Read failed");
    DebugUart_SendLine("MPU6050 init failed");
    return;
  }

  App_BringupFormatSignedX1000(ax_text, sizeof(ax_text), App_BringupFloatToX1000(SensorTask_GetAccelX()));
  App_BringupFormatSignedX1000(ay_text, sizeof(ay_text), App_BringupFloatToX1000(SensorTask_GetAccelY()));
  App_BringupFormatSignedX1000(az_text, sizeof(az_text), App_BringupFloatToX1000(SensorTask_GetAccelZ()));

  length = snprintf(uart_line,
                    sizeof(uart_line),
                    "ax=%s, ay=%s, az=%s",
                    ax_text,
                    ay_text,
                    az_text);
  if ((length > 0) && (length < (int)sizeof(uart_line)))
  {
    DebugUart_SendLine(uart_line);
  }

  (void)snprintf(oled_line3, sizeof(oled_line3), "ax=%s", ax_text);
  (void)snprintf(oled_line4, sizeof(oled_line4), "ay=%s z=%s", ay_text, az_text);
  App_BringupDrawOledMessage(oled_line3, oled_line4);
}

void App_Init(void)
{
  uint32_t now_tick;

  DebugUart_Init(&huart1);

  if (SSD1306_Init(&hi2c1) == HAL_OK)
  {
    s_bringup_oled_ready = 1U;
    App_BringupDrawOledMessage("MPU6050 INIT", "Please wait");
  }
  else
  {
    s_bringup_oled_ready = 0U;
  }

  SensorTask_Init();
  if (SensorTask_IsReady() == 0U)
  {
    App_BringupDrawOledMessage("MPU6050 FAIL", "Check I2C/Wiring");
    DebugUart_SendLine("MPU6050 init failed");
  }

  now_tick = HAL_GetTick();
  s_last_bringup_led_tick = now_tick;
  s_last_bringup_sample_tick = now_tick;

  HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
}

void App_MainLoop(void)
{
  uint32_t now_tick = HAL_GetTick();

  if ((now_tick - s_last_bringup_led_tick) >= APP_BRINGUP_LED_PERIOD_MS)
  {
    s_last_bringup_led_tick = now_tick;
    HAL_GPIO_TogglePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin);
  }

  if ((now_tick - s_last_bringup_sample_tick) >= APP_BRINGUP_SAMPLE_PERIOD_MS)
  {
    s_last_bringup_sample_tick = now_tick;
    App_BringupUpdateMpu6050();
  }
}
#else
static uint32_t s_last_update_tick = 0U;
static uint32_t s_last_uart_tick = 0U;
static uint32_t s_last_oled_tick = 0U;
static float s_current_rms = 0.0f;

static const char *App_GetStatusText(void)
{
  if (VibrationAlgo_GetState() == VIBRATION_CALIBRATING)
  {
    return "CALIBRATING";
  }

  return SystemStatus_GetCurrentString();
}

static void App_SendStatusMessage(uint32_t time_ms, float rms_value)
{
  if (SensorTask_IsReady() == 0U)
  {
    DebugUart_SendLine("MPU6050 init failed");
    return;
  }

  DebugUart_PrintSample(time_ms,
                        SensorTask_GetAccelX(),
                        SensorTask_GetAccelY(),
                        SensorTask_GetAccelZ(),
                        rms_value,
                        App_GetStatusText());
}

void App_Init(void)
{
  DebugUart_Init(&huart1);

  SensorTask_Init();
  VibrationAlgo_Init();
  SystemStatus_Init();
  AlarmTask_Init();

  if (OledUI_Init() != 0U)
  {
    DebugUart_SendLine("OLED init ok");
    OledUI_ShowLogoBoot();
  }
  else
  {
    DebugUart_SendLine("OLED init failed");
  }

  DebugUart_PrintCsvHeader();

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
    SystemStatus_UpdateFromRms(s_current_rms);
    AlarmTask_Update(s_current_rms);
  }

  if ((now_tick - s_last_oled_tick) >= APP_OLED_PERIOD_MS)
  {
    s_last_oled_tick = now_tick;
    OledUI_Update(s_current_rms, App_GetStatusText());
  }

  if ((now_tick - s_last_uart_tick) >= APP_UART_PERIOD_MS)
  {
    s_last_uart_tick = now_tick;
    App_SendStatusMessage(now_tick, s_current_rms);
  }
}
#endif



