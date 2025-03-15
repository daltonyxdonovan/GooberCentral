#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <ctime>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

class Logger {
public:
    Logger(const std::string& logFilePath);
    ~Logger();

    void Log(LogLevel level, const std::string& message);

    void SetLogLevel(LogLevel level);
    void EnableConsoleOutput(bool enable);
    void Warn(const std::string& message);
    void Inform(const std::string& message);
    void Error(const std::string& message);

private:
    std::ofstream logFile;
    std::mutex logMutex;
    LogLevel currentLogLevel;
    bool consoleOutputEnabled;

    std::string GetTimestamp() const;
    std::string LogLevelToString(LogLevel level) const;
};

#endif // LOGGER_H
