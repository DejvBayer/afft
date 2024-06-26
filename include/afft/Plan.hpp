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

#ifndef AFFT_PLAN_HPP
#define AFFT_PLAN_HPP

#ifndef AFFT_TOP_LEVEL_INCLUDE
# include "detail/include.hpp"
#endif

#include "backend.hpp"
#include "common.hpp"
#include "transform.hpp"
#include "detail/Desc.hpp"

AFFT_EXPORT namespace afft
{
  class Plan : public std::enable_shared_from_this<Plan>
  {
    friend struct detail::DescGetter; 

    private:
      /// @brief Default execution parameters helper.
      struct DefaultExecParams
      {
        /// @brief Conversion to any execution parameters.
        template<typename ExecParamsT>
        [[nodiscard]] constexpr operator ExecParamsT() const noexcept
        {
          static_assert(isExecutionParameters<ExecParamsT>, "invalid execution parameters type");

          return ExecParamsT{};
        }
      };

      /**
       * @brief Check if the type is known execution parameters.
       * @tparam T Type to check.
       */
      template<typename T>
      static constexpr bool isKnownExecParams = isExecutionParameters<T> || std::is_same_v<T, DefaultExecParams>;

    public:
      /// @brief Copy constructor is deleted.
      Plan(const Plan&) = delete;

      /// @brief Move constructor.
      Plan(Plan&&) = default;

      /// @brief Destructor.
      virtual ~Plan() = default;

      /// @brief Copy assignment operator is deleted.
      Plan& operator=(const Plan&) = delete;

      /// @brief Move assignment operator.
      Plan& operator=(Plan&&) = default;

      /// @brief Get the transform.
      [[nodiscard]] constexpr Transform getTransform() const
      {
        return mDesc.getTransform();
      }

      /// @brief Get shape rank.
      [[nodiscard]] constexpr std::size_t getShapeRank() const noexcept
      {
        return mDesc.getShapeRank();
      }

      /// @brief Get transform rank.
      [[nodiscard]] constexpr std::size_t getTransformRank() const noexcept
      {
        return mDesc.getTransformRank();
      }

      /**
       * @brief Get transform parameters.
       * @tparam transform Transform type
       * @return Transform parameters
       */
      template<Transform transform>
      [[nodiscard]] constexpr TransformParameters<transform> getTransformParameters() const
      {
        static_assert(detail::isValid(transform), "invalid transform type");

        if (transform != getTransform())
        {
          throw std::invalid_argument("plan transform does not match requested transform");
        }

        return mDesc.getTransformParameters<transform>();
      }

      /**
       * @brief Get target.
       * @return Target
       */
      [[nodiscard]] constexpr Target getTarget() const noexcept
      {
        return mDesc.getTarget();
      }

      /**
       * @brief Get target count.
       * @return Target count.
       */
      [[nodiscard]] constexpr std::size_t getTargetCount() const noexcept
      {
        return mDesc.getTargetCount();
      }

      /**
       * @brief Get distribution.
       * @return Distribution
       */
      [[nodiscard]] constexpr Distribution getDistribution() const noexcept
      {
        return mDesc.getDistribution();
      }

      /**
       * @brief Get architecture parameters.
       * @tparam target Target type.
       * @tparam distribution Distribution type.
       * @return Architecture parameters.
       */
      template<Target target, Distribution distribution>
      [[nodiscard]] constexpr ArchitectureParameters<target, distribution> getArchitectureParameters() const
      {
        static_assert(detail::isValid(target), "invalid target type");
        static_assert(detail::isValid(distribution), "invalid distribution type");

        if (target != getTarget())
        {
          throw std::invalid_argument("plan target does not match requested target");
        }

        if (distribution != getDistribution())
        {
          throw std::invalid_argument("plan distribution does not match requested distribution");
        }

        return mDesc.getArchitectureParameters<target, distribution>();
      }

      /**
       * @brief Get backend.
       * @return Backend
       */
      [[nodiscard]] virtual Backend getBackend() const noexcept = 0;

      /**
       * @brief Get workspace size.
       * @return Workspace size.
       */
      [[nodiscard]] virtual View<std::size_t> getWorkspaceSize() const noexcept
      {
        return {};
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcDstT Source/destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param srcDst Source/destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename SrcDstT, typename ExecParamsT = DefaultExecParams>
      void execute(SrcDstT* srcDst, const ExecParamsT execParams = {})
      {
        static_assert(isKnownType<SrcDstT>, "unknown source/destination type");
        static_assert(!std::is_const_v<SrcDstT>, "source/destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<SrcDstT*>{&srcDst, 1}, View<SrcDstT*>{&srcDst, 1}, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcDstT Source/destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param srcDst Source/destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename SrcDstT, typename ExecParamsT = DefaultExecParams>
      void execute(PlanarComplex<SrcDstT> srcDst, const ExecParamsT execParams = {})
      {
        static_assert(isRealType<SrcDstT>, "source/destination type must be real");
        static_assert(!std::is_const_v<SrcDstT>, "source/destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<PlanarComplex<SrcDstT>>{&srcDst, 1},
                     View<PlanarComplex<SrcDstT>>{&srcDst, 1},
                     execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcDstT Source/destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param srcDst Source/destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcDstT, typename ExecParamsT = DefaultExecParams>
      void execute(View<SrcDstT*> srcDst, const ExecParamsT execParams = {})
      {
        static_assert(isKnownType<SrcDstT>, "unknown source/destination type");
        static_assert(!std::is_const_v<SrcDstT>, "source/destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(srcDst, srcDst, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcDstT Source/destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param srcDst Source/destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcDstT, typename ExecParamsT = DefaultExecParams>
      void execute(View<PlanarComplex<SrcDstT>> srcDst, const ExecParamsT execParams = {})
      {
        static_assert(isRealType<SrcDstT>, "source/destination type must be real");
        static_assert(!std::is_const_v<SrcDstT>, "source/destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(srcDst, srcDst, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT = DefaultExecParams>
      void execute(SrcT* src, DstT* dst, const ExecParamsT execParams = {})
      {
        static_assert(isKnownType<SrcT>, "unknown source type");
        static_assert(isKnownType<DstT>, "unknown destination type");
        static_assert(!std::is_const_v<DstT>, "destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<SrcT*>{&src, 1}, View<DstT*>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT = DefaultExecParams>
      void execute(PlanarComplex<SrcT> src, DstT* dst, const ExecParamsT execParams = {})
      {
        static_assert(isRealType<SrcT>, "source type must be real");
        static_assert(isKnownType<DstT>, "unknown destination type");
        static_assert(!std::is_const_v<DstT>, "destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<PlanarComplex<SrcT>>{&src, 1}, View<DstT*>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT = DefaultExecParams>
      void execute(SrcT* src, PlanarComplex<DstT> dst, const ExecParamsT execParams = {})
      {
        static_assert(isKnownType<SrcT>, "unknown source type");
        static_assert(isRealType<DstT>, "destination type must be real");
        static_assert(!std::is_const_v<DstT>, "destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<SrcT*>{&src, 1}, View<PlanarComplex<DstT>>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT = DefaultExecParams>
      void execute(PlanarComplex<SrcT> src, PlanarComplex<DstT> dst, const ExecParamsT execParams = {})
      {
        static_assert(isRealType<SrcT>, "source type must be real");
        static_assert(isRealType<DstT>, "destination type must be real");
        static_assert(!std::is_const_v<DstT>, "destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<PlanarComplex<SrcT>>{&src, 1}, View<PlanarComplex<DstT>>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT = DefaultExecParams>
      void execute(View<SrcT*> src, View<DstT*> dst, const ExecParamsT execParams = {})
      {
        static_assert(isKnownType<SrcT>, "unknown source type");
        static_assert(isKnownType<DstT>, "unknown destination type");
        static_assert(!std::is_const_v<DstT>, "destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT = DefaultExecParams>
      void execute(View<PlanarComplex<SrcT>> src, View<DstT*> dst, const ExecParamsT execParams = {})
      {
        static_assert(isRealType<SrcT>, "source type must be real");
        static_assert(isKnownType<DstT>, "unknown destination type");
        static_assert(!std::is_const_v<DstT>, "destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT = DefaultExecParams>
      void execute(View<SrcT*> src, View<PlanarComplex<DstT>> dst, const ExecParamsT execParams = {})
      {
        static_assert(isKnownType<SrcT>, "unknown source type");
        static_assert(isRealType<DstT>, "destination type must be real");
        static_assert(!std::is_const_v<DstT>, "destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT = DefaultExecParams>
      void execute(View<PlanarComplex<SrcT>> src, View<PlanarComplex<DstT>> dst, const ExecParamsT execParams = {})
      {
        static_assert(isRealType<SrcT>, "source type must be real");
        static_assert(isRealType<DstT>, "destination type must be real");
        static_assert(!std::is_const_v<DstT>, "destination type must be non-const");
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(const void* src, void* dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<const void*>{&src, 1}, View<void*>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(void* src, void* dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<void*>{&src, 1}, View<void*>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(PlanarComplex<const void> src, void* dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<PlanarComplex<const void>>{&src, 1}, View<void*>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(PlanarComplex<void> src, void* dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<PlanarComplex<void>>{&src, 1}, View<void*>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(const void* src, PlanarComplex<void> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<const void*>{&src, 1}, View<PlanarComplex<void>>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(void* src, PlanarComplex<void> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<void*>{&src, 1}, View<PlanarComplex<void>>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(PlanarComplex<const void> src, PlanarComplex<void> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<PlanarComplex<const void>>{&src, 1}, View<PlanarComplex<void>>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(PlanarComplex<void> src, PlanarComplex<void> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(View<PlanarComplex<void>>{&src, 1}, View<PlanarComplex<void>>{&dst, 1}, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(View<const void*> src, View<void*> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(View<void*> src, View<void*> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(View<PlanarComplex<const void>> src, View<void*> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(View<PlanarComplex<void>> src, View<void*> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(View<const void*> src, View<PlanarComplex<void>> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(View<void*> src, View<PlanarComplex<void>> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(View<PlanarComplex<const void>> src, View<PlanarComplex<void>> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

      /**
       * @brief Execute the plan without type checking.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffer.
       * @param dst Destination buffer.
       * @param execParams Execution parameters.
       */
      template<typename ExecParamsT = DefaultExecParams>
      void executeUnsafe(View<PlanarComplex<void>> src, View<PlanarComplex<void>> dst, const ExecParamsT& execParams = {})
      {
        static_assert(isKnownExecParams<ExecParamsT>, "invalid execution parameters type");

        executeImpl1(src, dst, execParams);
      }

    protected:
      /// @brief Default constructor is deleted.
      Plan() = delete;

      /// @brief Constructor.
      Plan(const detail::Desc& desc)
      : mDesc{desc}
      {}

      /// @brief Get the plan description.
      [[nodiscard]] const detail::Desc& getDesc() const noexcept
      {
        return mDesc;
      }

      /**
       * @brief Execute the plan backend implementation.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      virtual void executeBackendImpl([[maybe_unused]] View<void*>                                 src,
                                      [[maybe_unused]] View<void*>                                 dst,
                                      [[maybe_unused]] const afft::spst::cpu::ExecutionParameters& execParams)
      {
        throw std::logic_error{"backend does not implement spst cpu execution"};
      }

      /**
       * @brief Execute the plan backend implementation.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      virtual void executeBackendImpl([[maybe_unused]] View<void*>                                 src,
                                      [[maybe_unused]] View<void*>                                 dst,
                                      [[maybe_unused]] const afft::spst::gpu::ExecutionParameters& execParams)
      {
        throw std::logic_error{"backend does not implement spst gpu execution"};
      }

      /**
       * @brief Execute the plan backend implementation.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      virtual void executeBackendImpl([[maybe_unused]] View<void*>                                 src,
                                      [[maybe_unused]] View<void*>                                 dst,
                                      [[maybe_unused]] const afft::spmt::gpu::ExecutionParameters& execParams)
      {
        throw std::logic_error{"backend does not implement spmt gpu execution"};
      }

      /**
       * @brief Execute the plan backend implementation.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      virtual void executeBackendImpl([[maybe_unused]] View<void*>                                 src,
                                      [[maybe_unused]] View<void*>                                 dst,
                                      [[maybe_unused]] const afft::mpst::cpu::ExecutionParameters& execParams)
      {
        throw std::logic_error{"backend does not implement mpst cpu execution"};
      }

      /**
       * @brief Execute the plan backend implementation.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      virtual void executeBackendImpl([[maybe_unused]] View<void*>                                 src,
                                      [[maybe_unused]] View<void*>                                 dst,
                                      [[maybe_unused]] const afft::mpst::gpu::ExecutionParameters& execParams)
      {
        throw std::logic_error{"backend does not implement mpst gpu execution"};
      }
    
      detail::Desc mDesc;
    private:
      /**
       * @brief Check execution type properties.
       * @param srcPrecision Source precision.
       * @param srcComplexity Source complexity.
       * @param dstPrecision Destination precision.
       * @param dstComplexity Destination complexity.
       */
      void checkExecTypeProps(const Precision  srcPrecision,
                              const Complexity srcComplexity,
                              const Precision  dstPrecision,
                              const Complexity dstComplexity) const
      {
        const auto& prec = mDesc.getPrecision();
        const auto [refSrcCmpl, refDstCmpl] = mDesc.getSrcDstComplexity();

        switch (mDesc.getPlacement())
        {
        case Placement::inPlace:
          if (srcPrecision != prec.source && srcPrecision != prec.destination)
          {
            throw std::invalid_argument{"invalid type precision"};
          }

          if (srcComplexity != refSrcCmpl && srcComplexity != refDstCmpl)
          {
            throw std::invalid_argument{"invalid type complexity"};
          }
          break;
        case Placement::outOfPlace:
          if (srcPrecision != prec.source)
          {
            throw std::invalid_argument{"invalid source type precision"};
          }
          
          if (dstPrecision != prec.destination)
          {
            throw std::invalid_argument{"invalid destination type precision"};
          }

          if (srcComplexity != refSrcCmpl)
          {
            throw std::invalid_argument{"invalid source type complexity"};
          }

          if (dstComplexity != refDstCmpl)
          {
            throw std::invalid_argument{"invalid destination type complexity"};
          }
          break;
        default:
          detail::cxx::unreachable();
        }
      }

      /// @brief Check if the source is preserved.
      void checkSrcIsPreserved() const
      {
        if (!mDesc.getPreserveSource())
        {
          throw std::invalid_argument("running destructive transform on const source data");
        }
      }

      /**
       * @brief Check execution buffer count.
       * @param srcCount Source buffer count.
       * @param dstCount Destination buffer count.
       */
      void checkBufferCount(const std::size_t srcCount, const std::size_t dstCount) const
      {
        const auto targetCount = mDesc.getTargetCount();

        if (srcCount != targetCount)
        {
          throw std::invalid_argument{"invalid source buffer count"};
        }

        if (dstCount != targetCount)
        {
          throw std::invalid_argument{"invalid destination buffer count"};
        }
      }

      /**
       * @brief Check placement.
       * @param placement Placement.
       */
      void checkPlacement(const Placement placement) const
      {
        if (placement != mDesc.getPlacement())
        {
          throw std::invalid_argument("placement does not match plan placement");
        }
      }

      /**
       * @brief Level 1 implementation of the execute method.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT>
      void executeImpl1(View<SrcT*> src, View<DstT*> dst, const ExecParamsT& execParams)
      {
        static_assert((std::is_void_v<SrcT> && std::is_void_v<DstT>) ||
                      (!std::is_void_v<SrcT> && !std::is_void_v<DstT>), "invalid source and destination types");

        using NonConstSrcT = std::remove_const_t<SrcT>;

        if constexpr (std::is_const_v<SrcT>)
        {
          checkSrcIsPreserved();
        }

        View<NonConstSrcT*> srcNonConst(const_cast<NonConstSrcT* const*>(src.data()), src.size());

        checkBufferCount(src.size(), dst.size());
        
        const bool isInPlace = std::equal(src.begin(), src.end(), dst.begin(), [](const auto& s, const auto& d)
        {
          return reinterpret_cast<std::uintptr_t>(s) == reinterpret_cast<std::uintptr_t>(d);
        });
        checkPlacement((isInPlace) ? Placement::inPlace : Placement::outOfPlace);

        if constexpr (!std::is_void_v<SrcT> && !std::is_void_v<DstT>)
        {
          checkExecTypeProps(typePrecision<SrcT>, typeComplexity<SrcT>, typePrecision<DstT>, typeComplexity<DstT>);
        }

        executeImpl2(srcNonConst, dst, execParams);
      }

      /**
       * @brief Level 1 implementation of the execute method.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT>
      void executeImpl1(View<PlanarComplex<SrcT>> src, View<DstT*> dst, const ExecParamsT& execParams)
      {
        static_assert((std::is_void_v<SrcT> && std::is_void_v<DstT>) ||
                      (!std::is_void_v<SrcT> && !std::is_void_v<DstT>), "invalid source and destination types");

        using NonConstSrcT = std::remove_const_t<SrcT>;

        if constexpr (std::is_const_v<SrcT>)
        {
          checkSrcIsPreserved();
        }

        View<NonConstSrcT> srcNonConst{const_cast<PlanarComplex<NonConstSrcT>* const*>(src.data()), src.size()};

        checkBufferCount(src.size(), dst.size());
        
        const bool isInPlace = std::equal(src.begin(), src.end(), dst.begin(), [](const auto& s, const auto& d)
        {
          return reinterpret_cast<std::uintptr_t>(s.real) == reinterpret_cast<std::uintptr_t>(d);
        });

        checkPlacement((isInPlace) ? Placement::inPlace : Placement::outOfPlace);

        if constexpr (!std::is_void_v<SrcT> && !std::is_void_v<DstT>)
        {
          checkExecTypeProps(typePrecision<SrcT>, typeComplexity<SrcT>, typePrecision<DstT>, typeComplexity<DstT>);
        }

        executeImpl2(srcNonConst, dst, execParams);
      }

      /**
       * @brief Level 1 implementation of the execute method.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT>
      void executeImpl1(View<SrcT*> src, View<PlanarComplex<DstT>> dst, const ExecParamsT& execParams)
      {
        static_assert((std::is_void_v<SrcT> && std::is_void_v<DstT>) ||
                      (!std::is_void_v<SrcT> && !std::is_void_v<DstT>), "invalid source and destination types");

        using NonConstSrcT = std::remove_const_t<SrcT>;

        if constexpr (std::is_const_v<SrcT>)
        {
          checkSrcIsPreserved();
        }

        View<NonConstSrcT> srcNonConst{const_cast<NonConstSrcT* const*>(src.data()), src.size()};

        checkBufferCount(src.size(), dst.size());
        
        const bool isInPlace = std::equal(src.begin(), src.end(), dst.begin(), [](const auto& s, const auto& d)
        {
          return reinterpret_cast<std::uintptr_t>(s) == reinterpret_cast<std::uintptr_t>(d.real);
        });

        checkPlacement((isInPlace) ? Placement::inPlace : Placement::outOfPlace);

        if constexpr (!std::is_void_v<SrcT> && !std::is_void_v<DstT>)
        {
          checkExecTypeProps(typePrecision<SrcT>, typeComplexity<SrcT>, typePrecision<DstT>, typeComplexity<DstT>);
        }

        executeImpl2(srcNonConst, dst, execParams);
      }

      /**
       * @brief Level 1 implementation of the execute method.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT>
      void executeImpl1(View<PlanarComplex<SrcT>> src, View<PlanarComplex<DstT>> dst, const ExecParamsT& execParams)
      {
        static_assert((std::is_void_v<SrcT> && std::is_void_v<DstT>) ||
                      (!std::is_void_v<SrcT> && !std::is_void_v<DstT>), "invalid source and destination types");

        using NonConstSrcT = std::remove_const_t<SrcT>;

        if constexpr (std::is_const_v<SrcT>)
        {
          checkSrcIsPreserved();
        }

        View<NonConstSrcT> srcNonConst{const_cast<PlanarComplex<NonConstSrcT>* const*>(src.data()), src.size()};

        checkBufferCount(src.size(), dst.size());
        
        const bool isInPlace = std::equal(src.begin(), src.end(), dst.begin(), [](const auto& s, const auto& d)
        {
          return reinterpret_cast<std::uintptr_t>(s.real) == reinterpret_cast<std::uintptr_t>(d.real) &&
                 reinterpret_cast<std::uintptr_t>(s.imag) == reinterpret_cast<std::uintptr_t>(d.imag);
        });

        checkPlacement((isInPlace) ? Placement::inPlace : Placement::outOfPlace);

        if constexpr (!std::is_void_v<SrcT> && !std::is_void_v<DstT>)
        {
          checkExecTypeProps(typePrecision<SrcT>, typeComplexity<SrcT>, typePrecision<DstT>, typeComplexity<DstT>);
        }

        executeImpl2(srcNonConst, dst, execParams);
      }

      /**
       * @brief Level 2 implementation of the execute method.
       * @tparam SrcT Source type.
       * @tparam DstT Destination type.
       * @tparam ExecParamsT Execution parameters type.
       * @param src Source buffers.
       * @param dst Destination buffers.
       * @param execParams Execution parameters.
       */
      template<typename SrcT, typename DstT, typename ExecParamsT>
      void executeImpl2(View<SrcT> src, View<DstT> dst, const ExecParamsT& execParams)
      {
        auto isNullPtr = [](const auto* ptr) { return ptr == nullptr; };

        if (std::any_of(src.begin(), src.end(), isNullPtr))
        {
          throw std::invalid_argument("a null pointer was passed as source buffer");
        }

        if (std::any_of(dst.begin(), dst.end(), isNullPtr))
        {
          throw std::invalid_argument("a null pointer was passed as destination buffer");
        }

        View<void*> srcVoid{reinterpret_cast<void* const*>(src.data()), src.size()};
        View<void*> dstVoid{reinterpret_cast<void* const*>(dst.data()), dst.size()};

        if constexpr (std::is_same_v<ExecParamsT, DefaultExecParams>)
        {
          switch (getTarget())
          {
          case Target::cpu:
            switch (getDistribution())
            {
            case Distribution::spst:
              executeBackendImpl(srcVoid, dstVoid, afft::spst::cpu::ExecutionParameters{});
              break;
            case Distribution::mpst:
              executeBackendImpl(srcVoid, dstVoid, afft::mpst::cpu::ExecutionParameters{});
              break;
            default:
              detail::cxx::unreachable();
            }
            break;
          case Target::gpu:
            switch (getDistribution())
            {
            case Distribution::spst:
              executeBackendImpl(srcVoid, dstVoid, afft::spst::gpu::ExecutionParameters{});
              break;
            case Distribution::spmt:
              executeBackendImpl(srcVoid, dstVoid, afft::spmt::gpu::ExecutionParameters{});
              break;
            case Distribution::mpst:
              executeBackendImpl(srcVoid, dstVoid, afft::mpst::gpu::ExecutionParameters{});
              break;
            default:
              detail::cxx::unreachable();
            }
            break;
          default:
            detail::cxx::unreachable();
          }
        }
        else
        {
          if (execParams.target != getTarget())
          {
            throw std::invalid_argument("execution parameters target does not match plan target");
          }

          if (execParams.distribution != getDistribution())
          {
            throw std::invalid_argument("execution parameters distribution does not match plan distribution");
          }

          executeBackendImpl(srcVoid, dstVoid, execParams);
        }
      }
  };
} // namespace afft

#endif /* AFFT_PLAN_HPP */
