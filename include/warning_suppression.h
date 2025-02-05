#ifndef WARNING_SUPPRESSION_H
#define WARNING_SUPPRESSION_H

// Compiler-specific warning suppression
#ifdef _MSC_VER
    // Start of external code warning suppression
    #define BEGIN_EXTERNAL_WARNINGS \
        __pragma(warning(push)) \
        __pragma(warning(disable:4018)) /* Disable signed/unsigned mismatch warnings */ \
        __pragma(warning(disable:4090)) /* Disable const qualifier warnings */ \
        __pragma(warning(disable:4244)) /* Disable conversion warnings */ \
        __pragma(warning(disable:4267)) /* Disable size_t to int conversion warnings */

    // QOA-specific warning suppression
    #define BEGIN_QOA_SECTION \
        __pragma(warning(push)) \
        __pragma(warning(disable:4018)) /* Disable signed/unsigned mismatch warnings */ \
        __pragma(warning(disable:4244)) /* Disable conversion warnings */ \
        __pragma(warning(disable:4267)) /* Disable size_t to int conversion warnings */

    #define END_QOA_SECTION __pragma(warning(pop))
    #define END_EXTERNAL_WARNINGS __pragma(warning(pop))
#else
    #define BEGIN_EXTERNAL_WARNINGS
    #define END_EXTERNAL_WARNINGS
    #define BEGIN_QOA_SECTION
    #define END_QOA_SECTION
#endif

#endif // WARNING_SUPPRESSION_H 