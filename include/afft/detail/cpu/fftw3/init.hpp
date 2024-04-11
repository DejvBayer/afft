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

#ifndef AFFT_DETAIL_CPU_FFTW3_INIT_HPP
#define AFFT_DETAIL_CPU_FFTW3_INIT_HPP

#include <fftw3.h>

#include "../../error.hpp"
#include "../../../type.hpp"

namespace afft::detail::cpu::fftw3
{
  /// @brief Initialize the FFTW3 library.
  inline void init()
  {
    auto check = [](int result)
    {
      if (result == 0)
      {
        throw makeException<std::runtime_error>("[FFTW3 error] initialization failed.");
      }
    };

    check(fftwf_init_threads());
    check(fftw_init_threads());

# if AFFT_CPU_FFTW3_LONG_FOUND
    if constexpr (hasPrecision<Precision::f80>())
    {
      check(fftwl_init_threads());
    }
# endif

# if AFFT_CPU_FFTW3_QUAD_FOUND    
    if constexpr (hasPrecision<Precision::f128>())
    {
      check(fftwq_init_threads());
    }
# endif
  }

  /// @brief Finalize the FFTW3 library.
  inline void finalize()
  {
    fftwf_cleanup_threads();
    fftw_cleanup_threads();

# if AFFT_CPU_FFTW3_LONG_FOUND
    if constexpr (hasPrecision<Precision::f80>())
    {
      fftwl_cleanup_threads();
    }
# endif

# if AFFT_CPU_FFTW3_QUAD_FOUND    
    if constexpr (hasPrecision<Precision::f128>())
    {
      fftwq_cleanup_threads();
    }
# endif
  }
} // namespace afft::detail::cpu::fftw3

#endif /* AFFT_DETAIL_CPU_FFTW3_INIT_HPP */
