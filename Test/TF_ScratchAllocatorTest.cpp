#include "TF_TestMain.h"
#include "Forge/Core/Mem/TF_Allocators.h"
#include "utest.h"

UTEST(Core, scratchAlloc_1)
{
  struct TFScratchAllocator testAlloc = {0};
  struct TFScratchAllocDesc desc = {
    .blockSize = 1024,
    .alignment = 16
  };
  tfAddScratchAllocator(&testAlloc, &desc);
  tfScratchAlloc(&testAlloc, 128);
  tfScratchAlloc(&testAlloc, 128);
  tfScratchAlloc(&testAlloc, 128);
  tfScratchAlloc(&testAlloc, 128);
  tfScratchAlloc(&testAlloc, 128);
  tfScratchAlloc(&testAlloc, 128);
  tfScratchAlloc(&testAlloc, 128);
  tfScratchAlloc(&testAlloc, 128);
  tfFreeScratchAllocator(&testAlloc);
}


UTEST(TF, scratchAlloc_2)
{
  struct TFScratchAllocator testAlloc = {0};
  struct TFScratchAllocDesc desc = {
    .blockSize = 1024,
    .alignment = 16
  };
  tfAddScratchAllocator(&testAlloc, &desc);
  tfScratchAlloc(&testAlloc, 128);
  tfScratchAlloc(&testAlloc, 128);
  tfScratchAlloc(&testAlloc, 128);
  void* data = tfScratchAlloc(&testAlloc, 2048);
  EXPECT_EQ(((size_t)data) % 16, 0);
  tfFreeScratchAllocator(&testAlloc);
}


#include "Common_3/Utilities/Interfaces/IFileSystem.h"
#include "Common_3/Utilities/Interfaces/IMemory.h"
#include "Common_3/Utilities/Interfaces/ILog.h"
UTEST_STATE();
TF_UTEST_MAIN("TF_ScratchAllocatorTest")


