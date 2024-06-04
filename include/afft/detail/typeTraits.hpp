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

#ifndef AFFT_DETAIL_TYPE_TRAITS_HPP
#define AFFT_DETAIL_TYPE_TRAITS_HPP

#ifndef AFFT_TOP_LEVEL_INCLUDE
# include "include.hpp"
#endif

#include "common.hpp"
#include "../cpu.hpp"
#include "../distrib.hpp"
#include "../gpu.hpp"
#include "../transform.hpp"
#include "../type.hpp"

namespace afft::detail
{
  /**
   * @brief TypeProperties helper. Removes const and volatile from Complex template parameter type.
   * @tparam T The type.
   */
  template<typename T>
  struct TypePropertiesHelper
    : TypeProperties<std::remove_cv_t<T>> {};
  
  /// @brief Specialization for std::complex.
  template<typename T>
  struct TypePropertiesHelper<std::complex<T>>
    : TypeProperties<std::complex<std::remove_cv_t<T>>> {};

  /**
   * @brief TransformParameters type for given transform.
   * @tparam transform The transform type.
   */
  template<Transform transform>
  struct TransformParametersSelect;

  /// @brief Specialization for dft transform.
  template<>
  struct TransformParametersSelect<Transform::dft>
  {
    using Type = dft::Parameters<>;
  };

  /// @brief Specialization for dht transform.
  template<>
  struct TransformParametersSelect<Transform::dht>
  {
    using Type = dht::Parameters<>;
  };

  /// @brief Specialization for dtt transform.
  template<>
  struct TransformParametersSelect<Transform::dtt>
  {
    using Type = dtt::Parameters<>;
  };

  /**
   * @brief Check if the type is TransformParameters.
   * @tparam T The type.
   */
  template<typename T>
  struct IsTransformParameters : std::false_type {};

  /// @brief Specialization for dft transform parameters.
  template<std::size_t sRank, std::size_t tRank>
  struct IsTransformParameters<afft::dft::Parameters<sRank, tRank>> : std::true_type {};

  /// @brief Specialization for dht transform parameters.
  template<std::size_t sRank, std::size_t tRank>
  struct IsTransformParameters<afft::dht::Parameters<sRank, tRank>> : std::true_type {};

  /// @brief Specialization for dtt transform parameters.
  template<std::size_t sRank, std::size_t tRank, std::size_t ttRank>
  struct IsTransformParameters<afft::dtt::Parameters<sRank, tRank, ttRank>> : std::true_type {};

  /**
   * @brief TransformParameters template ranks.
   * @tparam T The transform parameters type.
   */
  template<typename T>
  struct TransformParametersTemplateRanks;

  /// @brief Specialization for dft transform parameters.
  template<std::size_t sRank, std::size_t tRank>
  struct TransformParametersTemplateRanks<afft::dft::Parameters<sRank, tRank>>
  {
    static constexpr std::size_t shape     = sRank;
    static constexpr std::size_t transform = tRank;
  };
  
  /// @brief Specialization for dht transform parameters.
  template<std::size_t sRank, std::size_t tRank>
  struct TransformParametersTemplateRanks<afft::dht::Parameters<sRank, tRank>>
  {
    static constexpr std::size_t shape     = sRank;
    static constexpr std::size_t transform = tRank;
  };

  /// @brief Specialization for dtt transform parameters.
  template<std::size_t sRank, std::size_t tRank, std::size_t ttRank>
  struct TransformParametersTemplateRanks<afft::dtt::Parameters<sRank, tRank, ttRank>>
  {
    static constexpr std::size_t shape         = sRank;
    static constexpr std::size_t transform     = tRank;
    static constexpr std::size_t transformType = ttRank;
  };

  /**
   * @brief ArchitectureParameters type selection based on given target and distribution.
   * @tparam target The target type.
   * @tparam distrib The distribution type.
   */
  template<Target target, Distribution distrib>
  struct ArchParametersSelect;

  /// @brief Specialization for spst cpu target.
  template<>
  struct ArchParametersSelect<Target::cpu, Distribution::spst>
  {
    using Type = afft::cpu::Parameters<>;
  };

