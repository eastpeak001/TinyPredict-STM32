#ifndef MPU6050_H
#define MPU6050_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  int16_t ax_raw;
  int16_t ay_raw;
  int16_t az_raw;
  float ax_g;
  float ay_g;
  float az_g;
} MPU6050_AccelData_t;

HAL_StatusTypeDef MPU6050_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef MPU6050_ReadWhoAmI(I2C_HandleTypeDef *hi2c, uint8_t *who_am_i);
HAL_StatusTypeDef MPU6050_ReadAccel(I2C_HandleTypeDef *hi2c, MPU6050_AccelData_t *accel_data);

#ifdef __cplusplus
}
#endif

#endif /* MPU6050_H */
