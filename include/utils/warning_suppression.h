#ifndef SHADOW_WORKER_WARNING_SUPPRESSION_H
#define SHADOW_WORKER_WARNING_SUPPRESSION_H

#ifdef _MSC_VER
    // MSVC-specific warning suppressions
    #define BEGIN_EXTERNAL_WARNINGS __pragma(warning(push))
    #define END_EXTERNAL_WARNINGS __pragma(warning(pop))
    
    #pragma warning(disable: 4100) // unreferenced formal parameter
    #pragma warning(disable: 4101) // unreferenced local variable
    #pragma warning(disable: 4244) // conversion possible loss of data
    #pragma warning(disable: 4267) // conversion from size_t
    #pragma warning(disable: 4996) // deprecated functions
    #pragma warning(disable: 4706) // assignment within conditional
    #pragma warning(disable: 4127) // conditional expression is constant
#elif defined(__GNUC__)
    // GCC/Clang warning suppressions
    #define BEGIN_EXTERNAL_WARNINGS _Pragma("GCC diagnostic push")
    #define END_EXTERNAL_WARNINGS _Pragma("GCC diagnostic pop")
    
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wconversion"
    #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

#endif // SHADOW_WORKER_WARNING_SUPPRESSION_H 