  /// @brief Specialization for spst gpu target.
  template<>
  struct ArchParametersSelect<Target::gpu, Distribution::spst>
  {
    using Type = afft::gpu::Parameters<>;
  };

  /// @brief Specialization for distributed spmt gpu target.
  template<>
  struct ArchParametersSelect<Target::gpu, Distribution::spmt>
  {
    using Type = afft::spmt::gpu::Parameters<>;
  };

  /// @brief Specialization for distributed mpst cpu target.
  template<>
  struct ArchParametersSelect<Target::cpu, Distribution::mpst>
  {
    using Type = afft::mpst::cpu::Parameters<>;
  };

  /// @brief Specialization for distributed mpst gpu target.
  template<>
  struct ArchParametersSelect<Target::gpu, Distribution::mpst>
  {
    using Type = afft::mpst::gpu::Parameters<>;
  };

  /**
   * @brief Check if the type is ArchitectureParameters.
   * @tparam T The type.
   */
  template<typename T>
  struct IsArchParameters : std::false_type {};

  /// @brief Specialization for cpu target parameters.
  template<std::size_t sRank>
  struct IsArchParameters<afft::spst::cpu::Parameters<sRank>> : std::true_type {};

  /// @brief Specialization for gpu target parameters.
  template<std::size_t sRank>
  struct IsArchParameters<afft::spst::gpu::Parameters<sRank>> : std::true_type {};

  /// @brief Specialization for distributed spmt gpu target parameters.
  template<std::size_t sRank>
  struct IsArchParameters<afft::spmt::gpu::Parameters<sRank>> : std::true_type {};

  /// @brief Specialization for distributed mpst cpu target parameters.
  template<std::size_t sRank>
  struct IsArchParameters<afft::mpst::cpu::Parameters<sRank>> : std::true_type {};

  /// @brief Specialization for distributed cpu target parameters.
  template<std::size_t sRank>
  struct IsArchParameters<afft::mpst::gpu::Parameters<sRank>> : std::true_type {};

  /**
   * @brief ArchParameters template ranks.
   * @tparam T The architecture parameters type.
   */
  template<typename T>
  struct ArchParametersTemplateRanks;

  /// @brief Specialization for spst cpu target parameters.
  template<std::size_t sRank>
  struct ArchParametersTemplateRanks<afft::spst::cpu::Parameters<sRank>>
  {
    static constexpr std::size_t shape = sRank;
  };

  /// @brief Specialization for spst gpu target parameters.
  template<std::size_t sRank>
  struct ArchParametersTemplateRanks<afft::spst::gpu::Parameters<sRank>>
  {
    static constexpr std::size_t shape = sRank;
  };

  /// @brief Specialization for distributed spmt gpu target parameters.
  template<std::size_t sRank>
  struct ArchParametersTemplateRanks<afft::spmt::gpu::Parameters<sRank>>
  {
    static constexpr std::size_t shape = sRank;
  };

  /// @brief Specialization for distributed mpst cpu target parameters.
  template<std::size_t sRank>
  struct ArchParametersTemplateRanks<afft::mpst::cpu::Parameters<sRank>>
  {
    static constexpr std::size_t shape = sRank;
  };

  /// @brief Specialization for distributed mpst gpu target parameters.
  template<std::size_t sRank>
  struct ArchParametersTemplateRanks<afft::mpst::gpu::Parameters<sRank>>
  {
    static constexpr std::size_t shape = sRank;
  };

  /**
   * @brief BackendParameters type for given architecture.
   * @tparam target The target type.
   * @tparam distrib The distribution type.
   */
  template<Target target, Distribution distrib>
  struct BackendParametersSelect;

  /// @brief Specialization for spst cpu target.
  template<>
  struct BackendParametersSelect<Target::cpu, Distribution::spst>
  {
    using Type = afft::spst::cpu::BackendParameters;
  };

  /// @brief Specialization for spst gpu target.
  template<>
  struct BackendParametersSelect<Target::gpu, Distribution::spst>
  {
    using Type = afft::spst::gpu::BackendParameters;
  };

