#include "oled_ui.h"
#include "ssd1306.h"
#include "oled_logo_pg.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

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

void OledUI_ShowLogoBoot(void)
{
  enum
  {
    LOGO_SCAN_FRAMES = 8,
    LOGO_SCAN_FRAME_DELAY_MS = 55,
    LOGO_FINAL_HOLD_MS = 600,
    LOGO_READY_HOLD_MS = 800
  };
  uint8_t logo_x = (uint8_t)((SSD1306_WIDTH - OLED_LOGO_PG_WIDTH) / 2U);
  uint8_t logo_y = (uint8_t)((SSD1306_HEIGHT - OLED_LOGO_PG_HEIGHT) / 2U);
  uint8_t frame;
  uint8_t row_bytes = (uint8_t)((OLED_LOGO_PG_WIDTH + 7U) / 8U);
  uint8_t partial_bitmap[sizeof(OLED_LOGO_PG_BITMAP)];

  if (s_oled_ready == 0U)
  {
    return;
  }

  SSD1306_Fill(0U);
  SSD1306_UpdateScreen();

  for (frame = 1U; frame <= LOGO_SCAN_FRAMES; frame++)
  {
    uint8_t visible_rows = (uint8_t)((OLED_LOGO_PG_HEIGHT * frame) / LOGO_SCAN_FRAMES);
    uint16_t visible_bytes = (uint16_t)visible_rows * row_bytes;

    (void)memset(partial_bitmap, 0, sizeof(partial_bitmap));
    (void)memcpy(partial_bitmap, OLED_LOGO_PG_BITMAP, visible_bytes);

    SSD1306_Fill(0U);
    SSD1306_DrawBitmap(logo_x, logo_y, partial_bitmap, OLED_LOGO_PG_WIDTH, OLED_LOGO_PG_HEIGHT);
    SSD1306_UpdateScreen();
    HAL_Delay(LOGO_SCAN_FRAME_DELAY_MS);
  }

  SSD1306_Fill(0U);
  SSD1306_DrawBitmap(logo_x, logo_y, OLED_LOGO_PG_BITMAP, OLED_LOGO_PG_WIDTH, OLED_LOGO_PG_HEIGHT);
  SSD1306_UpdateScreen();
  HAL_Delay(LOGO_FINAL_HOLD_MS);

  SSD1306_Fill(0U);
  SSD1306_DrawString(0U, 8U, "TinyPredict");
  SSD1306_DrawString(0U, 28U, "V0.4B Ready");
  SSD1306_UpdateScreen();
  HAL_Delay(LOGO_READY_HOLD_MS);
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


