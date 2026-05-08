#include "debug_uart.h"
#include <stdio.h>

#define DEBUG_UART_TIMEOUT_MS  100U

static UART_HandleTypeDef *s_debug_uart = 0;

static int DebugUart_FloatToX1000(float value)
{
  if (value >= 0.0f)
  {
    return (int)((value * 1000.0f) + 0.5f);
  }

  return (int)((value * 1000.0f) - 0.5f);
}

static void DebugUart_FormatSignedX1000(char *buffer, uint16_t buffer_size, int value_x1000)
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

void DebugUart_Init(UART_HandleTypeDef *huart)
{
  s_debug_uart = huart;
}

void DebugUart_SendText(const char *text)
{
  uint16_t length = 0U;

  if ((s_debug_uart == 0) || (text == 0))
  {
    return;
  }

  while (text[length] != '\0')
  {
    length++;
  }

  if (length > 0U)
  {
    (void)HAL_UART_Transmit(s_debug_uart,
                            (uint8_t *)text,
                            length,
                            DEBUG_UART_TIMEOUT_MS);
  }
}

void DebugUart_SendLine(const char *text)
{
  DebugUart_SendText(text);
  DebugUart_SendText("\r\n");
}

void DebugUart_PrintCsvHeader(void)
{
  DebugUart_SendLine("time_ms,ax,ay,az,rms,status");
}

void DebugUart_PrintSample(uint32_t time_ms,
                           float ax,
                           float ay,
                           float az,
                           float rms,
                           const char *status)
{
  char tx_buffer[96];
  char ax_text[12];
  char ay_text[12];
  char az_text[12];
  char rms_text[12];
  int length;

  if (status == 0)
  {
    status = "UNKNOWN";
  }

  DebugUart_FormatSignedX1000(ax_text, sizeof(ax_text), DebugUart_FloatToX1000(ax));
  DebugUart_FormatSignedX1000(ay_text, sizeof(ay_text), DebugUart_FloatToX1000(ay));
  DebugUart_FormatSignedX1000(az_text, sizeof(az_text), DebugUart_FloatToX1000(az));
  DebugUart_FormatSignedX1000(rms_text, sizeof(rms_text), DebugUart_FloatToX1000(rms));

  length = snprintf(tx_buffer,
                    sizeof(tx_buffer),
                    "%lu,%s,%s,%s,%s,%s",
                    (unsigned long)time_ms,
                    ax_text,
                    ay_text,
                    az_text,
                    rms_text,
                    status);

  if ((length > 0) && (length < (int)sizeof(tx_buffer)))
  {
    DebugUart_SendLine(tx_buffer);
  }
}
