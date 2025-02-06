#ifndef TEST_SUITES_H
#define TEST_SUITES_H

#include <stdbool.h>

// Test environment setup/teardown
bool SetupTestEnvironment(void);
void TearDownTestEnvironment(void);

// Core system tests
bool RunCoreSystemTests(void);
bool RunMemoryTests(void);
bool RunLoggerTests(void);

// World and map tests
bool RunWorldTests(void);
bool RunMapTests(void);
bool RunMapGenerationTests(void);

// Resource management tests
bool RunResourceManagerTests(void);
bool RunTextureManagerTests(void);

// Integration tests
bool RunIntegrationTests(void);

// Test utilities
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("Assertion failed: %s\nFile: %s\nLine: %d\n", #condition, __FILE__, __LINE__); \
            return false; \
        } \
    } while (0)

#define ASSERT_MSG(condition, message) \
    do { \
        if (!(condition)) { \
            printf("Assertion failed: %s\nMessage: %s\nFile: %s\nLine: %d\n", \
                   #condition, message, __FILE__, __LINE__); \
            return false; \
        } \
    } while (0)

#define TEST_RESOURCES_DIR "test_resources"

// Test suite registration
typedef struct {
    const char* name;
    bool (*runFn)(void);
} TestSuite;

static const TestSuite TEST_SUITES[] = {
    {"core", RunCoreSystemTests},
    {"memory", RunMemoryTests},
    {"logger", RunLoggerTests},
    {"world", RunWorldTests},
    {"map", RunMapTests},
    {"mapgen", RunMapGenerationTests},
    {"resource", RunResourceManagerTests},
    {"texture", RunTextureManagerTests},
    {"integration", RunIntegrationTests},
    {NULL, NULL}  // Sentinel
};

#endif // TEST_SUITES_H 