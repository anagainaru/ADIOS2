#ifndef ADIOS2_DERIVED_Function_H_
#define ADIOS2_DERIVED_Function_H_

#include "ExprHelper.h"
#include "adios2/common/ADIOSTypes.h"
#include "adios2/helper/adiosLog.h"
#include <functional>

namespace adios2
{
namespace derived
{

struct DerivedData
{
    void *Data;
    Dims Start;
    Dims Count;
};

struct OperatorFunctions
{
    std::function<DerivedData(std::vector<DerivedData>, DataType, double)> ComputeFct;
    std::function<Dims(std::vector<Dims>, double)> DimsFct;
};

DerivedData AddFunc(std::vector<DerivedData> input, DataType type, double constant);
DerivedData MagnitudeFunc(std::vector<DerivedData> input, DataType type, double constant);
DerivedData PDFFunc(std::vector<DerivedData> input, DataType type, double numBins);

Dims SameDimsFunc(std::vector<Dims> input, double constant);
Dims FixSizeFunc(std::vector<Dims> input, double constant);

const std::map<adios2::detail::ExpressionOperator, OperatorFunctions> OpFunctions = {
    {adios2::detail::ExpressionOperator::OP_ADD, {AddFunc, SameDimsFunc}},
    {adios2::detail::ExpressionOperator::OP_MAGN, {MagnitudeFunc, SameDimsFunc}},
    {adios2::detail::ExpressionOperator::OP_CURL, {PDFFunc, FixSizeFunc}},
    {adios2::detail::ExpressionOperator::OP_PDF, {PDFFunc, FixSizeFunc}}};

template <class T>
T *ApplyOneToOne(std::vector<DerivedData> inputData, size_t dataSize,
                 std::function<T(T, T)> compFct);

template <class T>
T * GScompute_pdf(const std::vector<T> &data, const size_t slice_size, const size_t count,
                  const size_t nbins, const T min, const T max);
}
}
#endif
