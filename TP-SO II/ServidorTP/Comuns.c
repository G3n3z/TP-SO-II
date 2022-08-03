#include "ServidorFuncoes.h"


/*================================Funcao que testa a condicao de saida======================================*/
BOOL sair(dadosThreadJogador* dadosT) {
    WaitForSingleObject(dadosT->mutexSair, INFINITE);
    if (*dadosT->flagSair) {
        ReleaseMutex(dadosT->mutexSair);
        return TRUE;
    }
    ReleaseMutex(dadosT->mutexSair);
    return FALSE;
}
BOOL ganhou(dadosThreadJogador* dadosT) {
    WaitForSingleObject(dadosT->mutexStruct, INFINITE);
    if (*dadosT->ganhou) {
        ReleaseMutex(dadosT->mutexStruct);
        return TRUE;
    }
    ReleaseMutex(dadosT->mutexStruct);
    return FALSE;
}
BOOL perdeu(dadosThreadJogador* dadosT) {
    WaitForSingleObject(dadosT->mutexStruct, INFINITE);
    if (*dadosT->perdeu) {
        ReleaseMutex(dadosT->mutexStruct);
        return TRUE;
    }
    ReleaseMutex(dadosT->mutexStruct);
    return FALSE;
}