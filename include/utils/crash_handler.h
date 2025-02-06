#ifndef SHADOW_WORKER_CRASH_HANDLER_H
#define SHADOW_WORKER_CRASH_HANDLER_H

#include "utils/warning_suppression.h"

BEGIN_EXTERNAL_WARNINGS
#include <signal.h>
#include <stdbool.h>
END_EXTERNAL_WARNINGS

// Initialize crash handler
void setupCrashHandler(void);

// Register a callback to be called when a crash occurs
typedef void (*CrashCallback)(void);
void registerCrashCallback(CrashCallback callback);

// Write crash dump to file
void writeCrashDump(const char* filename);

// Get crash dump directory
const char* getCrashDumpDirectory(void);

// Check if a crash occurred in the last session
bool checkForPreviousCrash(void);

// Clear crash flag
void clearCrashFlag(void);

#endif // SHADOW_WORKER_CRASH_HANDLER_H 