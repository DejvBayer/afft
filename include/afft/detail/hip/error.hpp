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

#ifndef AFFT_DETAIL_HIP_ERROR_HPP
#define AFFT_DETAIL_HIP_ERROR_HPP

#ifndef AFFT_TOP_LEVEL_INCLUDE
# include "../include.hpp"
#endif

#include "../../exception.hpp"

namespace afft::detail::hip
{
  /**
   * @brief Check if HIP error is ok.
   * @param error HIP error.
   * @return True if error is hipSuccess, false otherwise.
   */
  [[nodiscard]] inline constexpr bool isOk(hipError_t error)
  {
    return (error == hipSuccess);
  }

  /**
   * @brief Check if HIP error is valid.
   * @param error HIP error.
   * @throw GpuBackendError if error is not valid.
   */
  inline void checkError(hipError_t error)
  {
    if (!isOk(error))
    {
      throw GpuBackendError{cformatNothrow("%s - %s", hipGetErrorName(error), hipGetErrorString(error))};
    }
  }
} // namespace afft::detail::hip

#endif /* AFFT_DETAIL_GPU_ERROR_HPP */
