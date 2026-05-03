#include "vibration_algo.h"
#include "sensor_task.h"
#include "main.h"
#include <math.h>
#include <stdint.h>

#define VIBRATION_GRAVITY_FAST_ALPHA   0.90f
#define VIBRATION_ACCEL_FAST_ALPHA     0.10f
#define VIBRATION_GRAVITY_SLOW_ALPHA   0.95f
#define VIBRATION_ACCEL_SLOW_ALPHA     0.05f
#define VIBRATION_LEARNING_TIME_MS     2000U
#define VIBRATION_RMS_WINDOW_SIZE      32U

static VibrationAlgo_State_t s_state = VIBRATION_CALIBRATING;
static uint32_t s_learning_start_tick = 0U;
static float s_gravity_x = 0.0f;
static float s_gravity_y = 0.0f;
static float s_gravity_z = 0.0f;
static float s_vib_x = 0.0f;
static float s_vib_y = 0.0f;
static float s_vib_z = 0.0f;
static float s_vib_mag_sq_buffer[VIBRATION_RMS_WINDOW_SIZE];
static float s_vib_mag_sq_sum = 0.0f;
static float s_last_rms = 0.0f;
static uint8_t s_gravity_initialized = 0U;
static uint8_t s_rms_index = 0U;
static uint8_t s_rms_count = 0U;

static void VibrationAlgo_ClearRmsWindow(void)
{
  uint8_t i;

  s_vib_mag_sq_sum = 0.0f;
  s_rms_index = 0U;
  s_rms_count = 0U;

  for (i = 0U; i < VIBRATION_RMS_WINDOW_SIZE; i++)
  {
    s_vib_mag_sq_buffer[i] = 0.0f;
  }
}

void VibrationAlgo_Init(void)
{
  s_state = VIBRATION_CALIBRATING;
  s_learning_start_tick = 0U;
  s_gravity_x = 0.0f;
  s_gravity_y = 0.0f;
  s_gravity_z = 0.0f;
  s_vib_x = 0.0f;
  s_vib_y = 0.0f;
  s_vib_z = 0.0f;
  s_last_rms = 0.0f;
  s_gravity_initialized = 0U;
  VibrationAlgo_ClearRmsWindow();
}

float VibrationAlgo_Update(void)
{
  float ax = SensorTask_GetAccelX();
  float ay = SensorTask_GetAccelY();
  float az = SensorTask_GetAccelZ();
  float vib_mag;
  float vib_mag_sq;
  float mean_sq;
  uint32_t now_tick = HAL_GetTick();

  if (SensorTask_IsReady() == 0U)
  {
    s_state = VIBRATION_CALIBRATING;
    s_last_rms = 0.0f;
    s_vib_x = 0.0f;
    s_vib_y = 0.0f;
    s_vib_z = 0.0f;
    VibrationAlgo_ClearRmsWindow();
    return s_last_rms;
  }

  if (s_gravity_initialized == 0U)
  {
    s_gravity_x = ax;
    s_gravity_y = ay;
    s_gravity_z = az;
    s_learning_start_tick = now_tick;
    s_gravity_initialized = 1U;
    s_state = VIBRATION_CALIBRATING;
  }

  if (s_state == VIBRATION_CALIBRATING)
  {
    s_gravity_x = (s_gravity_x * VIBRATION_GRAVITY_FAST_ALPHA) + (ax * VIBRATION_ACCEL_FAST_ALPHA);
    s_gravity_y = (s_gravity_y * VIBRATION_GRAVITY_FAST_ALPHA) + (ay * VIBRATION_ACCEL_FAST_ALPHA);
    s_gravity_z = (s_gravity_z * VIBRATION_GRAVITY_FAST_ALPHA) + (az * VIBRATION_ACCEL_FAST_ALPHA);
    s_vib_x = 0.0f;
    s_vib_y = 0.0f;
    s_vib_z = 0.0f;
    s_last_rms = 0.0f;
    VibrationAlgo_ClearRmsWindow();

    if ((now_tick - s_learning_start_tick) >= VIBRATION_LEARNING_TIME_MS)
    {
      s_state = VIBRATION_READY;
    }

    return s_last_rms;
  }

  s_gravity_x = (s_gravity_x * VIBRATION_GRAVITY_SLOW_ALPHA) + (ax * VIBRATION_ACCEL_SLOW_ALPHA);
  s_gravity_y = (s_gravity_y * VIBRATION_GRAVITY_SLOW_ALPHA) + (ay * VIBRATION_ACCEL_SLOW_ALPHA);
  s_gravity_z = (s_gravity_z * VIBRATION_GRAVITY_SLOW_ALPHA) + (az * VIBRATION_ACCEL_SLOW_ALPHA);

  s_vib_x = ax - s_gravity_x;
  s_vib_y = ay - s_gravity_y;
  s_vib_z = az - s_gravity_z;

  vib_mag = sqrtf((s_vib_x * s_vib_x) + (s_vib_y * s_vib_y) + (s_vib_z * s_vib_z));
  vib_mag_sq = vib_mag * vib_mag;

  s_vib_mag_sq_sum -= s_vib_mag_sq_buffer[s_rms_index];
  s_vib_mag_sq_buffer[s_rms_index] = vib_mag_sq;
  s_vib_mag_sq_sum += vib_mag_sq;

  s_rms_index++;
  if (s_rms_index >= VIBRATION_RMS_WINDOW_SIZE)
  {
    s_rms_index = 0U;
  }

  if (s_rms_count < VIBRATION_RMS_WINDOW_SIZE)
  {
    s_rms_count++;
  }

  mean_sq = s_vib_mag_sq_sum / (float)s_rms_count;
  s_last_rms = sqrtf(mean_sq);

  return s_last_rms;
}

VibrationAlgo_State_t VibrationAlgo_GetState(void)
{
  return s_state;
}

float VibrationAlgo_GetVibX(void)
{
  return s_vib_x;
}

float VibrationAlgo_GetVibY(void)
{
  return s_vib_y;
}

float VibrationAlgo_GetVibZ(void)
{
  return s_vib_z;
}
