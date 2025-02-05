#ifndef TEST_SUITES_H
#define TEST_SUITES_H

#include <math.h>

// Test suite declarations
int run_resource_manager_tests(void);
int run_core_system_tests(void);
int run_world_tests(void);
int run_map_tests(void);
int run_memory_tests(void);
int run_integration_tests(void);
int run_texture_manager_tests(void);

// Test utilities
void setup_test_environment(void);
void teardown_test_environment(void);

// Test result macros
#define TEST_PASSED 0
#define TEST_FAILED 1

// Float comparison epsilon
#define TEST_FLOAT_EPSILON 0.0001f

// Test assertion macros
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("Test failed at %s:%d: %s\n", __FILE__, __LINE__, #condition); \
            return TEST_FAILED; \
        } \
    } while (0)

#define TEST_TRUE(condition) \
    do { \
        if (!(condition)) { \
            printf("Test failed at %s:%d: Expected true, got false\n", __FILE__, __LINE__); \
            return TEST_FAILED; \
        } \
    } while (0)

#define TEST_FALSE(condition) \
    do { \
        if (condition) { \
            printf("Test failed at %s:%d: Expected false, got true\n", __FILE__, __LINE__); \
            return TEST_FAILED; \
        } \
    } while (0)

#define TEST_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            printf("Test failed at %s:%d: Expected NULL\n", __FILE__, __LINE__); \
            return TEST_FAILED; \
        } \
    } while (0)

#define TEST_EQUAL(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            printf("Test failed at %s:%d: Expected %d, got %d\n", \
                __FILE__, __LINE__, (int)(expected), (int)(actual)); \
            return TEST_FAILED; \
        } \
    } while (0)

#define TEST_FLOAT_EQUAL(actual, expected) \
    do { \
        if (fabsf((float)(actual) - (float)(expected)) > TEST_FLOAT_EPSILON) { \
            printf("Test failed at %s:%d: Expected %f, got %f\n", \
                __FILE__, __LINE__, (float)(expected), (float)(actual)); \
            return TEST_FAILED; \
        } \
    } while (0)

#define TEST_EQUAL_ENUM(actual, expected) \
    do { \
        if ((int)(actual) != (int)(expected)) { \
            printf("Test failed at %s:%d: Expected enum value %d, got %d\n", \
                __FILE__, __LINE__, (int)(expected), (int)(actual)); \
            return TEST_FAILED; \
        } \
    } while (0)

#define TEST_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            printf("Test failed at %s:%d: Pointer is NULL\n", __FILE__, __LINE__); \
            return TEST_FAILED; \
        } \
    } while (0)

// Test suite runner macro
#define RUN_TEST_SUITE(suite) \
    do { \
        printf("Running test suite: %s\n", #suite); \
        int suite_result = suite(); \
        if (suite_result == TEST_PASSED) { \
            printf("Test suite %s PASSED\n\n", #suite); \
        } else { \
            printf("Test suite %s FAILED\n\n", #suite); \
            failures += suite_result; \
        } \
    } while (0)

#endif // TEST_SUITES_H 