#------------------------------------------------------------------------------#
# Distributed under the OSI-approved Apache License, Version 2.0.  See
# accompanying file Copyright.txt for details.
#------------------------------------------------------------------------------#

if(ADIOS2_HAVE_CUDA)
  enable_language(CUDA)
  add_executable(GPUWriteRead_cuda cudaWriteRead.cu)
  target_include_directories(GPUWriteRead_cuda PUBLIC ${CUDA_INCLUDE_DIRS})
  target_link_libraries(GPUWriteRead_cuda PUBLIC adios2::cxx11 ${CUDA_LIBRARIES})
  set_target_properties(GPUWriteRead_cuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()
