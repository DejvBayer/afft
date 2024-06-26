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

#ifndef AFFT_DETAIL_ARCHITECTURE_HPP
#define AFFT_DETAIL_ARCHITECTURE_HPP

#include "common.hpp"

namespace afft::detail
{
  /**
   * @brief Architecture parameters base class
   * @tparam _target Target architecture
   * @tparam _distrib Distribution
   */
  template<Target _target, Distribution _distrib, std::size_t _shapeExt>
  struct ArchitectureParametersBase
  {
    static constexpr Target       target{_target};        ///< Target architecture
    static constexpr Distribution distribution{_distrib}; ///< Distribution
    static constexpr std::size_t  shapeExtent{_shapeExt}; ///< Shape extent
  };

  /**
   * @brief Architecture execution parameters base class
   * @tparam _target Target architecture
   * @tparam _distrib Distribution
   */
  template<Target _target, Distribution _distrib>
  struct ArchitectureExecutionParametersBase
  {
    static constexpr Target       target{_target};        ///< Target architecture
    static constexpr Distribution distribution{_distrib}; ///< Distribution
  };
} // namespace afft::detail

#endif /* AFFT_DETAIL_ARCHITECTURE_HPP */
