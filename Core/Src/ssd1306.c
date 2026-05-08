#include "ssd1306.h"
#include <string.h>

#define SSD1306_I2C_ADDR        (0x3CU << 1)
#define SSD1306_CMD_TIMEOUT_MS  100U
#define SSD1306_DATA_TIMEOUT_MS 100U
#define SSD1306_BUFFER_SIZE     (SSD1306_WIDTH * SSD1306_HEIGHT / 8U)

static I2C_HandleTypeDef *s_hi2c = 0;
static uint8_t s_buffer[SSD1306_BUFFER_SIZE];
static uint8_t s_ready = 0U;

static HAL_StatusTypeDef SSD1306_WriteCommand(uint8_t command)
{
  uint8_t data[2];

  data[0] = 0x00U;
  data[1] = command;

  return HAL_I2C_Master_Transmit(s_hi2c,
                                 SSD1306_I2C_ADDR,
                                 data,
                                 sizeof(data),
                                 SSD1306_CMD_TIMEOUT_MS);
}

static HAL_StatusTypeDef SSD1306_WriteData(uint8_t *data, uint16_t size)
{
  uint8_t tx_buffer[17];
  uint16_t offset = 0U;

  tx_buffer[0] = 0x40U;

  while (offset < size)
  {
    uint16_t chunk_size = size - offset;

    if (chunk_size > 16U)
    {
      chunk_size = 16U;
    }

    memcpy(&tx_buffer[1], &data[offset], chunk_size);

    if (HAL_I2C_Master_Transmit(s_hi2c,
                                SSD1306_I2C_ADDR,
                                tx_buffer,
                                (uint16_t)(chunk_size + 1U),
                                SSD1306_DATA_TIMEOUT_MS) != HAL_OK)
    {
      return HAL_ERROR;
    }

    offset = (uint16_t)(offset + chunk_size);
  }

  return HAL_OK;
}

