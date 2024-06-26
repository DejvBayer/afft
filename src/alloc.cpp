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

#include <afft/afft.h>
#include <afft/afft.hpp>

/**
 * @brief Allocate aligned memory.
 * @param sizeInBytes Size of the memory block in bytes.
 * @param alignment Alignment of the memory block.
 * @return Pointer to the allocated memory block or NULL if the allocation failed.
 */
extern "C" void* afft_cpu_alignedAlloc(size_t sizeInBytes, afft_Alignment alignment)
{
  return ::operator new[](sizeInBytes, static_cast<std::align_val_t>(alignment), std::nothrow);
}

/**
 * @brief Free aligned memory.
 * @param ptr Pointer to the memory block.
 */
extern "C" void afft_cpu_alignedFree(void* ptr, afft_Alignment alignment)
{
  ::operator delete[](ptr, static_cast<std::align_val_t>(alignment), std::nothrow);
}

#if defined(AFFT_ENABLE_CUDA) || defined(AFFT_ENABLE_HIP)
/**
 * @brief Allocate unified memory.
 * @param sizeInBytes Size of the memory block in bytes.
 * @return Pointer to the allocated memory block or NULL if the allocation failed.
 */
extern "C" void* afft_gpu_unifiedAlloc(size_t sizeInBytes)
{
  void* ptr;

#if defined(AFFT_ENABLE_CUDA)
  if (cudaMallocManaged(&ptr, sizeInBytes) == cudaSuccess)
  {
    return ptr;
  }
#elif defined(AFFT_ENABLE_HIP)
  if (hipMallocManaged(&ptr, sizeInBytes) == hipSuccess)
  {
    return ptr;
  }
#endif
  
  return nullptr;
}

/**
 * @brief Free unified memory.
 * @param ptr Pointer to the memory block.
 */
extern "C" void afft_gpu_unifiedFree(void* ptr)
{
#if defined(AFFT_ENABLE_CUDA)
  cudaFree(ptr);
#elif defined(AFFT_ENABLE_HIP)
  hipFree(ptr);
#endif
}
#elif defined(AFFT_ENABLE_OPENCL)
/**
 * @brief Allocate unified memory.
 * @param sizeInBytes Size of the memory block in bytes.
 * @param context OpenCL context.
 * @return Pointer to the allocated memory block or NULL if the allocation failed.
 */
extern "C" void* afft_gpu_unifiedAlloc(size_t sizeInBytes, cl_context context)
{
  return clSVMAlloc(context, CL_MEM_READ_WRITE, sizeInBytes, 0);
}

/**
 * @brief Free unified memory.
 * @param ptr Pointer to the memory block.
 * @param context OpenCL context.
 */
extern "C" void afft_gpu_unifiedFree(void* ptr, cl_context context)
{
  clSVMFree(context, ptr);
}
#endif
