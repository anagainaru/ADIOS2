#------------------------------------------------------------------------------#
# Distributed under the OSI-approved Apache License, Version 2.0.  See
# accompanying file Copyright.txt for details.
#------------------------------------------------------------------------------#

set(CTEST_PROJECT_NAME "ADIOS")
set(CTEST_NIGHTLY_START_TIME "01:00:00 UTC")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "open.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=ADIOS")
set(CTEST_DROP_SITE_CDASH TRUE)
set(MEMORYCHECK_SUPPRESSIONS_FILE ${CMAKE_SOURCE_DIR}/scripts/dashboard/nightly/valgrind-suppressions.txt)

# Ignore tests that are currently failing, remove tests here as they are fixed
list(APPEND CTEST_CUSTOM_MEMCHECK_IGNORE 
Staging.TimeoutReader.1x1.CommMin.BP5.SST
Staging.1x1Struct.BP5
Staging.WriteMemorySelectionRead.1x1.CommMin.BP.SST
Staging.1x1.Local2.CommMin.BP.SST
Staging.OnDemandSingle.1x1.CommMin.BP5.SST
Staging.AllToAllDistribution.1x1x3.CommMin.BP5.SST
Staging.RoundRobinDistribution.1x1x3.CommMin.BP5.SST
Staging.1x1.LocalMultiblock.CommMin.BP5.SST
Staging.1x1.SstRUDP.CommMin.BP5.SST
Staging.WriteMemorySelectionRead.1x1.CommMin.BP5.SST
Staging.1x1LockGeometry.CommMin.BP5.SST
Staging.1x1.ForcePreload.CommMin.BP5.SST
Staging.1x1.NoPreload.CommMin.BP5.SST
Staging.DiscardWriter.1x1.CommMin.BP5.SST
Staging.LatestReaderHold.1x1.CommMin.BP5.SST
Staging.LatestReader.1x1.CommMin.BP5.SST
Engine.BP.BPWriteReadAsStreamTestADIOS2.ReaderWriterDefineVariable.BP5.Serial
Staging.ZFPCompression.3x5.CommMin.BP5.SST
Staging.ZFPCompression.1x1.CommMin.BP5.SST
Staging.1x1Joined.CommMin.BP5.SST
Staging.1x1Struct.CommMin.BP5.SST
Staging.1x1.Local2.CommMin.BP5.SST
Staging.1x1VarDestruction.CommMin.BP5.SST
Staging.1x1.ModAttrs.CommMin.BP5.SST
Staging.1x1.Attrs.CommMin.BP5.SST
Staging.1x1.CommMin.BP5.SST
Engine.SST.SstWriteFails.InvalidBeginStep.Serial
remoteServerCleanup
Remote.BPWriteMemorySelectionRead.FileRemote
Remote.BPWriteMemorySelectionRead.GetRemote
Remote.BPWriteReadADIOS2stdio.GetRemote
)