  /// @brief Specialization for spmt gpu target.
  template<>
  struct BackendParametersSelect<Target::gpu, Distribution::spmt>
  {
    using Type = afft::spmt::gpu::BackendParameters;
  };

  /// @brief Specialization for mpst cpu target.
  template<>
  struct BackendParametersSelect<Target::cpu, Distribution::mpst>
  {
    using Type = afft::mpst::cpu::BackendParameters;
  };

  /// @brief Specialization for mpst gpu target.
  template<>
  struct BackendParametersSelect<Target::gpu, Distribution::mpst>
  {
    using Type = afft::mpst::gpu::BackendParameters;
  };

  /**
   * @brief Check if the type is BackendParametersSelect.
   * @tparam T The type.
   */
  template<typename T>
  struct IsBackendParameters : std::false_type {};

  /// @brief Specialization for spst cpu BackendParameters.
  template<>
  struct IsBackendParameters<afft::spst::cpu::BackendParameters> : std::true_type {};

  /// @brief Specialization for spst gpu BackendParameters.
  template<>
  struct IsBackendParameters<afft::spst::gpu::BackendParameters> : std::true_type {};

  /// @brief Specialization for spmt gpu BackendParameters.
  template<>
  struct IsBackendParameters<afft::spmt::gpu::BackendParameters> : std::true_type {};

  /// @brief Specialization for mpst cpu BackendParameters.
  template<>
  struct IsBackendParameters<afft::mpst::cpu::BackendParameters> : std::true_type {};

  /// @brief Specialization for mpst gpu BackendParameters.
  template<>
  struct IsBackendParameters<afft::mpst::gpu::BackendParameters> : std::true_type {};

  /**
   * @brief ExecutionParameters type for given architecture.
   * @tparam target The target type.
   * @tparam distrib The distribution type.
   */
  template<Target target, Distribution distrib>
  struct ArchExecutionParametersSelect;

  /// @brief Specialization for cpu target.
  template<>
  struct ArchExecutionParametersSelect<Target::cpu, Distribution::spst>
  {
    using Type = afft::cpu::ExecutionParameters;
  };

  /// @brief Specialization for gpu target.
  template<>
  struct ArchExecutionParametersSelect<Target::gpu, Distribution::spst>
  {
    using Type = afft::gpu::ExecutionParameters;
  };

  /// @brief Specialization for distributed spmt gpu target.
  template<>
  struct ArchExecutionParametersSelect<Target::gpu, Distribution::spmt>
  {
    using Type = afft::spmt::gpu::ExecutionParameters;
  };

  /// @brief Specialization for distributed mpst cpu target.
  template<>
  struct ArchExecutionParametersSelect<Target::cpu, Distribution::mpst>
  {
    using Type = afft::mpst::cpu::ExecutionParameters;
  };

  /// @brief Specialization for distributed mpst gpu target.
  template<>
  struct ArchExecutionParametersSelect<Target::gpu, Distribution::mpst>
  {
    using Type = afft::mpst::gpu::ExecutionParameters;
  };

  /**
   * @brief Check if the type is ExecutionParameters.
   * @tparam T The type.
   */
  template<typename T>
  struct IsExecutionParameters : std::false_type {};

  /// @brief Specialization for spst cpu ExecutionParameters.
  template<>
  struct IsExecutionParameters<afft::spst::cpu::ExecutionParameters> : std::true_type {};

  /// @brief Specialization for spst gpu ExecutionParameters.
  template<>
  struct IsExecutionParameters<afft::spst::gpu::ExecutionParameters> : std::true_type {};

  /// @brief Specialization for spmt gpu ExecutionParameters.
  template<>
  struct IsExecutionParameters<afft::spmt::gpu::ExecutionParameters> : std::true_type {};

  /// @brief Specialization for mpst cpu ExecutionParameters.
  template<>
  struct IsExecutionParameters<afft::mpst::cpu::ExecutionParameters> : std::true_type {};

  /// @brief Specialization for mpst gpu ExecutionParameters.
  template<>
  struct IsExecutionParameters<afft::mpst::gpu::ExecutionParameters> : std::true_type {};
} // namespace afft::detail

#endif /* AFFT_DETAIL_TYPE_TRAITS_HPP */
