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

#ifndef AFFT_DETAIL_VALIDATE_HPP
#define AFFT_DETAIL_VALIDATE_HPP

#ifndef AFFT_TOP_LEVEL_INCLUDE
# include "include.hpp"
#endif

#include "cxx.hpp"
#include "../backend.hpp"
#include "../common.hpp"
#include "../Span.hpp"
#include "../transform.hpp"

namespace afft::detail
{
  /**
   * @brief Validator class template declaration. Specializations must provide a call operator that takes a value of type T and returns true if the value is valid, false otherwise.
   * @tparam T Type of the value to validate.
   */
  template<typename T>
  struct Validator;

  /// @brief Validator for the Backend enum class.
  template<>
  struct Validator<Backend>
  {
    constexpr bool operator()(Backend backend) const noexcept
    {
      switch (backend)
      {
      case Backend::clfft:
      case Backend::cufft:
      case Backend::fftw3:
      case Backend::hipfft:
      case Backend::heffte:
      case Backend::mkl:
      case Backend::pocketfft:
      case Backend::rocfft:
      case Backend::vkfft:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the BackendMask enum class.
  template<>
  struct Validator<BackendMask>
  {
    constexpr bool operator()(BackendMask) const noexcept
    {
      return true;
    }
  };

  /// @brief Validator for the SelectStrategy enum class.
  template<>
  struct Validator<SelectStrategy>
  {
    constexpr bool operator()(SelectStrategy strategy) const noexcept
    {
      switch (strategy)
      {
      case SelectStrategy::first:
      case SelectStrategy::best:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the Precision enum class.
  template<>
  struct Validator<Precision>
  {
    constexpr bool operator()(Precision prec) const noexcept
    {
      switch (prec)
      {
      case Precision::bf16:
      case Precision::f16:
      case Precision::f32:
      case Precision::f64:
      case Precision::f64f64:
      case Precision::f80:
      case Precision::f128:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the Alignment enum class.
  template<>
  struct Validator<Alignment>
  {
    constexpr bool operator()(Alignment alignment) const noexcept
    {
      return cxx::has_single_bit(cxx::to_underlying(alignment));
    }
  };

  /// @brief Validator for the Complexity enum class.
  template<>
  struct Validator<Complexity>
  {
    constexpr bool operator()(Complexity complexity) const noexcept
    {
      switch (complexity)
      {
      case Complexity::real:
      case Complexity::complex:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the ComplexFormat enum class.
  template<>
  struct Validator<ComplexFormat>
  {
    constexpr bool operator()(ComplexFormat format) const noexcept
    {
      switch (format)
      {
      case ComplexFormat::interleaved:
      case ComplexFormat::planar:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the Direction enum class.
  template<>
  struct Validator<Direction>
  {
    constexpr bool operator()(Direction direction) const noexcept
    {
      switch (direction)
      {
      case Direction::forward:
      case Direction::inverse:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the Placement enum class.
  template<>
  struct Validator<Placement>
  {
    constexpr bool operator()(Placement placement) const noexcept
    {
      switch (placement)
      {
      case Placement::inPlace:
      case Placement::outOfPlace:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the Transform enum class.
  template<>
  struct Validator<Transform>
  {
    constexpr bool operator()(Transform transform) const noexcept
    {
      switch (transform)
      {
      case Transform::dft:
      case Transform::dht:
      case Transform::dtt:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the Target enum class.
  template<>
  struct Validator<Target>
  {
    constexpr bool operator()(Target target) const noexcept
    {
      switch (target)
      {
      case Target::cpu:
      case Target::gpu:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the Normalization enum class.
  template<>
  struct Validator<Normalization>
  {
    constexpr bool operator()(Normalization normalization) const noexcept
    {
      switch (normalization)
      {
      case Normalization::none:
      case Normalization::orthogonal:
      case Normalization::unitary:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the PrecisionTriad struct.
  template<>
  struct Validator<PrecisionTriad>
  {
    constexpr bool operator()(const PrecisionTriad& triad) const noexcept
    {
      return Validator<Precision>{}(triad.execution) &&
             Validator<Precision>{}(triad.source) &&
             Validator<Precision>{}(triad.destination);
    }
  };

  /// @brief Validator for the dft::Type enum class.
  template<>
  struct Validator<dft::Type>
  {
    constexpr bool operator()(dft::Type dftType) const noexcept
    {
      switch (dftType)
      {
      case dft::Type::complexToComplex:
      case dft::Type::realToComplex:
      case dft::Type::complexToReal:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the dht::Type enum class.
  template<>
  struct Validator<dht::Type>
  {
    constexpr bool operator()(dht::Type dhtType) const noexcept
    {
      switch (dhtType)
      {
      case dht::Type::separable:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the dtt::Type enum class.
  template<>
  struct Validator<dtt::Type>
  {
    constexpr bool operator()(dtt::Type dttType) const noexcept
    {
      switch (dttType)
      {
      case dtt::Type::dct1:
      case dtt::Type::dct2:
      case dtt::Type::dct3:
      case dtt::Type::dct4:
      case dtt::Type::dst1:
      case dtt::Type::dst2:
      case dtt::Type::dst3:
      case dtt::Type::dst4:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the Distribution enum class.
  template<>
  struct Validator<Distribution>
  {
    constexpr bool operator()(Distribution distrib) const noexcept
    {
      switch (distrib)
      {
      case Distribution::spst:
      case Distribution::spmt:
      case Distribution::mpst:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the afft::cufft::WorkspacePolicy enum class.
  template<>
  struct Validator<afft::cufft::WorkspacePolicy>
  {
    constexpr bool operator()(afft::cufft::WorkspacePolicy policy) const noexcept
    {
      switch (policy)
      {
      case afft::cufft::WorkspacePolicy::performance:
      case afft::cufft::WorkspacePolicy::minimal:
      case afft::cufft::WorkspacePolicy::user:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the afft::fftw3::PlannerFlag enum class.
  template<>
  struct Validator<afft::fftw3::PlannerFlag>
  {
    constexpr bool operator()(afft::fftw3::PlannerFlag flag) const noexcept
    {
      switch (flag)
      {
      case afft::fftw3::PlannerFlag::estimate:
      case afft::fftw3::PlannerFlag::measure:
      case afft::fftw3::PlannerFlag::patient:
      case afft::fftw3::PlannerFlag::exhaustive:
      case afft::fftw3::PlannerFlag::estimatePatient:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the afft::heffte::cpu::Backend enum class.
  template<>
  struct Validator<afft::heffte::cpu::Backend>
  {
    constexpr bool operator()(afft::heffte::cpu::Backend backend) const noexcept
    {
      switch (backend)
      {
      case afft::heffte::cpu::Backend::fftw3:
      case afft::heffte::cpu::Backend::mkl:
        return true;
      default:
        return false;
      }
    }
  };

  /// @brief Validator for the afft::heffte::gpu::Backend enum class.
  template<>
  struct Validator<afft::heffte::gpu::Backend>
  {
    constexpr bool operator()(afft::heffte::gpu::Backend backend) const noexcept
    {
      switch (backend)
      {
      case afft::heffte::gpu::Backend::cufft:
      case afft::heffte::gpu::Backend::rocfft:
        return true;
      default:
        return false;
      }
    }
  };

  /**
   * @brief Is the value valid?
   * @tparam T Type of the value.
   * @param value Value to validate.
   * @return true if the value is valid, false otherwise.
   */
  template<typename T>
  constexpr bool isValid(const T& value)
  {
    return Validator<T>{}(value);
  }

  /**
   * @brief Validates a value.
   * @tparam T Type of the value.
   * @param value Value to validate.
   */
  template<typename T>
  constexpr void validate(const T& value)
  {
    if (!isValid(value))
    {
      throw std::invalid_argument{"Invalid value"};
    }
  }

  /**
   * @brief Validates a span of values.
   * @tparam T Type of the values.
   * @param span Span of values to validate.
   */
  template<typename T>
  constexpr void validate(const Span<T>& span)
  {
    for (const auto& value : span)
    {
      validate(value);
    }
  }

  /**
   * @brief Validates a value and returns it.
   * @tparam T Type of the value.
   * @param value Value to validate.
   * @return The validated value.
   */
  template<typename T>
  [[nodiscard]] constexpr T validateAndReturn(const T& value)
  {
    validate(value);
    return value;
  }
} // namespace afft::detail

#endif /* AFFT_DETAIL_VALIDATE_HPP */
