#ifndef ALARM_TASK_H
#define ALARM_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

void AlarmTask_Init(void);
void AlarmTask_Update(float rms_value);

#ifdef __cplusplus
}
#endif

#endif /* ALARM_TASK_H */
