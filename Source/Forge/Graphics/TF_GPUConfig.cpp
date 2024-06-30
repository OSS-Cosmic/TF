#include "Forge/Graphics/TF_GPUConfig.h"
#include "Forge/Core/TF_String.h"

#define GPUCFG_VERSION_MAJOR           0
#define GPUCFG_VERSION_MINOR           2

static GPUConfigExprSymbol getOpSymbol(struct TStrSpan input);
static bool parseGPUExpression(struct TFScratchAllocator* alloc,struct GPUConfigExpression* expr, TStrSpan input); 
struct TFStrGpuConfigIterable {
  const struct TStrSpan buffer; // the buffer to iterrate over
  size_t cursor; // the current position in the buffer
};



static struct TStrSpan tfGPUConfigIter(struct TFStrGpuConfigIterable* iterable)
{
    if (iterable->cursor == iterable->buffer.len)
        return (struct TStrSpan){ 0 };

    size_t cursor = iterable->cursor;
    for (; cursor < iterable->buffer.len; cursor++)
    {
        if (iterable->buffer.buf[cursor] == '\n' || iterable->buffer.buf[cursor] == '\r')
        {
            struct TStrSpan res = tfSub(iterable->buffer, iterable->cursor, cursor);
            const int       leaderOffset = tfStrIndexOfOffset(res, iterable->cursor, tfToRef("#"));
            if (leaderOffset > 0)
            {
                res = tfSub(res, 0, leaderOffset);
            }
            res = tfStrTrim(res);
            while (cursor < iterable->buffer.len)
            {
                if (iterable->buffer.buf[cursor] == '\n' || iterable->buffer.buf[cursor] == '\r')
                {
                    cursor++;
                    continue;
                }
                break;
            }
            iterable->cursor = cursor;
            return res;
        }
    }

    return (struct TStrSpan){ 0 };
}

void tfInitGPUConfiguration(struct GPUConfiguration* config) {
    struct TFScratchAllocDesc allocDesc = {
      .blockSize = 8192 
    };
    tfAddScratchAllocator(&config->mAlloc, &allocDesc);
}

void tfFreeGPUConfiguration(struct GPUConfiguration* config) {
  tfFreeScratchAllocator(&config->mAlloc);
  arrfree(config->mGpuModels);
}

static GPUPresetLevel strToPresetLevel(TStrSpan input)
{
    if (tfStrCaselessEqual(input, tfToRef("office")))
        return GPU_PRESET_OFFICE;
    if (tfStrCaselessEqual(input, tfToRef("verylow")))
        return GPU_PRESET_VERYLOW;
    if (tfStrCaselessEqual(input, tfToRef("low")))
        return GPU_PRESET_LOW;
    if (tfStrCaselessEqual(input, tfToRef("medium")))
        return GPU_PRESET_MEDIUM;
    if (tfStrCaselessEqual(input, tfToRef("high")))
        return GPU_PRESET_HIGH;
    if (tfStrCaselessEqual(input, tfToRef("ultra")))
        return GPU_PRESET_ULTRA;
    return GPU_PRESET_NONE;
}


static bool isTermToken(const char token) {
    return isalnum(token) || token == '.';
}


static bool isOpRuleToken(const char token) {
    switch (token)
    {
    case '<':
    case '>':
    case '=':
    case '!':
        return false;
    default:
        break;
    }
    return true;
}

static GPUConfigExprSymbol getOpSymbol(struct TStrSpan input) {
   if(tfStrEqual(input, tfToRef(">=")))
        return GPUConfigExprSymbol::GPUSymbolOpGTE;
   if(tfStrEqual(input, tfToRef(">")))
        return GPUConfigExprSymbol::GPUSymbolOpGT;
   if(tfStrEqual(input, tfToRef("<=")))
        return GPUConfigExprSymbol::GPUSymbolOpLTE;
   if(tfStrEqual(input, tfToRef(">")))
        return GPUConfigExprSymbol::GPUSymbolOpGT;
   if(tfStrEqual(input, tfToRef("<")))
        return GPUConfigExprSymbol::GPUSymbolOpLT;
   if(tfStrEqual(input, tfToRef("==")))
        return GPUConfigExprSymbol::GPUSymbolOpEQ;
   if(tfStrEqual(input, tfToRef("!=")))
        return GPUConfigExprSymbol::GPUSymbolOpNE;
    return GPUConfigExprSymbol::GPUSymbolNone;
}

