#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#include <string>
#include "Globals.h"

class CrashHandler
{
  public:
    static void Initialize();
    static void SignalHandler(int signal);

  private:
    static void WriteCrashReport(const std::string& message);
    static std::string GenerateStackTrace();
};

#endif // CRASH_HANDLER_H
