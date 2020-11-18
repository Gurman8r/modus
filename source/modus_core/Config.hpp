#ifndef _ML_CONFIG_HPP_
#define _ML_CONFIG_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// PROJECT
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define ML__author              "Melody Gurman"
#define ML__name                "modus"
#define ML__version             "alpha"
#define ML__url                 "https://www.github.com/Gurman8r/modus"
#define ML__date                __DATE__
#define ML__time                __TIME__


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// CONFIGURATION
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(_DEBUG)
//                              Debug
#   define ML_is_debug          1
#   define ML_configuration     "debug"
#else
//                              Release
#   define ML_is_debug          0
#   define ML_configuration     "release"
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// LANGUAGE
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(__cplusplus)
#   if defined(_MSVC_LANG)
#       define ML_lang           _MSVC_LANG
#   else
#       define ML_lang           __cplusplus
#   endif

#   if (ML_lang >= 201907L)
//                              C++20
#       define ML_has_cxx20     1
#       define ML_has_cxx17     1
#       define ML_has_cxx14     1
#       define ML_has_cxx11     1

#   elif (ML_lang >= 201703L)
//                              C++17
#       define ML_has_cxx20     0
#       define ML_has_cxx17     1
#       define ML_has_cxx14     1
#       define ML_has_cxx11     1

#   elif (ML_lang >= 201402L)
//                              C++14
#       define ML_has_cxx20     0
#       define ML_has_cxx17     0
#       define ML_has_cxx14     1
#       define ML_has_cxx11     1

#   elif (ML_lang >= 201103L)
//                              C++11
#       define ML_has_cxx20     0
#       define ML_has_cxx17     0
#       define ML_has_cxx14     0
#       define ML_has_cxx11     1

#   else
#       error "this version of C++ is not supported"
#   endif

#else
#   error "this system does not support C++"
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// OPERATING SYSTEM
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
//                              Windows
#   define ML_os_windows        1
#   define ML_os_name           "Windows"

#elif defined(__APPLE__) && defined(__MACH__)
//                              Apple
#   define ML_os_apple          1
#   define ML_os_name           "Apple"

#elif defined(__unix__)
//                              Unix
#   define ML_os_unix           1

#   if defined(__ANDROID__)
//                              Android
#       define ML_os_android    1
#       define ML_os_name       "Android"

#   elif defined(__linux__)
//                              Linux
#       define ML_os_linux      1
#       define ML_os_name       "Linux"

#   elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
//                              FreeBSD
#       define ML_os_freebsd    1
#       define ML_os_name       "FreeBSD"

#   else
#       error "this unix operating system is not supported"
#   endif

#else
#   error "this operating system is not supported"
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// ARCHITECTURE
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(__x86_64__) || defined(_M_X64) || defined(_x64)
//                              x64
#   define ML_x64               1
#   define ML_arch              64
#   define ML_platform          "x64"

#elif defined(__i386__) || defined(_M_IX86)
//                              x86
#   define ML_x86               1
#   define ML_arch              32
#   define ML_platform          "x86"

#elif defined(__arm__) || defined(_M_ARM) || defined(__aarch64__)
//                              ARM
#   if defined(__aarch64__)
#       define ML_arm64         1
#       define ML_arch          64
#       define ML_platform      "arm64"
#   else
#       define ML_arm32         1
#       define ML_arch          32
#       define ML_platform      "arm32"
#   endif

#elif defined(ppc) || defined(_M_PPC) || defined(__ppc64__)
//                              PowerPC
#   if defined(__ppc64__)
#       define ML_ppc64         1
#       define ML_arch          64
#       define ML_platform      "ppc64"
#   else
#       define ML_ppc32         1
#       define ML_arch          32
#       define ML_platform      "ppc32"
#   endif

#else
#   error "unable to detect platform architecture"
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// COMPILER
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if defined(_MSC_VER)
//                              Visual Studio
#   define ML_cc_msvc           _MSC_VER
#   define ML_cc_version        ML_cc_msvc
#   if (ML_cc_version >= 1920)
#       define ML_cc_name       "Visual Studio 2019"
#   elif (ML_cc_version >= 1910)
#       define ML_cc_name       "Visual Studio 2017"
#   elif (ML_cc_version >= 1900)
#       define ML_cc_name       "Visual Studio 2015"
#   elif (ML_cc_version >= 1800)
#       define ML_cc_name       "Visual Studio 2013"
#   else
#       error "this version of Visual Studio is not supported"
#   endif

#elif defined(__clang__)
//                              Clang / LLVM
#   define ML_cc_clang          __clang__
#   define ML_cc_version        ML_cc_clang
#   define ML_cc_name           "Clang/LLVM"

#elif (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
//                              GCC
#   if defined(__GNUC__)
#       define ML_cc_gcc        __GNUC__
#   else
#       define ML_cc_gcc        __GNUG__
#   endif
#   define ML_cc_version        ML_cc_gcc
#   define ML_cc_name           "GCC"

#elif defined(__ICC) || defined(__INTEL_COMPILER)
//                              Intel
#   if defined(__ICC)
#       define ML_cc_intel      __ICC
#   else
#       define ML_cc_intel      __INTEL_COMPILER
#   endif
#   define ML_cc_version        ML_cc_intel
#   define ML_cc_name           "Intel"

