#include "../include/Logger.h"


Logger::Logger(const std::string& logFilePath) : currentLogLevel(LogLevel::Info), consoleOutputEnabled(true)
{
    // Ensure the directory exists
    std::filesystem::path logPath(logFilePath);
    auto directory = logPath.parent_path();
    if (!directory.empty() && !std::filesystem::exists(directory))
    {
        try
        {
            std::filesystem::create_directories(directory);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Failed to create directories for log file: " << e.what() << std::endl;
        }
    }

    // Wipe the file if it exists
    {
        std::ofstream clearFile(logFilePath, std::ios::out);
        if (!clearFile.is_open())
        {
            std::cerr << "Failed to create or open log file for wiping: " << logFilePath << std::endl;
            consoleOutputEnabled = true; // Fallback to console logging
            return;
        }
    }

    // Open the file in append mode for logging
    logFile.open(logFilePath, std::ios::out | std::ios::app);
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file for logging: " << logFilePath << std::endl;
        consoleOutputEnabled = true; // Fallback to console logging
    }
}



Logger::~Logger()
{
    if (logFile.is_open())
    {
        logFile.close();
    }
}

void Logger::Log(LogLevel level, const std::string& message)
{
    if (level < currentLogLevel)
        return;

    std::lock_guard<std::mutex> lock(logMutex);

    std::ostringstream logEntry;
    logEntry << "[" << GetTimestamp() << "][" << LogLevelToString(level) << "]::        " << message << "\n";

    if (logFile.is_open())
    {
        logFile << logEntry.str();
        logFile.flush();
    }

    if (consoleOutputEnabled)
    {
        std::cout << logEntry.str();
    }
}

void Logger::SetLogLevel(LogLevel level)
{
    currentLogLevel = level;
}

void Logger::EnableConsoleOutput(bool enable)
{
    consoleOutputEnabled = enable;
}


void Logger::Warn(const std::string& message)
{
    Log(LogLevel::Warning, message);
}
void Logger::Inform(const std::string& message)
{
    Log(LogLevel::Info, message);
}
void Logger::Error(const std::string& message)
{
    Log(LogLevel::Error, message);
}

std::string Logger::GetTimestamp() const
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime(&time_t_now), "%H:%M:%S") << "." << std::setfill('0') << std::setw(3) << ms.count();
    return timestamp.str();
}

std::string Logger::LogLevelToString(LogLevel level) const
{
    switch (level)
    {
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARNING";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Critical:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}
