/* 
 * File:   ug_qa.h
 * Author: Garthower
 *
 * Created on 23 лютого 2017, 13:32
 */

#ifndef UG_QA_H
#define UG_QA_H

#ifdef _WIN32
#define UG_QA_IMPORT __declspec(dllexport) __stdcall
#else
#define UG_QA_IMPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

UG_QA_IMPORT float Version();
UG_QA_IMPORT const char* VersionText();
UG_QA_IMPORT void Deinitialize();
UG_QA_IMPORT int Initialize(const void* initData, size_t size);
UG_QA_IMPORT int DecodeQRMemory(const void* data, uint32_t size, void** answer, uint32_t* answerSize);
UG_QA_IMPORT int DecodeQR(const void* data, uint32_t size, void** answer, uint32_t* answerSize);
UG_QA_IMPORT int AnalysImageMemory(const void* data, uint32_t size, void** answer, uint32_t* answerSize);
UG_QA_IMPORT int AnalysImage(const void* data, uint32_t size, void** answer, uint32_t* answerSize);

#ifdef __cplusplus
}
#endif

#endif /* UG_QA_H */
