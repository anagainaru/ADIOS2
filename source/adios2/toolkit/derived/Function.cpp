#ifndef ADIOS2_DERIVED_Function_CPP_
#define ADIOS2_DERIVED_Function_CPP_

#include "Function.h"
#include "Function.tcc"
#include "adios2/common/ADIOSMacros.h"
#include "adios2/helper/adiosFunctions.h"
#include <algorithm>
#include <cmath>

namespace adios2
{
namespace derived
{

DerivedData AddFunc(std::vector<DerivedData> inputData, DataType type, double constant)
{
    size_t dataSize = std::accumulate(std::begin(inputData[0].Count), std::end(inputData[0].Count),
                                      1, std::multiplies<size_t>());

#define declare_type_add(T)                                                                        \
    if (type == helper::GetDataType<T>())                                                          \
    {                                                                                              \
        T *addValues = ApplyOneToOne<T>(inputData, dataSize, [](T a, T b) { return a + b; });      \
        return DerivedData({(void *)addValues, inputData[0].Start, inputData[0].Count});           \
    }
    ADIOS2_FOREACH_ATTRIBUTE_PRIMITIVE_STDTYPE_1ARG(declare_type_add)
    helper::Throw<std::invalid_argument>("Derived", "Function", "AddFunc",
                                         "Invalid variable types");
    return DerivedData();
}

DerivedData MagnitudeFunc(std::vector<DerivedData> inputData, DataType type, double constant)
{
    size_t dataSize = std::accumulate(std::begin(inputData[0].Count), std::end(inputData[0].Count),
                                      1, std::multiplies<size_t>());
#define declare_type_mag(T)                                                                        \
    if (type == helper::GetDataType<T>())                                                          \
    {                                                                                              \
        T *magValues = ApplyOneToOne<T>(inputData, dataSize, [](T a, T b) { return a + b * b; });  \
        for (size_t i = 0; i < dataSize; i++)                                                      \
        {                                                                                          \
            magValues[i] = std::sqrt(magValues[i]);                                                \
        }                                                                                          \
        return DerivedData({(void *)magValues, inputData[0].Start, inputData[0].Count});           \
    }
    ADIOS2_FOREACH_ATTRIBUTE_PRIMITIVE_STDTYPE_1ARG(declare_type_mag)
    helper::Throw<std::invalid_argument>("Derived", "Function", "MagnitudeFunc",
                                         "Invalid variable types");
    return DerivedData();
}

DerivedData PDFFunc(std::vector<DerivedData> inputData, DataType type, double numBins)
{
    if (inputData.size() > 1)
        helper::Throw<std::invalid_argument>("Derived", "Function", "PDFFunc",
                                             "The PDF function can only take one varaible");

    if (numBins < 1)
        helper::Throw<std::invalid_argument>("Derived", "Function", "PDFFunc",
                                             "Number of bins needs to be greater than 1");
    if (inputData[0].Count.size() != 3)
        helper::Throw<std::invalid_argument>("Derived", "Function", "PDFFunc",
                                             "Shape is expected to be 3D");
    size_t dataSize = std::accumulate(std::begin(inputData[0].Count), std::end(inputData[0].Count),
                                      1, std::multiplies<size_t>());
#define declare_type_pdf(T)                                                                        \
    if (type == helper::GetDataType<T>())                                                          \
    {                                                                                              \
        std::vector<T> inputValues(static_cast<T *>(inputData[0].Data),                            \
                                   static_cast<T *>(inputData[0].Data) + dataSize);                \
        auto min_value = *std::min_element(inputValues.begin(), inputValues.end());                \
        auto max_value = *std::max_element(inputValues.begin(), inputValues.end());                \
        size_t slice_size = slice_size = inputData[0].Count[1] * inputData[0].Count[2];            \
        T *histogram = GScompute_pdf<T>(inputValues, slice_size, inputData[0].Start[0],            \
                              inputData[0].Count[0], numBins, min_value, max_value);               \
        return DerivedData({(void *)histogram,                                                     \
                            {inputData[0].Start[0], 0},                                            \
                            {inputData[0].Count[0], static_cast<size_t>(numBins)}});               \
    }
    ADIOS2_FOREACH_ATTRIBUTE_PRIMITIVE_STDTYPE_1ARG(declare_type_pdf)
    helper::Throw<std::invalid_argument>("Derived", "Function", "PDFFunc",
                                         "Invalid variable types");
    return DerivedData();
}

/*
DerivedData PDFFunc(std::vector<DerivedData> inputData, DataType type, double numBins)
{
    if (inputData.size() > 1)
        helper::Throw<std::invalid_argument>("Derived", "Function", "PDFFunc",
                                             "The PDF function can only take one varaible");

    if (numBins < 1)
        helper::Throw<std::invalid_argument>("Derived", "Function", "PDFFunc",
                                             "Number of bins needs to be greater than 1");
    size_t dataSize = std::accumulate(std::begin(inputData[0].Count), std::end(inputData[0].Count),
                                      1, std::multiplies<size_t>());
#define declare_type_pdf(T)                                                                        \
    if (type == helper::GetDataType<T>())                                                          \
    {                                                                                              \
        std::vector<T> inputValues(static_cast<T *>(inputData[0].Data),                            \
                                   static_cast<T *>(inputData[0].Data) + dataSize);                \
        auto min_value = *std::min_element(inputValues.begin(), inputValues.end());                \
        auto max_value = *std::max_element(inputValues.begin(), inputValues.end());                \
        float binWidth = static_cast<float>(max_value - min_value) / numBins;                      \
        T *histogram = (T *)malloc(numBins * sizeof(T));                                           \
        if (histogram == nullptr)                                                                  \
        {                                                                                          \
            std::cout << "Allocation failed for the derived data" << std::endl;                    \
        }                                                                                          \
        memset(histogram, 0, numBins * sizeof(T));                                                 \
        for (T value : inputValues)                                                                \
        {                                                                                          \
            size_t bin = std::floor((value - min_value) / binWidth);                               \
            if (bin >= numBins || bin < 0)                                                         \
                continue;                                                                          \
            histogram[bin]++;                                                                      \
        }                                                                                          \
        return DerivedData({(void *)histogram, {0}, {static_cast<size_t>(numBins)}});              \
    }
    ADIOS2_FOREACH_ATTRIBUTE_PRIMITIVE_STDTYPE_1ARG(declare_type_pdf)
    helper::Throw<std::invalid_argument>("Derived", "Function", "PDFFunc",
                                         "Invalid variable types");
    return DerivedData();
}
 */

/* variable list with the same dimensions {N1, N2, ..} will output a variable {constant} */
Dims FixSizeFunc(std::vector<Dims> input, double constant)
{
    Dims dim({input[0][0], static_cast<size_t>(constant)});
    return dim;
};

/* variable list with the same dimensions {N1, N2, ..} will output a variable {N1, N2, ..} */
Dims SameDimsFunc(std::vector<Dims> input, double contant)
{
    // check that all dimenstions are the same
    if (input.size() > 1)
    {
        bool dim_are_equal = std::equal(input.begin() + 1, input.end(), input.begin());
        if (!dim_are_equal)
            helper::Throw<std::invalid_argument>("Derived", "Function", "SameDimFunc",
                                                 "Invalid variable dimensions");
    }
    // return the first dimension
    return input[0];
}

#define declare_template_instantiation(T)                                                          \
    T *ApplyOneToOne(std::vector<DerivedData>, size_t, std::function<T(T, T)>);

ADIOS2_FOREACH_PRIMITIVE_STDTYPE_1ARG(declare_template_instantiation)
#undef declare_template_instantiation

}
} // namespace adios2
#endif
