#pragma once

extern "C" int AnalyseGPE(const void* image, int imageSize, int extrnWhite, int blackBorder, int internWhite, int GPESize,
                          int seed1, int seed2, int seed3, int seed4, int localShiftsCount, int localShiftSize,
                          float RefPointsCountPercent, float NoiceCountPercent, const void* rsMemory, int rsMemorySize,
                          void** resultMemory, int* resultMemorySize);

extern "C" int GenerateGPE(int qrUnitSize, const char* qrData, int extrnWhite, int blackBorder, int internWhite, int GPESize,
                           int seed1, int seed2, int seed3, int seed4, const char* gpeData, int localShiftsCount, int localShiftSize,
                           float RefPointsCountPercent, float NoiceCountPercent, int DPI,
                           void** gpeMemory, int* gpememorySize, void** rsMemory, int* rsMemorySize);