#pragma once

#ifdef _WIN32
#define GPE_UNIT_IMPORT __declspec(dllexport) __stdcall
#else
#define GPE_UNIT_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct AnalyseGPEResult_t{
    int status;
    char *GPEData;
    double score;
    double scoreAdv;
    uint32_t localShifts;
} __attribute__((aligned(1)));

GPE_UNIT_IMPORT const char* Version();

GPE_UNIT_IMPORT int
AnalyseGPE(const void *image, int imageSize, int extrnWhite, int blackBorder, int internWhite, int GPESize,
           int seed1, int seed2, int seed3, int seed4, int localShiftsCount, int localShiftSize,
           float RefPointsCountPercent, float NoiceCountPercent, const void *rsMemory, int rsMemorySize,
           void **resultMemory, int *resultMemorySize);

GPE_UNIT_IMPORT int
GenerateGPE(int qrUnitSize, const char *qrData, int extrnWhite, int blackBorder, int internWhite, int GPESize,
            int seed1, int seed2, int seed3, int seed4, const char *gpeData, int localShiftsCount, int localShiftSize,
            float RefPointsCountPercent, float NoiceCountPercent, int DPI,
            void **gpeMemory, int *gpememorySize, void **rsMemory, int *rsMemorySize);

#ifdef __cplusplus
}
#endif