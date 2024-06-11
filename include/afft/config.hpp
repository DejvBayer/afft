/*
  This file is part of afft library.

  Copyright (c) 2024 David Bayer

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef AFFT_CONFIG_HPP
#define AFFT_CONFIG_HPP

#define AFFT_VERSION_MAJOR      0 ///< Major version.
#define AFFT_VERSION_MINOR      1 ///< Minor version.
#define AFFT_VERSION_PATCH      0 ///< Patch version.

/**
 * @brief Expand and concatenate two tokens.
 * @param a First token.
 * @param b Second token.
 * @return Concatenated tokens.
 */
#define AFFT_EXPAND_AND_CONCAT(a, b) \
  AFFT_EXPAND_AND_CONCAT_HELPER(a, b)

// Helper macro for AFFT_EXPAND_AND_CONCAT
#define AFFT_EXPAND_AND_CONCAT_HELPER(a, b) \
  a##b

/// @brief clFFT backend id
#define AFFT_BACKEND_CLFFT      (1 << 0)
/// @brief cuFFT backend id
#define AFFT_BACKEND_CUFFT      (1 << 1)
/// @brief FFTW3 backend id
#define AFFT_BACKEND_FFTW3      (1 << 2)
/// @brief HeFFTe backend id
#define AFFT_BACKEND_HEFFTE     (1 << 3)
/// @brief hipFFT backend id
#define AFFT_BACKEND_HIPFFT     (1 << 4)
/// @brief MKL backend id
#define AFFT_BACKEND_MKL        (1 << 5)
/// @brief PocketFFT backend id
#define AFFT_BACKEND_POCKETFFT  (1 << 6)
/// @brief rocFFT backend id
#define AFFT_BACKEND_ROCFFT     (1 << 7)
/// @brief VkFFT backend id
#define AFFT_BACKEND_VKFFT      (1 << 8)

/**
 * @brief Is backend enabled?
 * @param bckndName Backend name.
 * @return True if the backend is enabled, false otherwise.
 */