static bool parseGPUTerm(struct TFScratchAllocator* alloc, struct GPUConfigTerm* term, TStrSpan input) {
    unsigned long long value = 0;
    memset(term, 0, sizeof(struct GPUConfigTerm));
    if (tfStrIndexOf(input, tfToRef(".")) > 0)
    {
        struct TFStrSplitIterable versionIter = { .buffer = input, .delim = tfToRef("."), .cursor = 0 };

        while (versionIter.cursor < versionIter.buffer.len)
        {
            struct TStrSpan verSpan = tfStrSplitIter(&versionIter);
            if (tfStrEmpty(verSpan))
                continue;
            if (!tfStrReadull(verSpan, &value))
                return false;

            term->mDriverVersion.versionNumbers[term->mDriverVersion.versionNumbersCount++] = value;
        }
        term->mSymbol = GPUConfigExprSymbol::GPUSymbolDriverVersion;
    }
    else if (tfStrReadull(input, &value))
    {
        term->mValue = value;
        term->mSymbol = GPUConfigExprSymbol::GPUSymbolDigit;
    }
    else
    {
        term->mSymbol = GPUConfigExprSymbol::GPUSymbolVariable;
        term->mVariable.buf = (char*)tfScratchAlloc(alloc, input.len);
        term->mVariable.len = input.len;
        memcpy(term->mVariable.buf, input.buf, input.len);
    }
    return true;
}


static bool parseGPUExpression(struct TFScratchAllocator* alloc,struct GPUConfigExpression* expr, TStrSpan input) {
    TStrSpan leftParam = { 0 };
    TStrSpan rightParam = { 0 };
    TStrSpan op = { 0 };

    TStrSpan consume = tfStrLTrim(input);
    size_t   cursor = 0;
    for (cursor = 0; cursor < consume.len; cursor++)
    {
        if (isspace(consume.buf[cursor]) || !isTermToken(consume.buf[cursor]))
            break;
        leftParam = tfSub(consume, 0, cursor);
    }

    consume = tfStrLTrim(tfSub(consume, 0, cursor));
    for (cursor = 0; cursor < consume.len; cursor++)
    {
        if (isspace(consume.buf[cursor]) || !isOpRuleToken(consume.buf[cursor]))
            break;
        op = tfSub(consume, 0, cursor);
    }

    consume = tfStrLTrim(tfSub(consume, 0, cursor));
    for (cursor = 0; cursor < consume.len; cursor++)
    {
        if (isspace(consume.buf[cursor]) || !isTermToken(consume.buf[cursor]))
            break;
        rightParam = tfSub(consume, 0, cursor);
    }
    consume = tfStrLTrim(tfSub(consume, 0, cursor));

    // still have characters left in the expression something is malformed
    if(!tfStrEmpty(consume))
        return false;


    if(!tfStrEmpty(leftParam) && !tfStrEmpty(op) && !tfStrEmpty(rightParam)) {
        GPUConfigExprSymbol operatorSym = getOpSymbol(op);
        if (operatorSym == GPUConfigExprSymbol::GPUSymbolNone)
            return false;
        expr->opToken = operatorSym;
        unsigned long long value = 0;
        if(!parseGPUTerm(alloc, &expr->mPrimary, leftParam))
            return false;
        if(!parseGPUTerm(alloc, &expr->mSecondary, leftParam))
            return false;
        return true;
    } else if(!tfStrEmpty(leftParam)) {
        if(!parseGPUTerm(alloc, &expr->mPrimary, leftParam))
            return false;
        return true;
    }
    return false;
}

