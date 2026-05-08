#ifndef SSD1306_H
#define SSD1306_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SSD1306_WIDTH   128U
#define SSD1306_HEIGHT  64U

HAL_StatusTypeDef SSD1306_Init(I2C_HandleTypeDef *hi2c);
void SSD1306_Fill(uint8_t color);
void SSD1306_UpdateScreen(void);
void SSD1306_DrawString(uint8_t x, uint8_t y, const char *text);
void SSD1306_DrawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height);
uint8_t SSD1306_IsReady(void);

#ifdef __cplusplus
}
#endif

#endif /* SSD1306_H */

