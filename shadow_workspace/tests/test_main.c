#include <stdio.h>
#include "include/test_suites.h"

int main(void) {
    int failures = 0;
    
    printf("=================================\n");
    printf("   Shadow Worker Test Suite\n");
    printf("=================================\n");
    
    setup_test_environment();
    
    // Temporarily skip resource manager tests
    // RUN_TEST_SUITE(run_resource_manager_tests);
    RUN_TEST_SUITE(run_core_system_tests);
    RUN_TEST_SUITE(run_world_tests);
    RUN_TEST_SUITE(run_map_tests);
    RUN_TEST_SUITE(run_memory_tests);
    RUN_TEST_SUITE(run_integration_tests);
    RUN_TEST_SUITE(run_texture_manager_tests);
    
    teardown_test_environment();
    
    printf("\n=================================\n");
    printf("Test Summary:\n");
    printf("Total failures: %d\n", failures);
    printf("=================================\n");
    
    return failures ? 1 : 0;
} 