static const uint8_t *SSD1306_GetGlyph(char character)
{
  static const uint8_t blank[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
  static const uint8_t dot[5] = {0x00, 0x60, 0x60, 0x00, 0x00};
  static const uint8_t colon[5] = {0x00, 0x36, 0x36, 0x00, 0x00};
  static const uint8_t zero[5] = {0x3E, 0x51, 0x49, 0x45, 0x3E};
  static const uint8_t one[5] = {0x00, 0x42, 0x7F, 0x40, 0x00};
  static const uint8_t two[5] = {0x42, 0x61, 0x51, 0x49, 0x46};
  static const uint8_t three[5] = {0x21, 0x41, 0x45, 0x4B, 0x31};
  static const uint8_t four[5] = {0x18, 0x14, 0x12, 0x7F, 0x10};
  static const uint8_t five[5] = {0x27, 0x45, 0x45, 0x45, 0x39};
  static const uint8_t six[5] = {0x3C, 0x4A, 0x49, 0x49, 0x30};
  static const uint8_t seven[5] = {0x01, 0x71, 0x09, 0x05, 0x03};
  static const uint8_t eight[5] = {0x36, 0x49, 0x49, 0x49, 0x36};
  static const uint8_t nine[5] = {0x06, 0x49, 0x49, 0x29, 0x1E};
  static const uint8_t glyph_a_upper[5] = {0x7E, 0x11, 0x11, 0x11, 0x7E};
  static const uint8_t glyph_b_upper[5] = {0x7F, 0x49, 0x49, 0x49, 0x36};
  static const uint8_t glyph_c_upper[5] = {0x3E, 0x41, 0x41, 0x41, 0x22};
  static const uint8_t glyph_d_upper[5] = {0x7F, 0x41, 0x41, 0x22, 0x1C};
  static const uint8_t glyph_e_upper[5] = {0x7F, 0x49, 0x49, 0x49, 0x41};
  static const uint8_t glyph_g_upper[5] = {0x3E, 0x41, 0x49, 0x49, 0x7A};
  static const uint8_t glyph_i_upper[5] = {0x00, 0x41, 0x7F, 0x41, 0x00};
  static const uint8_t glyph_l_upper[5] = {0x7F, 0x40, 0x40, 0x40, 0x40};
  static const uint8_t glyph_m_upper[5] = {0x7F, 0x02, 0x0C, 0x02, 0x7F};
  static const uint8_t glyph_n_upper[5] = {0x7F, 0x04, 0x08, 0x10, 0x7F};
  static const uint8_t glyph_o_upper[5] = {0x3E, 0x41, 0x41, 0x41, 0x3E};
  static const uint8_t glyph_p_upper[5] = {0x7F, 0x09, 0x09, 0x09, 0x06};
  static const uint8_t glyph_r_upper[5] = {0x7F, 0x09, 0x19, 0x29, 0x46};
  static const uint8_t glyph_s_upper[5] = {0x46, 0x49, 0x49, 0x49, 0x31};
  static const uint8_t glyph_t_upper[5] = {0x01, 0x01, 0x7F, 0x01, 0x01};
  static const uint8_t glyph_v_upper[5] = {0x1F, 0x20, 0x40, 0x20, 0x1F};
  static const uint8_t glyph_w_upper[5] = {0x7F, 0x20, 0x18, 0x20, 0x7F};
  static const uint8_t glyph_y_upper[5] = {0x07, 0x08, 0x70, 0x08, 0x07};
  static const uint8_t glyph_a[5] = {0x20, 0x54, 0x54, 0x54, 0x78};
  static const uint8_t glyph_c[5] = {0x38, 0x44, 0x44, 0x44, 0x20};
  static const uint8_t glyph_d[5] = {0x38, 0x44, 0x44, 0x48, 0x7F};
  static const uint8_t glyph_e[5] = {0x38, 0x54, 0x54, 0x54, 0x18};
  static const uint8_t glyph_i[5] = {0x00, 0x44, 0x7D, 0x40, 0x00};
  static const uint8_t glyph_n[5] = {0x7C, 0x08, 0x04, 0x04, 0x78};
  static const uint8_t glyph_o[5] = {0x38, 0x44, 0x44, 0x44, 0x38};
  static const uint8_t glyph_p[5] = {0x7C, 0x14, 0x14, 0x14, 0x08};
  static const uint8_t glyph_r[5] = {0x7C, 0x08, 0x04, 0x04, 0x08};
  static const uint8_t glyph_s[5] = {0x48, 0x54, 0x54, 0x54, 0x20};
  static const uint8_t glyph_t[5] = {0x04, 0x3F, 0x44, 0x40, 0x20};
  static const uint8_t glyph_u[5] = {0x3C, 0x40, 0x40, 0x20, 0x7C};
  static const uint8_t glyph_y[5] = {0x0C, 0x50, 0x50, 0x50, 0x3C};

  switch (character)
  {
    case ' ': return blank;
    case '.': return dot;
    case ':': return colon;
    case '0': return zero;
    case '1': return one;
    case '2': return two;
    case '3': return three;
    case '4': return four;
    case '5': return five;
    case '6': return six;
    case '7': return seven;
    case '8': return eight;
    case '9': return nine;
    case 'A': return glyph_a_upper;
    case 'B': return glyph_b_upper;
    case 'C': return glyph_c_upper;
    case 'D': return glyph_d_upper;
    case 'E': return glyph_e_upper;
    case 'G': return glyph_g_upper;
    case 'I': return glyph_i_upper;
    case 'L': return glyph_l_upper;
    case 'M': return glyph_m_upper;
    case 'N': return glyph_n_upper;
    case 'O': return glyph_o_upper;
    case 'P': return glyph_p_upper;
    case 'R': return glyph_r_upper;
    case 'S': return glyph_s_upper;
    case 'T': return glyph_t_upper;
    case 'V': return glyph_v_upper;
    case 'W': return glyph_w_upper;
    case 'Y': return glyph_y_upper;
    case 'a': return glyph_a;
    case 'c': return glyph_c;
    case 'd': return glyph_d;
    case 'e': return glyph_e;
    case 'i': return glyph_i;
    case 'n': return glyph_n;
    case 'o': return glyph_o;
    case 'p': return glyph_p;
    case 'r': return glyph_r;
    case 's': return glyph_s;
    case 't': return glyph_t;
    case 'u': return glyph_u;
    case 'y': return glyph_y;
    default: return blank;
  }
}

static void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  if ((x >= SSD1306_WIDTH) || (y >= SSD1306_HEIGHT))
  {
    return;
  }

  if (color != 0U)
  {
    s_buffer[x + ((uint16_t)(y / 8U) * SSD1306_WIDTH)] |= (uint8_t)(1U << (y % 8U));
  }
  else
  {
    s_buffer[x + ((uint16_t)(y / 8U) * SSD1306_WIDTH)] &= (uint8_t)~(uint8_t)(1U << (y % 8U));
  }
}

