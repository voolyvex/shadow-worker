#ifndef TEST_SUITES_H
#define TEST_SUITES_H

// Test suite declarations
void run_core_system_tests(void);
void run_world_tests(void);
void run_map_tests(void);
void run_memory_tests(void);

// Test utilities
void setup_test_environment(void);
void teardown_test_environment(void);

#endif // TEST_SUITES_H 