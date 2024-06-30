#include "TF_TestMain.h"
#include "Common_3/Application/Config.h"
#include "Forge/Core/TF_String.h"
#include "Forge/Graphics/TF_GPUConfig.h"
#include "utest.h"

UTEST(Core, loadData_0)
{
  struct GPUConfiguration def = {0};
  tfInitGPUConfiguration(&def);

  bool res = tfLoadGPUData(&def, tfToRef(
    "version:0.1"
  ));
  EXPECT_EQ(res, false); 
  tfFreeGPUConfiguration(&def);
}

UTEST(Core, loadData_1)
{
  struct GPUConfiguration def = {0};
  tfInitGPUConfiguration(&def);

  bool res = tfLoadGPUData(&def, tfToRef(
    "version:0.2\r\n"
    "BEGIN_VENDOR_LIST;\r\n"
    "intel; 0x8086, 0x1004;\r\n"
    "amd; 0x1002;\r\n"
    "nvidia; 0x10de;\r\n"
    "qualcomm; 0x5143;\r\n"
    "END_VENDOR_LIST;\r\n"
  ));
  EXPECT_TRUE(res); 
  EXPECT_EQ(def.mVendorCount,4); 
  EXPECT_TRUE(tfStrEqual(def.mVendorDefinitions[0].mVendorName, tfToRef("intel")));
  EXPECT_TRUE(tfStrEqual(def.mVendorDefinitions[1].mVendorName, tfToRef("amd")));
  EXPECT_TRUE(tfStrEqual(def.mVendorDefinitions[2].mVendorName, tfToRef("nvidia")));
  EXPECT_TRUE(tfStrEqual(def.mVendorDefinitions[3].mVendorName, tfToRef("qualcomm")));

  EXPECT_EQ(def.mVendorDefinitions[0].mIdentifierCount, 2);
  EXPECT_EQ(def.mVendorDefinitions[0].mIdentifierArray[0], 0x8086);
  EXPECT_EQ(def.mVendorDefinitions[0].mIdentifierArray[1], 0x1004);

  EXPECT_EQ(def.mVendorDefinitions[1].mIdentifierCount, 1);
  EXPECT_EQ(def.mVendorDefinitions[1].mIdentifierArray[0], 0x1002);
  
  EXPECT_EQ(def.mVendorDefinitions[2].mIdentifierCount, 1);
  EXPECT_EQ(def.mVendorDefinitions[2].mIdentifierArray[0], 0x10de);
  
  EXPECT_EQ(def.mVendorDefinitions[3].mIdentifierCount, 1);
  EXPECT_EQ(def.mVendorDefinitions[3].mIdentifierArray[0], 0x5143);

  tfFreeGPUConfiguration(&def);
}

UTEST(Core, loadData_2)
{   
  struct GPUConfiguration def = {0};
  tfInitGPUConfiguration(&def);
  bool res = tfLoadGPUData(&def, tfToRef(
      "version: 0.2\n"
      "#Possible Classfications for Preset: ultra; high; medium; low; verylow; office\n"
      "#Sources:\n"
      "#https://technical.city/en/video/rating\n"
      "#https://raw.githubusercontent.com/GameTechDev/gpudetect/master/IntelGfx.cfg\n"
      "#https://vulkan.gpuinfo.org/api/v2/getreportlist.php\n"
      "\n"
      "BEGIN_DEFAULT_CONFIGURATION;\n"
      "#if the gpu doesn't exist in the GPU_LIST use this instead\n"
      "DefaultPresetLevel; low;\n"
      "END_DEFAULT_CONFIGURATION;\n"
      "\n"
      "BEGIN_VENDOR_LIST;\n"
      "intel; 0x8086;\n"
      "amd; 0x1002;\n"
      "nvidia; 0x10de;\n"
      "apple; 0x106b;\n"
      "END_VENDOR_LIST;\n"
      "\n"
      "#VendorID; DeviceID; Preset; #Device comments\n"
      "\n"
      "BEGIN_GPU_LIST;\n"
      "#intel\n"
      "0x8086; 0x0042; verylow; #Intel; Intel(R) HD Graphics - Core i3/i5 + Pentium G9650 Processors\n"
      "0x8086; 0x0046; verylow; #Intel; Intel(R) HD Graphics - Core i3/i5/i7 Mobile Processors\n"
      "0x8086; 0x0102; verylow; #Intel; Intel(R) HD Graphics 2000\n"
      "\n" 
      "0x106B; 0x0001; low; Apple; Apple M1\n"
      "0x106B; 0x0002; low; Apple; Apple M1 Pro\n"
      "0x106B; 0x0003; low; Apple; Apple M1 Max\n"
      "END_GPU_LIST;\n"
  ));
  
  EXPECT_EQ(def.mVendorCount,4); 
  EXPECT_TRUE(tfStrEqual(def.mVendorDefinitions[0].mVendorName, tfToRef("intel")));
  EXPECT_TRUE(tfStrEqual(def.mVendorDefinitions[1].mVendorName, tfToRef("amd")));
  EXPECT_TRUE(tfStrEqual(def.mVendorDefinitions[2].mVendorName, tfToRef("nvidia")));
  EXPECT_TRUE(tfStrEqual(def.mVendorDefinitions[3].mVendorName, tfToRef("apple")));

  EXPECT_EQ(def.mVendorDefinitions[0].mIdentifierCount, 1);
  EXPECT_EQ(def.mVendorDefinitions[0].mIdentifierArray[0], 0x8086);

  EXPECT_EQ(def.mVendorDefinitions[1].mIdentifierCount, 1);
  EXPECT_EQ(def.mVendorDefinitions[1].mIdentifierArray[0], 0x1002);
  
  EXPECT_EQ(def.mVendorDefinitions[2].mIdentifierCount, 1);
  EXPECT_EQ(def.mVendorDefinitions[2].mIdentifierArray[0], 0x10de);
  
  EXPECT_EQ(def.mVendorDefinitions[3].mIdentifierCount, 1);
  EXPECT_EQ(def.mVendorDefinitions[3].mIdentifierArray[0], 0x106b);

  EXPECT_EQ(arrlen(def.mGpuModels), 6);
  EXPECT_EQ(def.mGpuModels[0].mVendorId, 0x8086);
  EXPECT_EQ(def.mGpuModels[0].mDeviceId, 0x0042);

  tfFreeGPUConfiguration(&def);
}

#include "Common_3/Utilities/Interfaces/IFileSystem.h"
#include "Common_3/Utilities/Interfaces/IMemory.h"
#include "Common_3/Utilities/Interfaces/ILog.h"
UTEST_STATE();
TF_UTEST_MAIN("TF_StringTest")


