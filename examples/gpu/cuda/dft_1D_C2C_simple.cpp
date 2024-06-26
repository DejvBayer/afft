#include <complex>
#include <vector>

#include <afft/afft.hpp>

template<typename T>
using UnifiedMemoryVector = std::vector<T, afft::gpu::UnifiedMemoryAllocator<T>>;

int main(void)
{
  using PrecT = float;

  constexpr std::size_t size{1024}; // size of the transform

  afft::init(); // initialize afft library, also initializes CUDA

  UnifiedMemoryVector<std::complex<PrecT>> src(size); // source vector
  UnifiedMemoryVector<std::complex<PrecT>> dst(size); // destination vector

  // initialize source vector

  afft::dft::Parameters dftParams{}; // parameters for dft
  dftParams.direction     = afft::Direction::forward; // it will be a forward transform
  dftParams.precision     = afft::makePrecision<PrecT>(); // set up precision of the transform
  dftParams.shape         = {{size}}; // set up the dimensions
  dftParams.type          = afft::dft::Type::complexToComplex; // let's use complex-to-complex transform

  afft::gpu::Parameters gpuParams{}; // it will run on a gpu
  gpuParams.preserveSource = false; // allow to destroy source data

  auto plan = afft::makePlan(dftParams, gpuParams); // generate the plan of the transform, uses current device

  plan->execute(src.data(), dst.data()); // execute the transform into zero stream

  if (cudaDeviceSynchronize() != cudaSuccess)
  {
    throw std::runtime_error("CUDA error: failed to synchronize");
  }

  // use results from dst vector
}
