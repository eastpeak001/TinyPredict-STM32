#ifndef VIBRATION_ALGO_H
#define VIBRATION_ALGO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  VIBRATION_CALIBRATING = 0,
  VIBRATION_READY = 1
} VibrationAlgo_State_t;

void VibrationAlgo_Init(void);
float VibrationAlgo_Update(void);
VibrationAlgo_State_t VibrationAlgo_GetState(void);
float VibrationAlgo_GetVibX(void);
float VibrationAlgo_GetVibY(void);
float VibrationAlgo_GetVibZ(void);

#ifdef __cplusplus
}
#endif

#endif /* VIBRATION_ALGO_H */