#elif defined(__MINGW32__) || defined(__MINGW64__)
//                              MinGW
#   if defined(__MINGW64__)
#       define ML_cc_mingw      __MINGW64__
#   else
#       define ML_cc_mingw      __MINGW32__
#   endif
#   define ML_cc_version        ML_cc_mingw
#   define ML_cc_name           "MinGW"

#elif defined(__EMSCRIPTEN__)
//                              Emscripten
#   define ML_cc_emscripten     __EMSCRIPTEN__
#   define ML_cc_version        ML_cc_emscripten
#   define ML_cc_name           "Emscripten"

#elif defined(__asmjs__)
//                              asm.js
#   define ML_cc_asmjs          __asmjs__
#   define ML_cc_version        ML_cc_asmjs
#   define ML_cc_name           "asm.js"

#elif defined(__wasm__)
//                              WebAssembly
#   define ML_cc_wasm           __wasm__
#   define ML_cc_version        ML_cc_wasm
#   define ML_cc_name           "WebAssembly"

#else
#   error "this compiler is not supported"
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// TYPES
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// byte
#ifndef ML_byte
#define ML_byte                 unsigned char
#endif

// integers
#if defined(ML_cc_msvc)
#   define  ML_int8             signed __int8
#   define  ML_int16            signed __int16
#   define  ML_int32            signed __int32
#   define  ML_int64            signed __int64
#   define  ML_uint8            unsigned __int8
#   define  ML_uint16           unsigned __int16
#   define  ML_uint32           unsigned __int32
#   define  ML_uint64           unsigned __int64
#else
#   define  ML_int8             signed char
#   define  ML_int16            signed short
#   define  ML_int32            signed int
#   define  ML_int64            signed long long
#   define  ML_uint8            unsigned char
#   define  ML_uint16           unsigned short
#   define  ML_uint32           unsigned int
#   define  ML_uint64           unsigned long long
#endif

// floats
#define ML_float32              float
#define ML_float64              double
#define ML_float80              long double // 8, 10, 12, or 16 bytes (cc dependant)


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// ATTRIBUTES
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// has attribute
#ifdef __has_cpp_attribute
#   define ML_has_attr(expr)    __has_cpp_attribute(expr)
#else
#   define ML_has_attr(expr)    (0)
#endif

// nodiscard
#if __has_cpp_attribute(nodiscard) >= 201603L
#   define ML_NODISCARD         [[nodiscard]]
#else
#   define ML_NODISCARD
#endif

// likely
#if __has_cpp_attribute(likely) >= 201907L
#   define ML_LIKELY(expr)      ((expr)) [[likely]]
#else
#   define ML_LIKELY(expr)      ((expr))
#endif

// unlikely
#if __has_cpp_attribute(unlikely) >= 201907L
#   define ML_UNLIKELY(expr)    ((expr)) [[unlikely]]
#else
#   define ML_UNLIKELY(expr)    ((expr))
#endif

// inlining
#ifdef ML_cc_msvc
#   define ML_FORCE_INLINE      __forceinline
#   define ML_NEVER_INLINE      __declspec(noinline)
#elif defined(ML_cc_clang) || defined(ML_cc_gcc)
#   define ML_FORCE_INLINE      inline __attribute__((always_inline))
#   define ML_NEVER_INLINE      __attribute__((noinline))
#else
#   define ML_FORCE_INLINE      inline
#   define ML_NEVER_INLINE
#endif

// visibility
#ifndef ML_STATIC
#   ifdef ML_cc_msvc
#      define ML_API_EXPORT     __declspec(dllexport)
#      define ML_API_IMPORT     __declspec(dllimport)
#   elif (defined(ML_cc_clang) || defined(ML_cc_gcc)) && (ML_cc_version >= 4)
#      define ML_API_EXPORT    __attribute__((visibility("default")))
#      define ML_API_IMPORT    __attribute__((visibility("hidden")))
#   else
#      define ML_API_EXPORT
#      define ML_API_IMPORT
#   endif
#else
#      define ML_API_EXPORT
#      define ML_API_IMPORT
#endif


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
// COMPILER WARNINGS
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifdef ML_os_windows
#   pragma warning(disable: 4031)   // second formal parameter list longer than the first list
#   pragma warning(disable: 4067)   // unexpected tokens following preprocessor directive - expected a newline
#   pragma warning(disable: 4251)   // type1 needs to have dll-interface to be used by type2
#   pragma warning(disable: 4307)   // integral constant overflow
#   pragma warning(disable: 4308)   // negative integral constant converted to unsigned t
#   pragma warning(disable: 4309)   // truncation of constant value
#   pragma warning(disable: 4312)   // conversion to greater size
#   pragma warning(disable: 4723)   // potential divide by zero
#   pragma warning(disable: 6011)   // dereferencing NULL pointer
#   pragma warning(disable: 6282)   // incorrect operator
#   pragma warning(disable: 26437)  // do not slice
#   pragma warning(disable: 26444)  // avoid unnamed objecs with custom construction and destruction
#   pragma warning(disable: 26451)  // arithmetic overflow
#   pragma warning(disable: 26495)  // value may be uninitialized
#   pragma warning(disable: 26812)  // unscoped enum
#endif

#endif // !_ML_CONFIG_HPP_