#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define MAX_LOG_MESSAGE 1024
#define MAX_TIMER_NAME 64
#define MAX_ACTIVE_TIMERS 32

// Log levels
typedef enum {
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
} LogLevel;

// Log categories
typedef enum {
    LOG_CORE,
    LOG_AUDIO,
    LOG_RESOURCE,
    LOG_ENTITY,
    LOG_WORLD,
    LOG_CATEGORY_COUNT
} LogCategory;

// Timer structure
typedef struct {
    char name[MAX_TIMER_NAME];
    double start_time;
    double elapsed_time;
} LogTimer;

// Logger structure
typedef struct {
    FILE* log_file;
    LogLevel min_level;
    bool file_output;
    bool console_output;
    LogTimer active_timers[MAX_ACTIVE_TIMERS];
    int timer_count;
} Logger;

// Logging macros
#define LOG_TRACE(category, ...) Logger_Log(LOG_LEVEL_TRACE, category, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(category, ...) Logger_Log(LOG_LEVEL_DEBUG, category, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(category, ...) Logger_Log(LOG_LEVEL_INFO, category, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(category, ...) Logger_Log(LOG_LEVEL_WARNING, category, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(category, ...) Logger_Log(LOG_LEVEL_WARNING, category, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(category, ...) Logger_Log(LOG_LEVEL_ERROR, category, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(category, ...) Logger_Log(LOG_LEVEL_FATAL, category, __FILE__, __LINE__, __VA_ARGS__)

// Function declarations
bool Logger_Init(const char* filename);
void Logger_Shutdown(void);
void Logger_SetMinLevel(LogLevel level);
void Logger_EnableFileOutput(bool enable);
void Logger_EnableConsoleOutput(bool enable);
void Logger_Log(LogLevel level, LogCategory category, const char* file, int line, const char* format, ...);
void Logger_BeginTimer(const char* name);
double Logger_EndTimer(const char* name);
void Logger_LogMemoryUsage(void);

#endif // LOGGER_H 