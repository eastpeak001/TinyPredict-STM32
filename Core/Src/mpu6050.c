#include "mpu6050.h"

#define MPU6050_ADDR              (0x68U << 1)
#define MPU6050_REG_WHO_AM_I      0x75U
#define MPU6050_REG_PWR_MGMT_1    0x6BU
#define MPU6050_REG_ACCEL_CONFIG  0x1CU
#define MPU6050_REG_ACCEL_XOUT_H  0x3BU

#define MPU6050_WHO_AM_I_VALUE    0x68U
#define MPU6050_I2C_TIMEOUT_MS    100U
#define MPU6050_ACCEL_SCALE_2G    16384.0f

static HAL_StatusTypeDef MPU6050_WriteReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(hi2c,
                           MPU6050_ADDR,
                           reg,
                           I2C_MEMADD_SIZE_8BIT,
                           &value,
                           1U,
                           MPU6050_I2C_TIMEOUT_MS);
}

static HAL_StatusTypeDef MPU6050_ReadRegs(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data, uint16_t length)
{
  return HAL_I2C_Mem_Read(hi2c,
                          MPU6050_ADDR,
                          reg,
                          I2C_MEMADD_SIZE_8BIT,
                          data,
                          length,
                          MPU6050_I2C_TIMEOUT_MS);
}

HAL_StatusTypeDef MPU6050_ReadWhoAmI(I2C_HandleTypeDef *hi2c, uint8_t *who_am_i)
{
  if ((hi2c == NULL) || (who_am_i == NULL))
  {
    return HAL_ERROR;
  }

  return MPU6050_ReadRegs(hi2c, MPU6050_REG_WHO_AM_I, who_am_i, 1U);
}

HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c)
{
  uint8_t who_am_i = 0U;

  if (MPU6050_ReadWhoAmI(hi2c, &who_am_i) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (who_am_i != MPU6050_WHO_AM_I_VALUE)
  {
    return HAL_ERROR;
  }

  if (MPU6050_WriteReg(hi2c, MPU6050_REG_PWR_MGMT_1, 0x00U) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (MPU6050_WriteReg(hi2c, MPU6050_REG_ACCEL_CONFIG, 0x00U) != HAL_OK)
  {
    return HAL_ERROR;
  }

  HAL_Delay(100U);

  return HAL_OK;
}

HAL_StatusTypeDef MPU6050_ReadAccel(I2C_HandleTypeDef *hi2c, MPU6050_AccelData_t *accel_data)
{
  uint8_t raw_data[6];

  if ((hi2c == NULL) || (accel_data == NULL))
  {
    return HAL_ERROR;
  }

  if (MPU6050_ReadRegs(hi2c, MPU6050_REG_ACCEL_XOUT_H, raw_data, sizeof(raw_data)) != HAL_OK)
  {
    return HAL_ERROR;
  }

  accel_data->ax_raw = (int16_t)((raw_data[0] << 8) | raw_data[1]);
  accel_data->ay_raw = (int16_t)((raw_data[2] << 8) | raw_data[3]);
  accel_data->az_raw = (int16_t)((raw_data[4] << 8) | raw_data[5]);

  accel_data->ax_g = (float)accel_data->ax_raw / MPU6050_ACCEL_SCALE_2G;
  accel_data->ay_g = (float)accel_data->ay_raw / MPU6050_ACCEL_SCALE_2G;
  accel_data->az_g = (float)accel_data->az_raw / MPU6050_ACCEL_SCALE_2G;

  return HAL_OK;
}
