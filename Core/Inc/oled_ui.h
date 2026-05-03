#ifndef OLED_UI_H
#define OLED_UI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t OledUI_Init(void);
void OledUI_Update(float rms_value, const char *status_text);
uint8_t OledUI_IsReady(void);

#ifdef __cplusplus
}
#endif

#endif /* OLED_UI_H */
