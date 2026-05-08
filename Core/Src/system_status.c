#include "system_status.h"

#define SYSTEM_STATUS_WARNING_ENTER_RMS  0.05f
#define SYSTEM_STATUS_WARNING_EXIT_RMS   0.04f
#define SYSTEM_STATUS_ALARM_ENTER_RMS    0.15f
#define SYSTEM_STATUS_ALARM_EXIT_RMS     0.12f

#define SYSTEM_STATUS_ENTER_CONFIRM_COUNT  3U
#define SYSTEM_STATUS_EXIT_CONFIRM_COUNT   5U

static SystemStatus_t s_current_status = SYSTEM_STATUS_NORMAL;
static SystemStatus_t s_pending_enter_status = SYSTEM_STATUS_NORMAL;
static uint8_t s_enter_count = 0U;
static uint8_t s_exit_count = 0U;

static void SystemStatus_ResetEnterCounter(void)
{
  s_pending_enter_status = SYSTEM_STATUS_NORMAL;
  s_enter_count = 0U;
}

static void SystemStatus_ResetExitCounter(void)
{
  s_exit_count = 0U;
}

static uint8_t SystemStatus_UpdateEnterCounter(SystemStatus_t target_status)
{
  if (s_pending_enter_status != target_status)
  {
    s_pending_enter_status = target_status;
    s_enter_count = 0U;
  }

  if (s_enter_count < SYSTEM_STATUS_ENTER_CONFIRM_COUNT)
  {
    s_enter_count++;
  }

  return (s_enter_count >= SYSTEM_STATUS_ENTER_CONFIRM_COUNT) ? 1U : 0U;
}

void SystemStatus_Init(void)
{
  s_current_status = SYSTEM_STATUS_NORMAL;
  SystemStatus_ResetEnterCounter();
  SystemStatus_ResetExitCounter();
}

SystemStatus_t SystemStatus_UpdateFromRms(float rms)
{
  switch (s_current_status)
  {
    case SYSTEM_STATUS_NORMAL:
      if (rms >= SYSTEM_STATUS_ALARM_ENTER_RMS)
      {
        if (SystemStatus_UpdateEnterCounter(SYSTEM_STATUS_ALARM) != 0U)
        {
          s_current_status = SYSTEM_STATUS_ALARM;
          SystemStatus_ResetEnterCounter();
        }
      }
      else if (rms >= SYSTEM_STATUS_WARNING_ENTER_RMS)
      {
        if (SystemStatus_UpdateEnterCounter(SYSTEM_STATUS_WARNING) != 0U)
        {
          s_current_status = SYSTEM_STATUS_WARNING;
          SystemStatus_ResetEnterCounter();
        }
      }
      else
      {
        SystemStatus_ResetEnterCounter();
      }
      SystemStatus_ResetExitCounter();
      break;

    case SYSTEM_STATUS_WARNING:
      if (rms >= SYSTEM_STATUS_ALARM_ENTER_RMS)
      {
        if (SystemStatus_UpdateEnterCounter(SYSTEM_STATUS_ALARM) != 0U)
        {
          s_current_status = SYSTEM_STATUS_ALARM;
          SystemStatus_ResetEnterCounter();
        }
        SystemStatus_ResetExitCounter();
      }
      else if (rms < SYSTEM_STATUS_WARNING_EXIT_RMS)
      {
        SystemStatus_ResetEnterCounter();
        if (s_exit_count < SYSTEM_STATUS_EXIT_CONFIRM_COUNT)
        {
          s_exit_count++;
        }
        if (s_exit_count >= SYSTEM_STATUS_EXIT_CONFIRM_COUNT)
        {
          s_current_status = SYSTEM_STATUS_NORMAL;
          SystemStatus_ResetExitCounter();
        }
      }
      else
      {
        SystemStatus_ResetEnterCounter();
        SystemStatus_ResetExitCounter();
      }
      break;

    case SYSTEM_STATUS_ALARM:
    default:
      SystemStatus_ResetEnterCounter();
      if (rms < SYSTEM_STATUS_ALARM_EXIT_RMS)
      {
        if (s_exit_count < SYSTEM_STATUS_EXIT_CONFIRM_COUNT)
        {
          s_exit_count++;
        }
        if (s_exit_count >= SYSTEM_STATUS_EXIT_CONFIRM_COUNT)
        {
          if (rms < SYSTEM_STATUS_WARNING_EXIT_RMS)
          {
            s_current_status = SYSTEM_STATUS_NORMAL;
          }
          else
          {
            s_current_status = SYSTEM_STATUS_WARNING;
          }
          SystemStatus_ResetExitCounter();
        }
      }
      else
      {
        SystemStatus_ResetExitCounter();
      }
      break;
  }

  return s_current_status;
}

SystemStatus_t SystemStatus_GetCurrent(void)
{
  return s_current_status;
}

const char *SystemStatus_ToString(SystemStatus_t status)
{
  switch (status)
  {
    case SYSTEM_STATUS_ALARM:
      return "ALARM";
    case SYSTEM_STATUS_WARNING:
      return "WARNING";
    case SYSTEM_STATUS_NORMAL:
    default:
      return "NORMAL";
  }
}

const char *SystemStatus_GetCurrentString(void)
{
  return SystemStatus_ToString(s_current_status);
}

uint8_t SystemStatus_IsAlarm(SystemStatus_t status)
{
  return (status == SYSTEM_STATUS_ALARM) ? 1U : 0U;
}

uint8_t SystemStatus_GetEnterCount(void)
{
  return s_enter_count;
}

uint8_t SystemStatus_GetExitCount(void)
{
  return s_exit_count;
}