static void SSD1306_DrawChar(uint8_t x, uint8_t y, char character)
{
  const uint8_t *glyph = SSD1306_GetGlyph(character);
  uint8_t column;

  for (column = 0U; column < 5U; column++)
  {
    uint8_t line = glyph[column];
    uint8_t row;

    for (row = 0U; row < 7U; row++)
    {
      if ((line & (1U << row)) != 0U)
      {
        SSD1306_DrawPixel((uint8_t)(x + column), (uint8_t)(y + row), 1U);
      }
    }
  }
}

HAL_StatusTypeDef SSD1306_Init(I2C_HandleTypeDef *hi2c)
{
  static const uint8_t init_sequence[] = {
    0xAE, 0x20, 0x00, 0xB0, 0xC8, 0x00, 0x10, 0x40,
    0x81, 0x7F, 0xA1, 0xA6, 0xA8, 0x3F, 0xA4, 0xD3,
    0x00, 0xD5, 0x80, 0xD9, 0xF1, 0xDA, 0x12, 0xDB,
    0x40, 0x8D, 0x14, 0xAF
  };
  uint8_t i;

  s_hi2c = hi2c;
  s_ready = 0U;

  if (s_hi2c == 0)
  {
    return HAL_ERROR;
  }

  if (HAL_I2C_IsDeviceReady(s_hi2c, SSD1306_I2C_ADDR, 2U, SSD1306_CMD_TIMEOUT_MS) != HAL_OK)
  {
    return HAL_ERROR;
  }

  for (i = 0U; i < sizeof(init_sequence); i++)
  {
    if (SSD1306_WriteCommand(init_sequence[i]) != HAL_OK)
    {
      return HAL_ERROR;
    }
  }

  SSD1306_Fill(0U);
  s_ready = 1U;
  SSD1306_UpdateScreen();

  return HAL_OK;
}

void SSD1306_Fill(uint8_t color)
{
  memset(s_buffer, (color != 0U) ? 0xFF : 0x00, sizeof(s_buffer));
}

void SSD1306_UpdateScreen(void)
{
  uint8_t page;

  if (s_ready == 0U)
  {
    return;
  }

  for (page = 0U; page < 8U; page++)
  {
    (void)SSD1306_WriteCommand((uint8_t)(0xB0U + page));
    (void)SSD1306_WriteCommand(0x00U);
    (void)SSD1306_WriteCommand(0x10U);
    (void)SSD1306_WriteData(&s_buffer[(uint16_t)page * SSD1306_WIDTH], SSD1306_WIDTH);
  }
}

void SSD1306_DrawString(uint8_t x, uint8_t y, const char *text)
{
  uint8_t cursor_x = x;

  if (text == 0)
  {
    return;
  }

  while ((*text != '\0') && (cursor_x < (SSD1306_WIDTH - 6U)))
  {
    SSD1306_DrawChar(cursor_x, y, *text);
    cursor_x = (uint8_t)(cursor_x + 6U);
    text++;
  }
}

void SSD1306_DrawBitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t width, uint8_t height)
{
  uint8_t row;
  uint8_t column;
  uint8_t row_bytes;

  if ((bitmap == 0) || (width == 0U) || (height == 0U))
  {
    return;
  }

  row_bytes = (uint8_t)((width + 7U) / 8U);

  for (row = 0U; row < height; row++)
  {
    for (column = 0U; column < width; column++)
    {
      uint16_t byte_index = ((uint16_t)row * row_bytes) + (column / 8U);
      uint8_t bit_mask = (uint8_t)(0x80U >> (column % 8U));

      if ((bitmap[byte_index] & bit_mask) != 0U)
      {
        SSD1306_DrawPixel((uint8_t)(x + column), (uint8_t)(y + row), 1U);
      }
    }
  }
}
uint8_t SSD1306_IsReady(void)
{
  return s_ready;
}