bool tfLoadGPUConfig(struct GPUConfiguration* def, struct GPUConfiguration* config, TStrSpan input)
{
    TFStrGpuConfigIterable iterable = { .buffer = input, .cursor = 0 };
    TStrSpan               line = tfGPUConfigIter(&iterable);
    TStr tempLineMsg = { 0 };

    while (iterable.cursor < iterable.buffer.len)
    {
        line = tfGPUConfigIter(&iterable);

        if (tfStrIndexOf(line, tfToRef("BEGIN_GPU_SELECTION;")) >= 0)
        {
            while (iterable.cursor < iterable.buffer.len)
            {
                line = tfGPUConfigIter(&iterable);
                if (tfStrEmpty(line))
                    continue;
                if (tfStrIndexOf(line, tfToRef("END_GPU_SELECTION;")) >= 0)
                    break;
                
                if(def->mGpuSelectionCount >= TF_ARRAY_COUNT(def->mGPUSelection)) {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Max number of rules exausted '%d'.", TF_ARRAY_COUNT(def->mGPUSelection));
                    break;
                }
                
                struct TFStrSplitIterable tokenIterable = { .buffer = line, .delim = tfToRef(";"), .cursor = 0 };
                TStrSpan                  ruleSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));

                struct TFStrSplitIterable         ruleIterable = { .buffer = ruleSpan, .delim = tfToRef(","), .cursor = 0 };
                bool                              isValid = true;
                
                struct GPUConfigurationSelection* configurationSelection = &def->mGPUSelection[def->mGpuSelectionCount];
                configurationSelection->mComparisonExprCount = 0;

                while (ruleIterable.cursor < ruleIterable.buffer.len)
                {
                    struct TStrSpan exprSpan = tfStrTrim(tfStrSplitIter(&ruleIterable));
                    if(configurationSelection->mComparisonExprCount >= TF_ARRAY_COUNT(configurationSelection->mComparisonExpr)) {
                        isValid = false;
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Number of expressions is exausted %d.", TF_ARRAY_COUNT(configurationSelection->mComparisonExpr));
                        LOGF(eDEBUG, tempLineMsg.buf);
                        break;
                    }

                    if (!parseGPUExpression(&def->mAlloc, &configurationSelection->mComparisonExpr[configurationSelection->mComparisonExprCount], exprSpan))
                    {
                        isValid = false;
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Invalid GPU expression '%S'.", exprSpan);
                        LOGF(eDEBUG, tempLineMsg.buf);
                    }
                    else
                    {
                        configurationSelection->mComparisonExprCount++;
                    }
                }

                if (isValid)
                {
                    def->mGpuSelectionCount++;
                }
                else
                {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Invalid GPU selection rule '%S'.", line);
                    LOGF(eDEBUG, tempLineMsg.buf);
                }
            }
        }
        else if (tfStrIndexOf(line, tfToRef("BEGIN_DRIVER_REJECTION;")) >= 0)
        {
            while (iterable.cursor < iterable.buffer.len)
            {
                line = tfGPUConfigIter(&iterable);
                if (tfStrEmpty(line))
                    continue;
                if (tfStrIndexOf(line, tfToRef("END_DRIVER_REJECTION;")) >= 0)
                    break;

                if(def->mGpuRejectionCount >= TF_ARRAY_COUNT(def->mGPURejection)) {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Max number of rules exausted '%d'.", TF_ARRAY_COUNT(def->mGPUSelection));
                    break;
                }

                struct TFStrSplitIterable tokenIterable = { .buffer = line, .delim = tfToRef(";"), .cursor = 0 };
                TStrSpan                  vendorSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                TStrSpan                  ruleParametersSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                TStrSpan                  reasonSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                long long                 vendorId = 0;
                bool                      isValid = true;
                
                struct GPUConfigurationRejection* rejection = &config->mGPURejection[config->mGpuRejectionCount];
                rejection->mComparisonExprCount = 0;

                if (!tfStrReadll(vendorSpan, &vendorId)) {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Invalid GPU rejection rule '%S'.", line);
                    LOGF(eDEBUG, tempLineMsg.buf);
                    continue;
                }

                struct TFStrSplitIterable ruleIterable = { .buffer = ruleParametersSpan, .delim = tfToRef(","), .cursor = 0 };
                while (ruleIterable.cursor < ruleIterable.buffer.len)
                {
                    struct TStrSpan exprSpan = tfStrTrim(tfStrSplitIter(&ruleIterable));
                    
                    if(rejection->mComparisonExprCount >= TF_ARRAY_COUNT(rejection->mComparisonExpr)) {
                        isValid = false;
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Number of expressions is exausted %d.", TF_ARRAY_COUNT(rejection->mComparisonExpr));
                        LOGF(eDEBUG, tempLineMsg.buf);
                        break;
                    }
                    

                    if (!parseGPUExpression(&def->mAlloc, &rejection->mComparisonExpr[rejection->mComparisonExprCount], exprSpan)) {
                        isValid = false;
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Invalid GPU expression '%S'.", exprSpan);
                        LOGF(eDEBUG, tempLineMsg.buf);
                    }   
                    else
                    {
                        rejection->mComparisonExprCount++;
                    }
                }

                if (!isValid)
                {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Invalid GPU rejection rule '%S'.", line);
                    LOGF(eDEBUG, tempLineMsg.buf);
                    continue;
                }
                config->mGpuRejectionCount++;
                rejection->mVendorID = vendorId;
                rejection->mReasonStr.buf = (char*)tfScratchAlloc(&config->mAlloc, reasonSpan.len);
                rejection->mReasonStr.len = reasonSpan.len;
                memcpy(rejection->mReasonStr.buf, reasonSpan.buf, reasonSpan.len);
            }

        }
        else if (tfStrIndexOf(line, tfToRef("BEGIN_GPU_SETTINGS;")) >= 0)
        {
            while (iterable.cursor < iterable.buffer.len)
            {
                line = tfGPUConfigIter(&iterable);
                if (tfStrEmpty(line))
                    continue;
                if (tfStrIndexOf(line, tfToRef("END_GPU_SETTINGS;")) >= 0)
                    break;

                struct TFStrSplitIterable tokenIterable = { .buffer = line, .delim = tfToRef(";"), .cursor = 0 };
                TStrSpan                  propertySpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                TStrSpan                  ruleParametersSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                TStrSpan                  assignmentSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                bool                      isValid = true;
                long long                 assignmentValue = 0;

                if (!tfStrReadll(assignmentSpan, &assignmentValue ))
                {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Invalid GPU rejection rule '%S'.", line);
                    LOGF(eDEBUG, tempLineMsg.buf);
                    continue;
                }

                struct GPUConfigurationSetting* setting = &config->mConfigurationSetting[config->mGpuConfigurationSettingCount];

                struct TFStrSplitIterable ruleIterable = { .buffer = ruleParametersSpan, .delim = tfToRef(","), .cursor = 0 };
                while (ruleIterable.cursor < ruleIterable.buffer.len)
                {
                    struct TStrSpan exprSpan = tfStrTrim(tfStrSplitIter(&ruleIterable));
                    
                    if(setting->mComparisonExprCount >= TF_ARRAY_COUNT(setting->mComparisonExpr)) {
                        isValid = false;
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Number of expressions is exausted %d.", TF_ARRAY_COUNT(setting->mComparisonExpr));
                        LOGF(eDEBUG, tempLineMsg.buf);
                        break;
                    }
                    
                    if (!parseGPUExpression(&def->mAlloc, &setting->mComparisonExpr[setting->mComparisonExprCount], exprSpan)) {
                        isValid = false;
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Invalid GPU expression '%S'.", exprSpan);
                        LOGF(eDEBUG, tempLineMsg.buf);
                        break;
                    }   
                    setting->mComparisonExprCount++;
                }

                if (!isValid)
                {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Invalid GPU setting rule '%S'.", line);
                    LOGF(eDEBUG, tempLineMsg.buf);
                    continue;
                }

                config->mGpuConfigurationSettingCount++;
                setting->mAssignmentValue = assignmentValue;
                setting->mUpdateProperty.buf = (char*)tfScratchAlloc(&config->mAlloc, propertySpan.len);
                setting->mUpdateProperty.len = propertySpan.len;
                memcpy(setting->mUpdateProperty.buf, propertySpan.buf, propertySpan.len);
            }
        }
        else if (tfStrIndexOf(line, tfToRef("BEGIN_USER_SETTINGS;")) >= 0)
        {
            while (iterable.cursor < iterable.buffer.len)
            {
                line = tfGPUConfigIter(&iterable);
                if (tfStrEmpty(line))
                    continue;
                if (tfStrIndexOf(line, tfToRef("END_USER_SETTINGS;")) >= 0)
                    break;

                struct TFStrSplitIterable tokenIterable = { .buffer = line, .delim = tfToRef(";"), .cursor = 0 };
                TStrSpan                  propertySpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                TStrSpan                  ruleParametersSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                TStrSpan                  assignmentSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                bool                      isValid = true;
                long long                 assignmentValue = 0;

                if (!tfStrReadll(assignmentSpan, &assignmentValue ) || tfStrEmpty(ruleParametersSpan) || tfStrEmpty(propertySpan))
                {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Invalid GPU user setting rule '%S'.", line);
                    LOGF(eDEBUG, tempLineMsg.buf);
                    continue;
                }

                struct GPUConfigurationSetting* setting = &config->mConfigurationUserSettings[config->mGpuUserSettingCount];

                struct TFStrSplitIterable ruleIterable = { .buffer = ruleParametersSpan, .delim = tfToRef(","), .cursor = 0 };
                while (ruleIterable.cursor < ruleIterable.buffer.len)
                {
                    struct TStrSpan exprSpan = tfStrTrim(tfStrSplitIter(&ruleIterable));
                    
                    if(setting->mComparisonExprCount >= TF_ARRAY_COUNT(setting->mComparisonExpr)) {
                        isValid = false;
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Number of expressions is exausted %d.", TF_ARRAY_COUNT(setting->mComparisonExpr));
                        LOGF(eDEBUG, tempLineMsg.buf);
                        break;
                    }
                    
                    if (!parseGPUExpression(&def->mAlloc, &setting->mComparisonExpr[setting->mComparisonExprCount], exprSpan)) {
                        isValid = false;
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Invalid GPU expression '%S'.", exprSpan);
                        LOGF(eDEBUG, tempLineMsg.buf);
                        break;
                    }   
                    setting->mComparisonExprCount++;
                }

                if (!isValid)
                {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Invalid GPU user setting rule '%S'.", line);
                    LOGF(eDEBUG, tempLineMsg.buf);
                    continue;
                }

                config->mGpuConfigurationSettingCount++;
                setting->mAssignmentValue = assignmentValue;
                setting->mUpdateProperty.buf = (char*)tfScratchAlloc(&config->mAlloc, propertySpan.len);
                setting->mUpdateProperty.len = propertySpan.len;
                memcpy(setting->mUpdateProperty.buf, propertySpan.buf, propertySpan.len);
            }
        }
    }
    return true;
}


