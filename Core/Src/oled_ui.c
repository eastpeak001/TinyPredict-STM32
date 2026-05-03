#include "oled_ui.h"
#include "ssd1306.h"
#include "main.h"
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;

static uint8_t s_oled_ready = 0U;

static int OledUI_FloatToX1000(float value)
{
  if (value >= 0.0f)
  {
    return (int)((value * 1000.0f) + 0.5f);
  }

  return (int)((value * 1000.0f) - 0.5f);
}

static void OledUI_FormatRms(char *buffer, uint16_t buffer_size, float value)
{
  int value_x1000 = OledUI_FloatToX1000(value);

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

uint8_t OledUI_Init(void)
{
  if (SSD1306_Init(&hi2c1) == HAL_OK)
  {
    s_oled_ready = 1U;
    SSD1306_Fill(0U);
    SSD1306_DrawString(0U, 0U, "TinyPredict");
    SSD1306_DrawString(0U, 16U, "RMS: 0.000");
    SSD1306_DrawString(0U, 32U, "Status: CALIBRATING");
    SSD1306_DrawString(0U, 48U, "V0.3 OLED");
    SSD1306_UpdateScreen();
  }
  else
  {
    s_oled_ready = 0U;
  }

  return s_oled_ready;
}

void OledUI_Update(float rms_value, const char *status_text)
{
  char line[24];
  char rms_text[12];

  if (s_oled_ready == 0U)
  {
    return;
  }

  OledUI_FormatRms(rms_text, sizeof(rms_text), rms_value);

  SSD1306_Fill(0U);
  SSD1306_DrawString(0U, 0U, "TinyPredict");

  (void)snprintf(line, sizeof(line), "RMS: %s", rms_text);
  SSD1306_DrawString(0U, 16U, line);

  (void)snprintf(line, sizeof(line), "Status: %s", status_text);
  SSD1306_DrawString(0U, 32U, line);

  SSD1306_DrawString(0U, 48U, "V0.3 OLED");
  SSD1306_UpdateScreen();
}

uint8_t OledUI_IsReady(void)
{
  return s_oled_ready;
}