#define AFFT_BACKEND_IS_ENABLED(bckndName) \
  (((AFFT_BACKEND_MASK) & AFFT_BACKEND_##bckndName) != 0)

// Disable FFTW3 quad precision if the compiler is not supported
#if !((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) \
      && !(defined(__ICC) || defined(__INTEL_COMPILER) || defined(__CUDACC__) || defined(__PGI)) \
      && (defined(__i386__) || defined(__x86_64__) || defined(__ia64__)))
# ifdef AFFT_FFTW3_HAS_QUAD
#   undef AFFT_FFTW3_HAS_QUAD
# endif
#endif

/// @brief Macro for disabling GPU support
#define AFFT_GPU_BACKEND_NONE   0
/// @brief Macro for CUDA GPU backend
#define AFFT_GPU_BACKEND_CUDA   1
/// @brief Macro for HIP GPU backend
#define AFFT_GPU_BACKEND_HIP    2
/// @brief Macro for OpenCL GPU backend
#define AFFT_GPU_BACKEND_OPENCL 3

/// @brief Macro for checking if GPU is enabled
#define AFFT_GPU_IS_ENABLED     (!AFFT_GPU_BACKEND_IS(NONE))

/**
 * @brief Macro for checking if the GPU backend is selected
 * @param bckndName Name of the backend
 * @return True zero if the backend is selected, false otherwise
 */
#define AFFT_GPU_BACKEND_IS(bckndName) \
  (AFFT_EXPAND_AND_CONCAT(AFFT_GPU_BACKEND_, AFFT_GPU_BACKEND) == AFFT_GPU_BACKEND_##bckndName)

/// @brief Disable multi-process backend
#define AFFT_MP_BACKEND_NONE    0
/// @brief MPI multi-process backend
#define AFFT_MP_BACKEND_MPI     1

/// @brief Check if multi-process is enabled
#define AFFT_MP_IS_ENABLED      (!AFFT_MP_BACKEND_IS(NONE))

/**
 * @brief Check if multi-process backend is enabled
 * @param bckndName multi-process backend name
 * @return true if multi-process backend is enabled, false otherwise
 */
#define AFFT_MP_BACKEND_IS(bckndName) \
  (AFFT_EXPAND_AND_CONCAT(AFFT_MP_BACKEND_, AFFT_MP_BACKEND) == AFFT_MP_BACKEND_##bckndName)

// If max dimension count is not defined, use 4 as default
#ifdef AFFT_MAX_DIM_COUNT
# if AFFT_MAX_DIM_COUNT < 1
#   error "AFFT_MAX_DIM_COUNT must be at least 1"
# endif
#else
# define AFFT_MAX_DIM_COUNT     4
#endif

// Check if GPU backend is defined (GPU support is enabled)
#ifdef AFFT_GPU_BACKEND
# if !(AFFT_GPU_BACKEND_IS(CUDA) || AFFT_GPU_BACKEND_IS(HIP) || AFFT_GPU_BACKEND_IS(OPENCL))
#   error "Invalid GPU backend"
# endif
#else
# define AFFT_GPU_BACKEND       NONE
#endif

// Check if multi-process backend is supported
#ifdef AFFT_MP_BACKEND
# if !AFFT_MP_BACKEND_IS(MPI)
#  error "Unsupported multi-process backend"
# endif
#else
# define AFFT_MP_BACKEND        NONE
#endif

// Define the AFFT_CXX_VERSION version macro
#ifdef _MSVC_LANG
#  define AFFT_CXX_VERSION      _MSVC_LANG
#else
#  define AFFT_CXX_VERSION      __cplusplus
#endif

// Define AFFT_EXPORT macro to expand to nothing if not defined
#ifndef AFFT_EXPORT
# define AFFT_EXPORT
#endif

// Define AFFT_HEADER_ONLY_INLINE macro to expand to inline if not defined
#ifndef AFFT_HEADER_ONLY_INLINE
# ifdef AFFT_HEADER_ONLY
#   define AFFT_HEADER_ONLY_INLINE inline
# else
#   define AFFT_HEADER_ONLY_INLINE
# endif
#endif

/**
 * @brief Define AFFT_PARAM macro to enable passing parameters containing commas
 * @param ... Parameter
 */
#define AFFT_PARAM(...)         __VA_ARGS__

// If C++ version is 20, try to include <version> header
#if (AFFT_CXX_VERSION >= 202002L) && __has_include(<version>)
# define AFFT_CXX_HAS_VERSION
# include <version>
#endif

// implementation of C++20 requires clause for older C++ versions, should be used as:
// AFFT_TEMPL_REQUIRES(typename T, std::is_integral_v<T>)
// auto func(...) { ...}
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
  /// @brief Macro for requires clause
# define AFFT_TEMPL_REQUIRES(templParam, requiredExpr) \
    template<templParam> requires(requiredExpr)
#else
  /// @brief Macro for requires clause, using std::enable_if_t for older C++ versions
# define AFFT_TEMPL_REQUIRES(templParam, requiredExpr) \
    template<templParam, std::enable_if_t<requiredExpr, int> = 0>
#endif

// implementation of C++20 requires clause for older C++ versions, should be used as:
// template<typename T>
// auto func() -> AFFT_RET_REQUIRES(returnType, std::is_integral_v<T>) { ... }
#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
  /// @brief Macro for requires clause
# define AFFT_RET_REQUIRES(retType, requiredExpr) \
    retType requires(requiredExpr)
#else
  /// @brief Macro for requires clause, using std::enable_if_t for older C++ versions
# define AFFT_RET_REQUIRES(retType, requiredExpr) \
    std::enable_if_t<requiredExpr, retType>
#endif

// Check if C++20 <span> is supported
#if defined(AFFT_CXX_HAS_VERSION) && defined(__cpp_lib_span) && (__cpp_lib_span >= 202002L)
# define AFFT_CXX_HAS_SPAN
#endif

// Check if C++23 `import std` is supported
#if defined(AFFT_CXX_HAS_VERSION) && defined(__cpp_lib_modules) && (__cpp_lib_modules >= 202207L)
# define AFFT_CXX_HAS_IMPORT_STD
#endif

// Check if C++23 <stdfloat> is implemented
#if (AFFT_CXX_VERSION >= 202002L) && __has_include(<stdfloat>)
# define AFFT_CXX_HAS_STD_FLOAT
#endif

#endif /* AFFT_CONFIG_HPP */
