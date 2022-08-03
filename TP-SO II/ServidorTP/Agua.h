#pragma once
#include "ServidorFuncoes.h"

DWORD WINAPI colocaAgua(LPVOID d);
BOOL analisaTabColocaAgua(tabuleiros* tab, int nlinhas, int ncol, int* pPosX, int* pPosY, int posXfinal, int posYfinal);
TCHAR sameCharWithWater(TCHAR character);
BOOL jogadaValida(TCHAR charc, TCHAR newChar, int posX, int posY, int newPosX, int newPosY, const TCHAR characters[]);
BOOL isCharacters(TCHAR caracter, const TCHAR characters[]);