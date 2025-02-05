#include "../include/logger.h"
#include <stdio.h>
#include <stdarg.h>

static LogLevel g_logLevel = LOG_LEVEL_INFO;

void InitLogger(void) {
    g_logLevel = LOG_LEVEL_INFO;
}

void SetLogLevel(LogLevel level) {
    g_logLevel = level;
}

void Logger_Log(LogLevel level, LogCategory category, const char* file, int line, const char* format, ...) {
    if (level < g_logLevel) return;
    
    // Print log level and category
    const char* levelStr = "UNKNOWN";
    const char* categoryStr = "CORE";
    switch (level) {
        case LOG_LEVEL_TRACE: levelStr = "TRACE"; break;
        case LOG_LEVEL_DEBUG: levelStr = "DEBUG"; break;
        case LOG_LEVEL_INFO: levelStr = "INFO"; break;
        case LOG_LEVEL_WARNING: levelStr = "WARNING"; break;
        case LOG_LEVEL_ERROR: levelStr = "ERROR"; break;
        case LOG_LEVEL_FATAL: levelStr = "FATAL"; break;
    }
    
    switch (category) {
        case LOG_CORE: categoryStr = "CORE"; break;
        case LOG_AUDIO: categoryStr = "AUDIO"; break;
        case LOG_RESOURCE: categoryStr = "RESOURCE"; break;
        case LOG_ENTITY: categoryStr = "ENTITY"; break;
        case LOG_WORLD: categoryStr = "WORLD"; break;
        default: break;
    }
    
    printf("[%s][%s][%s:%d] ", levelStr, categoryStr, file, line);
    
    // Print the actual message
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

void CloseLogger(void) {
    // Nothing to clean up in test logger
} 