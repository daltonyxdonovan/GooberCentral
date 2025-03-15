#include "../include/CrashHandler.h"
#include "../include/Globals.h"

#include <csignal>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <execinfo.h>
#include <unistd.h>
#include <cxxabi.h>
#endif

void CrashHandler::Initialize()
{
    std::signal(SIGSEGV, SignalHandler);
    std::signal(SIGABRT, SignalHandler);
    logger.Inform("CrashHandler has been initialised\n");
}

void CrashHandler::SignalHandler(int signal)
{
    std::string signalName = (signal == SIGSEGV) ? "Segmentation Fault" : "Abort Signal";

    std::ostringstream crashMessage;
    crashMessage << "Oh no! Application crashed due to: " << signalName << "\n";
    crashMessage << "\n\n" << GenerateStackTrace();

    WriteCrashReport(crashMessage.str());

    logger.Log(LogLevel::Critical, crashMessage.str());
    std::cerr << crashMessage.str();

    std::_Exit(EXIT_FAILURE);
}

void CrashHandler::WriteCrashReport(const std::string& message)
{
    std::ofstream crashReport("RES/logs/crash_report.txt", std::ios::out | std::ios::trunc);
    if (crashReport.is_open())
    {
        crashReport << message;
        crashReport.close();
    }
    else
    {
        std::cerr << "Failed to write crash report to RES/logs/crash_report.txt\n";
    }
}

std::string CrashHandler::GenerateStackTrace()
{
    std::ostringstream stackTrace;
    stackTrace << "==== STACK TRACE ====\n";

#ifdef _WIN32
    const int maxFrames = 62;
    void* stack[maxFrames];
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, nullptr, TRUE);

    USHORT frames = CaptureStackBackTrace(0, maxFrames, stack, nullptr);

    for (USHORT i = 0; i < frames; ++i)
    {
        DWORD64 address = (DWORD64)(stack[i]);

        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)] = { 0 };
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)symbolBuffer;
        symbol->MaxNameLen = MAX_SYM_NAME;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

        DWORD displacement;
        IMAGEHLP_LINE64 lineInfo = { 0 };
        lineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        stackTrace << i + 1 << ". ";

        if (SymFromAddr(process, address, nullptr, symbol))
        {
            stackTrace << symbol->Name;

            if (SymGetLineFromAddr64(process, address, &displacement, &lineInfo))
            {
                stackTrace << "\n   File: " << lineInfo.FileName;
                stackTrace << "\n   Line: " << lineInfo.LineNumber;
            }
            stackTrace << "\n   Address: 0x" << std::hex << symbol->Address;
        }
        else
        {
            stackTrace << "Unknown Function";
            stackTrace << "\n   (No source information available)";
            stackTrace << "\n   Address: 0x" << std::hex << address;
        }

        stackTrace << "\n\n";
    }
#else
    const int maxFrames = 100;
    void* addrList[maxFrames];
    int addrLen = backtrace(addrList, maxFrames);

    if (addrLen == 0)
    {
        return "No stack trace available.\n";
    }

    char** symbolList = backtrace_symbols(addrList, addrLen);

    for (int i = 0; i < addrLen; ++i)
    {
        stackTrace << i + 1 << ". ";

        char* demangled = nullptr;
        char* leftParen = strchr(symbolList[i], '(');
        char* plus = strchr(symbolList[i], '+');

        if (leftParen && plus && leftParen < plus)
        {
            *plus = '\0';
            *leftParen = '\0';

            demangled = abi::__cxa_demangle(leftParen + 1, nullptr, nullptr, nullptr);
        }

        if (demangled)
        {
            stackTrace << demangled;
            free(demangled);
        }
        else
        {
            stackTrace << symbolList[i];
        }

        stackTrace << "\n\n";
    }

    free(symbolList);
#endif

    stackTrace << "==== END OF REPORT ====\n";
    return stackTrace.str();
}
