#include "../../include/logger.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#endif

// Logger state
static Logger logger = {
    .log_file = NULL,
    .min_level = LOG_LEVEL_INFO,
    .file_output = true,
    .console_output = true,
    .timer_count = 0
};

// Level to string conversion
static const char* level_strings[] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "FATAL"
};

// Category to string conversion
static const char* category_strings[] = {
    "CORE",
    "AUDIO",
    "RESOURCE",
    "ENTITY",
    "WORLD"
};

// ANSI color codes for console output
static const char* level_colors[] = {
    "\x1b[94m", // TRACE: Light blue
    "\x1b[36m", // DEBUG: Cyan
    "\x1b[32m", // INFO: Green
    "\x1b[33m", // WARN: Yellow
    "\x1b[31m", // ERROR: Red
    "\x1b[35m", // FATAL: Magenta
};

bool Logger_Init(const char* filename) {
    logger.log_file = fopen(filename ? filename : "game.log", "w");
    if (!logger.log_file) {
        fprintf(stderr, "Failed to open log file\n");
        return false;
    }
    LOG_INFO(LOG_CORE, "Logging system initialized");
    return true;
}

void Logger_Shutdown(void) {
    if (logger.log_file) {
        LOG_INFO(LOG_CORE, "Logging system shutdown");
        fclose(logger.log_file);
        logger.log_file = NULL;
    }
}

void Logger_SetMinLevel(LogLevel level) {
    logger.min_level = level;
}

void Logger_EnableFileOutput(bool enable) {
    logger.file_output = enable;
}

void Logger_EnableConsoleOutput(bool enable) {
    logger.console_output = enable;
}

void Logger_Log(LogLevel level, LogCategory category, const char* file, int line, const char* format, ...) {
    if (level < logger.min_level) return;

    char message[MAX_LOG_MESSAGE];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    time_t now;
    time(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    char final_message[MAX_LOG_MESSAGE];
    snprintf(final_message, sizeof(final_message), "[%s] [%s] [%s] %s:%d: %s\n",
             timestamp, level_strings[level], category_strings[category],
             file, line, message);

    if (logger.console_output) {
        printf("%s%s\x1b[0m", level_colors[level], final_message);
    }

    if (logger.file_output && logger.log_file) {
        fprintf(logger.log_file, "%s", final_message);
        fflush(logger.log_file);
    }
}

void Logger_BeginTimer(const char* name) {
    if (!logger.file_output && !logger.console_output) {
        return;
    }
    
    // Find existing timer or create new one
    for (int i = 0; i < logger.timer_count; i++) {
        if (strcmp(logger.active_timers[i].name, name) == 0) {
            logger.active_timers[i].start_time = clock() / (double)CLOCKS_PER_SEC;
            return;
        }
    }
    
    // Create new timer if we have space
    if (logger.timer_count < MAX_ACTIVE_TIMERS) {
        strncpy(logger.active_timers[logger.timer_count].name, name, MAX_TIMER_NAME - 1);
        logger.active_timers[logger.timer_count].name[MAX_TIMER_NAME - 1] = '\0';
        logger.active_timers[logger.timer_count].start_time = clock() / (double)CLOCKS_PER_SEC;
        logger.timer_count++;
    }
}

double Logger_EndTimer(const char* name) {
    if (!logger.file_output && !logger.console_output) {
        return 0.0;
    }
    
    // Find the timer
    for (int i = 0; i < logger.timer_count; i++) {
        if (strcmp(logger.active_timers[i].name, name) == 0) {
            double end_time = clock() / (double)CLOCKS_PER_SEC;
            double elapsed = end_time - logger.active_timers[i].start_time;
            logger.active_timers[i].elapsed_time = elapsed;
            
            // Log the timing
            LOG_DEBUG(LOG_CORE, "Timer '%s' completed in %.3f seconds", name, elapsed);
            
            // Remove the timer by shifting remaining ones down
            if (i < logger.timer_count - 1) {
                memmove(&logger.active_timers[i], &logger.active_timers[i + 1], 
                       (logger.timer_count - i - 1) * sizeof(LogTimer));
            }
            logger.timer_count--;
            
            return elapsed;
        }
    }
    
    LOG_WARN(LOG_CORE, "Timer '%s' not found", name);
    return 0.0;
}

void Logger_LogMemoryUsage(void) {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        LOG_INFO(LOG_CORE, "Memory Usage - Working Set: %.2f MB, Private Usage: %.2f MB",
                pmc.WorkingSetSize / (1024.0 * 1024.0),
                pmc.PrivateUsage / (1024.0 * 1024.0));
    }
#endif
} 