bool tfLoadGPUData(struct GPUConfiguration* config,TStrSpan input)
{
    TFStrGpuConfigIterable iterable = { .buffer = input, .cursor = 0 };

    TStrSpan line = tfGPUConfigIter(&iterable);
    {
        uint32_t versionMajor = 0;
        uint32_t versionMinor = 0;
        int      read = tfstrsscanf(line, "version:%u.%u", &versionMajor, &versionMinor);
        if (read != 2)
        {
            LOGF(eINFO, "Ill-formatted gpu.data file. Missing version at beginning of file");
            return false;
        }
        else if (versionMajor != GPUCFG_VERSION_MAJOR || versionMinor != GPUCFG_VERSION_MINOR)
        {
            LOGF(eINFO, "gpu.data version mismatch. Expected version %u.%u but got %u.%u", GPUCFG_VERSION_MAJOR, GPUCFG_VERSION_MINOR,
                 versionMajor, versionMinor);
            return false;
        }
    }
    TStr tempLineMsg = { 0 };

    while (iterable.cursor < iterable.buffer.len)
    {
        line = tfGPUConfigIter(&iterable);

        if (tfStrIndexOf(line, tfToRef("BEGIN_DEFAULT_CONFIGURATION;")) >= 0)
        {
            while (iterable.cursor < iterable.buffer.len)
            {
                line = tfGPUConfigIter(&iterable);
                if (tfStrEmpty(line))
                    continue;
                if (tfStrIndexOf(line, tfToRef("END_DEFAULT_CONFIGURATION;")) >= 0)
                    break;
                struct TFStrSplitIterable tokenIterable = { .buffer = line, .delim = tfToRef(";"), .cursor = 0 };
                TStrSpan                  ruleNameSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                TStrSpan                  assignmentSpan = tfStrTrim(tfStrSplitIter(&tokenIterable));
                if (tfStrIndexOf(ruleNameSpan, tfToRef("DefaultPresetLevel")) >= 0)
                {
                    GPUPresetLevel defaultLevel = strToPresetLevel(assignmentSpan);
                    if (defaultLevel != GPUPresetLevel::GPU_PRESET_NONE)
                    {
                        config->mDefaultPresetLevel = defaultLevel;
                    }
                    else
                    {
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Error invalid preset level in GPU Default Data Configuration value '%S' in '%S'.",
                                    ruleNameSpan, line);
                        LOGF(eDEBUG, tempLineMsg.buf);
                    }
                }
                else
                {
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Error could not parse GPU Default Data Configuration rule '%S' in '%S'.",
                                    ruleNameSpan, assignmentSpan);
                        LOGF(eDEBUG, tempLineMsg.buf);
                }
            }
        }
        else if (tfStrIndexOf(line, tfToRef("BEGIN_GPU_LIST;")) >= 0)
        {
            while (iterable.cursor < iterable.buffer.len)
            {
                line = tfGPUConfigIter(&iterable);
                if (tfStrEmpty(line))
                    continue;
                if (tfStrIndexOf(line, tfToRef("END_GPU_LIST;")) >= 0)
                    break;
                struct GPUModelDefinition modelDef = { 0 };
                struct TFStrSplitIterable iterable = { .buffer = line, .delim = tfToRef(";"), .cursor = 0 };
                TStrSpan                  vendorIdSpan = tfStrTrim(tfStrSplitIter(&iterable));
                TStrSpan                  modelIdSpan = tfStrTrim(tfStrSplitIter(&iterable));
                TStrSpan                  presetSpan = tfStrTrim(tfStrSplitIter(&iterable));
                TStrSpan                  vendorNameSpan = tfStrTrim(tfStrSplitIter(&iterable));
                TStrSpan                  modelNameSpan = tfStrTrim(tfStrSplitIter(&iterable));

                unsigned long long vendorId = 0;
                unsigned long long modelId = 0;
                if (tfStrReadull(vendorIdSpan, &vendorId))
                {
                    modelDef.mVendorId = vendorId;
                }
                if (tfStrReadull(modelIdSpan, &modelId))
                {
                    modelDef.mDeviceId = modelId;
                }
                modelDef.mPreset = strToPresetLevel(presetSpan);
                if (!tfStrEmpty(modelNameSpan))
                {
                    modelDef.mModelName.buf = (char*)tfScratchAlloc(&config->mAlloc, modelNameSpan.len);
                    modelDef.mModelName.len = modelNameSpan.len;
                    memcpy(modelDef.mModelName.buf, modelNameSpan.buf, modelNameSpan.len);
                }

                if (modelDef.mVendorId && modelDef.mDeviceId)
                {
                    arrpush(config->mGpuModels, modelDef);
                }
            }
        }
        else if (tfStrIndexOf(line, tfToRef("BEGIN_VENDOR_LIST;")) >= 0)
        {
            while (iterable.cursor < iterable.buffer.len)
            {
                line = tfGPUConfigIter(&iterable);
                if (tfStrEmpty(line))
                {
                    continue;
                }
                if (tfStrIndexOf(line, tfToRef("END_VENDOR_LIST;")) >= 0)
                {
                    break;
                }
                GPUVendorDefinition* vendorDef = &config->mVendorDefinitions[config->mVendorCount];
                memset(vendorDef, 0, sizeof(GPUVendorDefinition));

                struct TFStrSplitIterable iterable = { .buffer = line, .delim = tfToRef(";"), .cursor = 0 };
                TStrSpan                  vendorSpan = tfStrSplitIter(&iterable);
                TStrSpan                  identiferSpan = tfStrSplitIter(&iterable);
                struct TFStrSplitIterable idIterable = { .buffer = identiferSpan, .delim = tfToRef(","), .cursor = 0 };
                while (idIterable.cursor < idIterable.buffer.len)
                {
                    TStrSpan           idSpan = tfStrTrim(tfStrSplitIter(&idIterable));
                    unsigned long long vendorID = 0;
                    if (vendorDef->mIdentifierCount >= MAX_INDENTIFIER_PER_GPU_VENDOR_COUNT)
                    {
                        // we've ran out of vendor identifiers
                        vendorDef->mIdentifierCount = 0;
                        break;
                    }

                    if (tfStrReadull(idSpan, &vendorID))
                    {
                        vendorDef->mIdentifierArray[vendorDef->mIdentifierCount++] = vendorID;
                    }
                    else
                    {
                        tfStrClear(&tempLineMsg);
                        tfstrcatfmt(&tempLineMsg, "Invalid GPU vendor identifier %S from line: '%S'.", vendorSpan, identiferSpan);
                        LOGF(eDEBUG, tempLineMsg.buf);
                        break;
                    }
                }

                if (vendorDef->mIdentifierCount > 0 && !tfStrEmpty(tfStrTrim(vendorSpan)))
                {
                    vendorDef->mVendorName.buf = (char*)tfScratchAlloc(&config->mAlloc, vendorSpan.len);
                    vendorDef->mVendorName.len = vendorSpan.len;
                    memcpy(vendorDef->mVendorName.buf, vendorSpan.buf, vendorSpan.len);
                    config->mVendorCount++;
                }
                else
                {
                    tfStrClear(&tempLineMsg);
                    tfstrcatfmt(&tempLineMsg, "Error could not parse GPU vendor in '%S'.", line);
                    LOGF(eDEBUG, tempLineMsg.buf);
                }
            }
        }
    }

    // log default configuration
    LOGF(eINFO, "Default GPU Data:");
    LOGF(eINFO, "    DefaultGPUPresetLevel set to %s", presetLevelToString(config->mDefaultPresetLevel));
    // log gpu vendors
    LOGF(eINFO, "GPU vendors:");
    for(uint32_t vendorIdx = 0; vendorIdx < config->mVendorCount; vendorIdx++) {
        GPUVendorDefinition* vendorDef = &config->mVendorDefinitions[vendorIdx];
        tfStrClear(&tempLineMsg);
        tfstrcatfmt(&tempLineMsg, "%S: ", vendorDef->mVendorName);
        for (uint32_t identIdx = 0; identIdx < vendorDef->mIdentifierCount; identIdx++)
        {
            if (identIdx > 0)
            {
                tfstrcatfmt(&tempLineMsg, ", ");
            }
            tfstrcatprintf(&tempLineMsg, "%#x", vendorDef->mIdentifierArray[identIdx]);
        }
        LOGF(eINFO, tempLineMsg.buf);
    }
    tfStrFree(&tempLineMsg);
    return true;
}
