#pragma once

#include <windows.h>
#include <tchar.h>

#ifdef DLLFUNCOES_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif


__declspec(dllexport) void copia(TCHAR* dest, TCHAR* source, int tam);

__declspec(dllexport) void concatena(TCHAR* dest, TCHAR* source, int tam, int inicio, int tamDest);

__declspec(dllexport) void toLower(TCHAR* string);
