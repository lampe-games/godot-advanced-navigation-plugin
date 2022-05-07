#include "RecastContext.hpp"

#include <Godot.hpp>

void RecastContext::doLog(const rcLogCategory category, const char* msg, const int)
{
  switch (category)
  {
    case RC_LOG_PROGRESS:
      // ignore
      break;
    case RC_LOG_WARNING:
      WARN_PRINT(msg);
      break;
    case RC_LOG_ERROR:
      ERR_PRINT(msg);
      break;
  }
}

void RecastContext::doStartTimer(const rcTimerLabel label)
{
  // TODO: implement
}

void RecastContext::doStopTimer(const rcTimerLabel label)
{
  // TODO: implement
}
