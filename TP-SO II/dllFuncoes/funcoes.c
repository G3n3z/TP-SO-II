#include "funcoes.h"
#include <ctype.h>

void copia(TCHAR *dest, TCHAR* source, int tam) {
    for (int i = 0; i < tam; i++)
    {
        dest[i] = source[i];
    }
    dest[tam] = TEXT('\0');
}

void concatena(TCHAR* dest, TCHAR* source, int tam, int inicio, int tamDest) {
    int i, j;
    for (i = inicio, j = 0; j < tam && i < tamDest; i++, j++)
    {
        dest[i] = source[j];
    }
    dest[i] = TEXT('\0');
}

void toLower(TCHAR* string) {
    for (int i = 0; i < _tcslen(string); i++) {
        string[i] = tolower(string[i]);
    }
}

