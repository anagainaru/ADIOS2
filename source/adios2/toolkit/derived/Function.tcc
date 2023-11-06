#ifndef ADIOS2_DERIVED_Function_TCC_
#define ADIOS2_DERIVED_Function_TCC_

#include "Function.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <numeric>

namespace adios2
{
namespace derived
{

template <class T>
T *ApplyOneToOne(std::vector<DerivedData> inputData, size_t dataSize,
                 std::function<T(T, T)> compFct)
{
    T *outValues = (T *)malloc(dataSize * sizeof(T));
    if (outValues == nullptr)
    {
        std::cout << "Allocation failed for the derived data" << std::endl;
        // TODO - throw an exception
    }
    memset(outValues, 0, dataSize * sizeof(T));
    for (auto &variable : inputData)
    {
        for (size_t i = 0; i < dataSize; i++)
        {
            T data = *(reinterpret_cast<T *>(variable.Data) + i);
            outValues[i] = compFct(outValues[i], data);
        }
    }
    return outValues;
}

template <class T>
T * GScompute_pdf(const std::vector<T> &data, const size_t slice_size, const size_t start,
                  const size_t count, const size_t nbins, const T min, const T max)
{
    T *pdf = (T *) malloc(count * nbins * sizeof(T));
    memset(pdf, 0, count * nbins * sizeof(T));
    std::vector<T> bins(nbins);

    T binWidth = static_cast<T>((max - min) / static_cast<double>(nbins));
    for (size_t i = 0; i < nbins; ++i)
    {
        bins[i] = static_cast<T>(min + (static_cast<T>(i) * binWidth));
    }
    if (nbins == 1)
    {
        // special case: only one bin
        for (size_t i = 0; i < count; ++i)
        {
            pdf[i] = static_cast<T>(slice_size);
        }
        return pdf;
    }

    if (((max - min)<1.0e-20) || (binWidth<1.0e-20))
    {
        // special case: constant array
        for (size_t i = 0; i < count; ++i)
        {
            pdf[i * nbins + (nbins / 2)] = static_cast<T>(slice_size);
        }
        return pdf;
    }
    size_t start_data = 0;
    size_t start_pdf = 0;
    for (size_t i = 0; i < count; ++i)
    {
        // Calculate a PDF for 'nbins' bins for values between 'min' and 'max'
        // from data[ start_data .. start_data+slice_size-1 ]
        // into pdf[ start_pdf .. start_pdf+nbins-1 ]
        for (size_t j = 0; j < slice_size; ++j)
        {
            if (data[start_data + j] > max || data[start_data + j] < min)
            {
                std::cout << " data[" << start * slice_size + start_data + j
                          << "] = " << data[start_data + j] << " is out of [min,max] = [" << min
                          << "," << max << "]" << std::endl;
                continue;
            }
            size_t bin = static_cast<size_t>(std::floor((data[start_data + j] - min) / binWidth));
            if (bin == nbins)
            {
                bin = nbins - 1;
            }
            ++pdf[start_pdf + bin];
        }
        start_pdf += nbins;
        start_data += slice_size;
    }
    return pdf;
}

}
}
#endif
