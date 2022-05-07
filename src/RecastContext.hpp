#pragma once

#include <Recast.h>

class RecastContext : public rcContext
{
 public:
  void doLog(const rcLogCategory, const char* msg, const int len) override;
  void doStartTimer(const rcTimerLabel label) override;
  void doStopTimer(const rcTimerLabel label) override;
};
