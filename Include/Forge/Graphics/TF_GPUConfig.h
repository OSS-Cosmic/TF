
/* ------------------------ gpu.data ------------------------ */
// intel use 3 identifiers 0x163C, 0x8086, 0x8087;

#include "Forge/Core/TF_String.h"
#include "Forge/Core/Mem/TF_ScratchAllocator.h"

#include "Common_3/Graphics/Interfaces/IGraphics.h"

#define MAX_GPU_VENDOR_COUNT                 64
#define MAX_GPU_VENDOR_IDENTIFIER_LENGTH     16
#define MAX_INDENTIFIER_PER_GPU_VENDOR_COUNT 12 
#define MAX_GPU_COMPARISON_OP_COUNT 16 

enum class GPUConfigExprSymbol: uint8_t {
    GPUSymbolNone,
    
    // term
    GPUSymbolDigit,
    GPUSymbolVariable,

    // op
    GPUSymbolOpEQ,
    GPUSymbolOpNE,
    GPUSymbolOpLT,
    GPUSymbolOpGT,
    GPUSymbolOpLTE,
    GPUSymbolOpGTE,
};

// comparison expression to test against
struct GPUConfigExpression{
    GPUConfigExprSymbol mPrimaryToken;
    GPUConfigExprSymbol op; 
    GPUConfigExprSymbol mSecondaryToken;
    union {
        TStrSpan mVariable;
        uint64_t mValue;
    } mPrimary;
    union {
        TStrSpan mVariable;
        uint64_t mValue;
    } mSecondary;
};


struct GPUConfigurationSetting {
    TStrSpan mUpdateProperty; // the property this is refering to 
    uint32_t mComparisonExprCount;
    GPUConfigExpression mComparisonExpr[MAX_GPU_COMPARISON_OP_COUNT];
    uint64_t mAssignmentValue;
};

struct GPUConfigurationSelection {
    uint32_t mComparisonExprCount;
    GPUConfigExpression  mComparisonExpr[MAX_GPU_COMPARISON_OP_COUNT];

};

struct GPUVendorDefinition {
    TStrSpan mVendorName = {};
    uint32_t mIdentifierCount = 0;
    uint32_t mIdentifierArray[MAX_INDENTIFIER_PER_GPU_VENDOR_COUNT] = {};
};

struct GPUModelDefinition
{
    uint32_t       mVendorId;
    uint32_t       mDeviceId;
    GPUPresetLevel mPreset;
    TStrSpan mModelName;
};

struct GPUConfiguration {
  struct TFScratchAlloc mAlloc; // temporary scratch allocator for the lifetime of the configuration
    
  GPUPresetLevel mDefaultPresetLevel;

  struct GPUConfigurationSetting  mConfigurationUserSettings[256];
  struct GPUConfigurationSetting  mConfigurationSetting[256];
  struct GPUConfigurationSelection  mGPUSelection[256];

  // data.gpu
  uint32_t mVendorCount;
  GPUVendorDefinition mVendorDefinitions[MAX_GPU_VENDOR_COUNT]; 
  struct GPUModelDefinition* mGpuModels;
};

void tfInitGPUConfiguration(struct GPUConfiguration* def);
bool tfLoadGPUConfig(struct GPUConfiguration* config, TStrSpan input);
bool tfLoadGPUData(struct GPUConfiguration* config,TStrSpan input);
void tfFreeGPUConfiguration(struct GPUConfiguration* def);
