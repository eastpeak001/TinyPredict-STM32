#include "sensor_task.h"
#include "mpu6050.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

static uint8_t s_sensor_ready = 0U;
static float s_accel_x_g = 0.0f;
static float s_accel_y_g = 0.0f;
static float s_accel_z_g = 0.0f;
static float s_latest_sample = 0.0f;

void SensorTask_Init(void)
{
  s_sensor_ready = 0U;
  s_accel_x_g = 0.0f;
  s_accel_y_g = 0.0f;
  s_accel_z_g = 0.0f;
  s_latest_sample = 0.0f;

  if (MPU6050_Init(&hi2c1) == HAL_OK)
  {
    s_sensor_ready = 1U;
  }
}

void SensorTask_Update(void)
{
  MPU6050_AccelData_t accel_data;

  if (s_sensor_ready == 0U)
  {
    return;
  }

  if (MPU6050_ReadAccel(&hi2c1, &accel_data) == HAL_OK)
  {
    s_accel_x_g = accel_data.ax_g;
    s_accel_y_g = accel_data.ay_g;
    s_accel_z_g = accel_data.az_g;
    s_latest_sample = s_accel_x_g;
  }
  else
  {
    s_sensor_ready = 0U;
  }
}

uint8_t SensorTask_IsReady(void)
{
  return s_sensor_ready;
}

float SensorTask_GetAccelX(void)
{
  return s_accel_x_g;
}

float SensorTask_GetAccelY(void)
{
  return s_accel_y_g;
}

float SensorTask_GetAccelZ(void)
{
  return s_accel_z_g;
}

float SensorTask_GetLatestSample(void)
{
  return s_latest_sample;
}
