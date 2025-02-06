#include "utils/debug.h"
#include "utils/logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static LogLevel current_log_level = LOG_LEVEL_INFO;

void set_debug_level(LogLevel level) {
    current_log_level = level;
}

LogLevel get_debug_level(void) {
    return current_log_level;
}

void debug_log(LogLevel level, LogCategory category, const char* format, ...) {
    if (level > current_log_level) {
        return;
    }

    time_t now;
    time(&now);
    struct tm* local = localtime(&now);
    
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local);

    const char* level_str = GetLogLevelString(level);
    const char* category_str = GetLogCategoryString(category);

    printf("[%s] [%s] [%s] ", timestamp, level_str, category_str);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
} 