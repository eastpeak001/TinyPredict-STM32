#ifndef DEBUG_UART_H
#define DEBUG_UART_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void DebugUart_Init(UART_HandleTypeDef *huart);
void DebugUart_SendText(const char *text);
void DebugUart_SendLine(const char *text);
void DebugUart_PrintCsvHeader(void);
void DebugUart_PrintSample(uint32_t time_ms,
                           float ax,
                           float ay,
                           float az,
                           float rms,
                           const char *status);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_UART_H */
