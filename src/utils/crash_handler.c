#include "utils/warning_suppression.h"

// Include external headers first
BEGIN_EXTERNAL_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <sys/stat.h>
#include <unistd.h>
#include <execinfo.h>
#endif
END_EXTERNAL_WARNINGS

// Include internal headers
#include "utils/crash_handler.h"
#include "utils/logger.h"
#include "utils/debug.h"

// Static variables
static CrashCallback crashCallback = NULL;
static char crashDumpDir[256] = "crash_dumps";
static bool crashOccurred = false;

// Signal handler function
static void signalHandler(int signal) {
    LOG_ERROR(LOG_CORE, "Received signal %d", signal);
    
    // Call registered callback if any
    if (crashCallback) {
        crashCallback();
    }
    
    // Generate timestamp for crash dump filename
    time_t now;
    time(&now);
    struct tm* timeinfo = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", timeinfo);
    
    // Create crash dump filename
    char filename[512];
    snprintf(filename, sizeof(filename), "%s/crash_%s.dmp", crashDumpDir, timestamp);
    
    // Write crash dump
    writeCrashDump(filename);
    
    // Set crash flag
    crashOccurred = true;
    
    // Exit with error code
    exit(signal);
}

void setupCrashHandler(void) {
    // Create crash dump directory if it doesn't exist
#ifdef _WIN32
    CreateDirectoryA(crashDumpDir, NULL);
#else
    mkdir(crashDumpDir, 0777);
#endif
    
    // Register signal handlers
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGILL, signalHandler);
    
    LOG_INFO(LOG_CORE, "Crash handler initialized");
}

void registerCrashCallback(CrashCallback callback) {
    crashCallback = callback;
}

void writeCrashDump(const char* filename) {
#ifdef _WIN32
    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        LOG_ERROR(LOG_CORE, "Failed to create crash dump file");
        return;
    }
    
    HANDLE hProcess = GetCurrentProcess();
    DWORD processId = GetCurrentProcessId();
    
    MINIDUMP_EXCEPTION_INFORMATION exInfo;
    exInfo.ThreadId = GetCurrentThreadId();
    exInfo.ExceptionPointers = NULL;
    exInfo.ClientPointers = FALSE;
    
    BOOL result = MiniDumpWriteDump(hProcess, processId, hFile,
                                  MiniDumpNormal, &exInfo, NULL, NULL);
    
    if (!result) {
        LOG_ERROR(LOG_CORE, "Failed to write crash dump");
    } else {
        LOG_INFO(LOG_CORE, "Crash dump written to %s", filename);
    }
    
    CloseHandle(hFile);
#else
    // For non-Windows platforms, write a basic crash log
    FILE* file = fopen(filename, "w");
    if (file) {
        time_t now;
        time(&now);
        fprintf(file, "Crash occurred at: %s\n", ctime(&now));
        
        // Get backtrace
        void* callstack[128];
        int frames = backtrace(callstack, 128);
        char** strs = backtrace_symbols(callstack, frames);
        
        if (strs) {
            fprintf(file, "\nBacktrace:\n");
            for (int i = 0; i < frames; i++) {
                fprintf(file, "%s\n", strs[i]);
            }
            free(strs);
        }
        
        fclose(file);
        LOG_INFO(LOG_CORE, "Basic crash log written to %s", filename);
    } else {
        LOG_ERROR(LOG_CORE, "Failed to create crash log file");
    }
#endif
}

const char* getCrashDumpDirectory(void) {
    return crashDumpDir;
}

bool checkForPreviousCrash(void) {
    return crashOccurred;
}

void clearCrashFlag(void) {
    crashOccurred = false;
} 