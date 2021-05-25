/*
 * Distributed under the OSI-approved Apache License, Version 2.0.  See
 * accompanying file Copyright.txt for details.
 *
 * BP5Reader.tcc
 *
 *  Created on: Aug 1, 2018
 *      Author: Lipeng Wan wanl@ornl.gov
 */

#ifndef ADIOS2_ENGINE_BP5_BP5READER_TCC_
#define ADIOS2_ENGINE_BP5_BP5READER_TCC_

#include "BP5Reader.h"

#include "adios2/helper/adiosFunctions.h"

namespace adios2
{
namespace core
{
namespace engine
{

template <class T>
inline void BP5Reader::GetSyncCommon(Variable<T> &variable, T *data)
{
    bool need_sync = m_BP5Deserializer->QueueGet(variable, data);
    if (need_sync)
        PerformGets();
}

template <class T>
void BP5Reader::GetDeferredCommon(Variable<T> &variable, T *data)
{
    (void)m_BP5Deserializer->QueueGet(variable, data);
}

} // end namespace engine
} // end namespace core
} // end namespace adios2

#endif /* ADIOS2_ENGINE_BP5_BP5READER_TCC_ */
