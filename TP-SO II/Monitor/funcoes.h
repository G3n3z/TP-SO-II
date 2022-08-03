#pragma once

#include <windows.h>

#ifdef SO2F3DLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif


DLL_IMP_API void toLower(TCHAR* string);

DLL_IMP_API void concatena(TCHAR* dest, TCHAR* source, int tam, int inicio, int tamDest);

DLL_IMP_API void copia(TCHAR* dest, TCHAR* source, int tam);

DLL_IMP_API BOOL retiraArgs(TCHAR* comando, struct MSGbufferCircular* stMsg, int numWords, int numElements);