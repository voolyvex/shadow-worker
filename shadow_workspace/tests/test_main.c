#include <stdio.h>
#include "include/test_suites.h"
#include "../include/logger.h"

int main(void) {
    // Initialize logging
    if (!Logger_Init("test_log.txt")) {
        fprintf(stderr, "Failed to initialize logger\n");
        return 1;
    }

    LOG_INFO(LOG_CORE, "=== Starting Shadow Worker Test Suite ===");

    // Setup test environment
    setup_test_environment();

    // Run test suites
    run_core_system_tests();
    run_world_tests();
    run_map_tests();
    run_memory_tests();

    // Cleanup
    teardown_test_environment();

    LOG_INFO(LOG_CORE, "=== Test Suite Completed ===");
    Logger_Shutdown();

    return 0;
} 