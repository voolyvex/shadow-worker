#include <stdio.h>
#include <stdlib.h>
#include "include/test_suites.h"

int main(void) {
    int total_failures = 0;
    
    printf("=================================\n");
    printf("   Shadow Worker Test Suite\n");
    printf("=================================\n");
    
    // Set up test environment
    if (!setup_test_environment()) {
        fprintf(stderr, "Failed to set up test environment\n");
        return EXIT_FAILURE;
    }
    
    // Run test suites in a protected block
    do {
        // Core system tests
        total_failures += run_core_system_tests();
        if (total_failures > 0) {
            fprintf(stderr, "Core system tests failed, skipping remaining tests\n");
            break;
        }
        
        // Integration tests
        total_failures += run_integration_tests();
        if (total_failures > 0) {
            fprintf(stderr, "Integration tests failed, skipping remaining tests\n");
            break;
        }
        
        // Map tests
        total_failures += run_map_tests();
        if (total_failures > 0) {
            fprintf(stderr, "Map tests failed, skipping remaining tests\n");
            break;
        }
        
        // Memory tests
        total_failures += run_memory_tests();
        if (total_failures > 0) {
            fprintf(stderr, "Memory tests failed, skipping remaining tests\n");
            break;
        }
        
        // Texture manager tests
        total_failures += run_texture_manager_tests();
        if (total_failures > 0) {
            fprintf(stderr, "Texture manager tests failed, skipping remaining tests\n");
            break;
        }
        
        // World tests
        total_failures += run_world_tests();
        if (total_failures > 0) {
            fprintf(stderr, "World tests failed\n");
            break;
        }
    } while (0);
    
    // Clean up test environment
    teardown_test_environment();
    
    // Report results
    printf("\n=================================\n");
    if (total_failures == 0) {
        printf("All tests passed successfully!\n");
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Tests failed with %d errors\n", total_failures);
        return EXIT_FAILURE;
    }
} 