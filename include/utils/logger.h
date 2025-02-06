#ifndef SHADOW_WORKER_LOGGER_H
#define SHADOW_WORKER_LOGGER_H

#include <stdio.h>

// Log levels
typedef enum {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} LogLevel;

// Log categories
typedef enum {
    LOG_CORE,
    LOG_GRAPHICS,
    LOG_AUDIO,
    LOG_INPUT,
    LOG_PHYSICS,
    LOG_AI,
    LOG_NETWORK
} LogCategory;

// Logging macros
#define LOG_ERROR(category, format, ...) LogMessage(LOG_LEVEL_ERROR, category, format, ##__VA_ARGS__)
#define LOG_WARN(category, format, ...) LogMessage(LOG_LEVEL_WARN, category, format, ##__VA_ARGS__)
#define LOG_INFO(category, format, ...) LogMessage(LOG_LEVEL_INFO, category, format, ##__VA_ARGS__)
#define LOG_DEBUG(category, format, ...) LogMessage(LOG_LEVEL_DEBUG, category, format, ##__VA_ARGS__)
#define LOG_TRACE(category, format, ...) LogMessage(LOG_LEVEL_TRACE, category, format, ##__VA_ARGS__)

// Function declarations
void InitLogger(void);
void ShutdownLogger(void);
void SetLogLevel(LogLevel level);
void LogMessage(LogLevel level, LogCategory category, const char* format, ...);
const char* GetLogLevelString(LogLevel level);
const char* GetLogCategoryString(LogCategory category);

#endif // SHADOW_WORKER_LOGGER_H 