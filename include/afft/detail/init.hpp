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

#ifndef AFFT_DETAIL_INIT_HPP
#define AFFT_DETAIL_INIT_HPP

#include <utility>

#include "cpu/init.hpp"
#if AFFT_GPU_IS_ENABLED
# include "gpu/init.hpp"
#endif
#include "../distrib.hpp"

namespace afft::detail
{
  /// @brief Is the library initialized?
  inline bool isInitialized{false};

  /// @brief Finalize the library usage.
  inline void finalize()
  {
    if (std::exchange(isInitialized, false))
    {
      cpu::finalize();

#   if AFFT_GPU_IS_ENABLED
      gpu::finalize();
#   endif
    }
  }

  /// @brief Deinitializer.
  struct Deinitializer
  {
    /// @brief Destructor deinitializing the library if it was initialized.
    ~Deinitializer()
    {
      if (isInitialized)
      {
        finalize();
      }
    }
  };

  /// @brief Initialize the library.
  inline void init(const afft::cpu::InitParameters&                  cpuInitParams,
                   [[maybe_unused]] const afft::gpu::InitParameters& gpuInitParams)
  {
    // Deinitializer.
    static Deinitializer deinitializer{};

    if (!std::exchange(isInitialized, true))
    {
      // Init distrib implementation.
#   if AFFT_DISTRIB_IMPL_IS_ENABLED(MPI)
      int mpiIsInitialized{};
      MPI_Initialized(&mpiIsInitialized);

      if (!mpiIsInitialized)
      {
        throw std::runtime_error("MPI must be initialized before afft::init() is called.");
      }
#   endif

      // Init CPU implementation.
      cpu::init(cpuInitParams);

      // Init GPU implementation if enabled.
#   if AFFT_GPU_IS_ENABLED
      gpu::init(gpuInitParams);
#   endif
    }
  }
} // namespace afft::detail

#endif /* AFFT_DETAIL_INIT_HPP */
