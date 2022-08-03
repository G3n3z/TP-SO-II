
#include "Agua.h"

/*================================Thread que coloca a agua======================================*/
DWORD WINAPI colocaAgua(LPVOID d) {

    dadosThreadJogador* dadosT = (dadosThreadJogador*)d;
    //dadosJogo dados;
    dadosServ2Cli dados;
    int numJogadores, nlinhas, ncol, posXfinal, posYfinal;
    int posX, posY;
    if (sair(dadosT)) {
        ExitThread(0);
    }
    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    //CopyMemory(&dados, dadosT->shr, sizeof(dadosJogo));
    int time = dadosT->shr->time;
    numJogadores = dadosT->shr->numJogadores;
    nlinhas = dadosT->shr->nLinhas;
    ncol = dadosT->shr->nColunas;
    ReleaseMutex(dadosT->mutexSHR);
    WaitForSingleObject(dadosT->mutexStruct, INFINITE);
    posX = *dadosT->posXini;
    posY = *dadosT->posYini;
    posXfinal = *dadosT->posXfinal;
    posYfinal = *dadosT->posYfinal;
    ReleaseMutex(dadosT->mutexStruct);
    int jogador = dadosT->numJogador;
    OVERLAPPED ov;
    DWORD nEscritos;
    int perde = 0, ganho = 0;

    while (TRUE) {
        SetEvent(dadosT->EventServ2Moni);
        ResetEvent(dadosT->EventServ2Moni);
        if (sair(dadosT)) {
            break;
        }
        //_tprintf(TEXT("\n\nAntes do waitTimer %d - %p\n"), jogador, dadosT->waitTimer);
        WaitForSingleObject(dadosT->waitTimer, INFINITE);
        //_tprintf(TEXT("Depois do waitTimer %d - time:  %d\n"), jogador, time);
         //_tprintf(TEXT("A volta\n"));
        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        if (sair(dadosT)) {
            break;
        }


        if (!analisaTabColocaAgua(dadosT->shr->jogador + jogador, nlinhas, ncol, &posX, &posY, posXfinal, posYfinal)) { // CHARACTER e WATER nao estao a ser usados do parametro
            WaitForSingleObject(dadosT->mutexStruct, INFINITE);
            if (*dadosT->perdeu == 0)
                *dadosT->perdeu = jogador + 1;
            perde = jogador + 1;
            ReleaseMutex(dadosT->mutexStruct);

        }
        else {
            WaitForSingleObject(dadosT->mutexStruct, INFINITE);
            dadosT->shr->jogador[jogador].pont += 5;
            if (posX == posXfinal && posY == posYfinal) {
                if (*dadosT->ganhou == 0)
                    *dadosT->ganhou = jogador + 1;
                ganho = jogador + 1;
            }
            ReleaseMutex(dadosT->mutexStruct);

        }

        ReleaseMutex(dadosT->mutexSHR);

        WaitForSingleObject(dadosT->mutexSHR, INFINITE);

        //_tprintf(TEXT("Enviei Evento %d - %p"), jogador, dadosT->EventServ2Moni);
        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        dadosT->shr->modoJogo = *dadosT->modoJogo;
        CopyMemory(&dados.dados, dadosT->shr, sizeof(dadosJogo));
        //_tprintf(TEXT("Aqui Na agua\n"));
        dados.val = AGUA;
        dados.jogador = jogador;
        dados.xAgua = posX;
        dados.yAgua = posY;
        if (!WriteFile(dadosT->namePipe, &dados, sizeof(dados), &nEscritos, &ov)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(ov.hEvent, 1000);
                GetOverlappedResult(dadosT->namePipe, &ov, &nEscritos, FALSE);
            }
        }
        ReleaseMutex(dadosT->mutexSHR);

        if (perdeu(dadosT)) {

            break;
        }
        else if (ganhou(dadosT)) {
            break;
        }
    }
    //_tprintf(TEXT("Libertei Mutex %d - %p"), jogador, dadosT->eventFinal);
    SetEvent(dadosT->EventServ2Moni);
    Sleep(50);
    ResetEvent(dadosT->EventServ2Moni);
    SetEvent(dadosT->eventFinal);
    //_tprintf(TEXT("[DEBUG]Vou sair da agua\n"));
    ExitThread(0);
}

/*================================Funcao que analisa as pocicoes com jogadas======================================*/

BOOL analisaTabColocaAgua(tabuleiros* tab, int nlinhas, int ncol, int* pPosX, int* pPosY, int posXfinal, int posYfinal) {
    int posicoes[8]; int indice = -1;
    int empty[2] = { -1,-1 };
    int posX = *pPosX;
    int posY = *pPosY;
    for (int i = 0; i < 8; i++)
    {
        posicoes[i] = -1;
    }

    if (posX - 1 >= 0) {
        if (isCharacters(tab->tab[posX - 1][posY], CHARACTERS)) {
            posicoes[++indice] = posX - 1;
            posicoes[++indice] = posY;
        }
        else if ((tab->tab[posX - 1][posY] == TEXT(' '))) {
            empty[0] = posX - 1; empty[1] = posY;
        }
    }
    if (posY + 1 < ncol) {
        if (isCharacters(tab->tab[posX][posY + 1], CHARACTERS)) {
            posicoes[++indice] = posX;
            posicoes[++indice] = posY + 1;
        }
        else if (empty[0] != -1 && (tab->tab[posX][posY + 1] == TEXT(' '))) {
            empty[0] = posX; empty[1] = posY + 1;
        }
    }
    if (posY - 1 >= 0) {
        if (isCharacters(tab->tab[posX][posY - 1], CHARACTERS)) {
            posicoes[++indice] = posX;
            posicoes[++indice] = posY - 1;
        }
        else if (empty[0] != -1 && (tab->tab[posX][posY - 1] == TEXT(' '))) {
            empty[0] = posX; empty[1] = posY - 1;
        }
    }
    if (posX + 1 < nlinhas) {
        if (isCharacters(tab->tab[posX + 1][posY], CHARACTERS)) {
            posicoes[++indice] = posX + 1;
            posicoes[++indice] = posY;
        }
        else if (empty[0] != -1 && (tab->tab[posX + 1][posY] == TEXT(' '))) {
            empty[0] = posX + 1; empty[1] = posY;
        }
    }
    int i = 0;
    for (i = 0; i < 8; i += 2) {
        if (posicoes[i] == -1) {
            break;
        }
        if (jogadaValida(tab->tab[posX][posY], tab->tab[posicoes[i]][posicoes[i + 1]], posX, posY, posicoes[i], posicoes[i + 1], CHARACTERS)) {
            posX = posicoes[i]; posY = posicoes[i + 1];
            tab->tab[posX][posY] = sameCharWithWater(tab->tab[posicoes[i]][posicoes[i + 1]]);
            *pPosX = posX; *pPosY = posY;
            return TRUE;
        }
    }
    if (i == 0) {
        tab->tab[posX][posY] = TEXT('X');
    }
    else {
        tab->tab[posX][posY] = TEXT('X');

    }
    return FALSE;
}

/*================================Funcao que devolve o caracter da agua correspondente======================================*/
TCHAR sameCharWithWater(TCHAR character) {
    int i;
    for (i = 0; i < TAMCHAR; i++)
    {
        if (character == CHARACTERS[i]) {
            break;
        }
    }
    return WATER[i];
}

/*================================Funcao que verifica se a jogada é valida======================================*/
BOOL jogadaValida(TCHAR charc, TCHAR newChar, int posX, int posY, int newPosX, int newPosY, const TCHAR characters[]) {
    if (charc == WATER[0]) {
        if (posY - newPosY == 1 && posX == newPosX) {
            if (newChar == CHARACTERS[2] || newChar == CHARACTERS[5] || newChar == CHARACTERS[0])
                return TRUE;
        }
        if (posY - newPosY == -1 && posX == newPosX) {
            if (newChar == CHARACTERS[3] || newChar == CHARACTERS[4] || newChar == CHARACTERS[0])
                return TRUE;
        }
    }
    else if (charc == WATER[1]) {
        if (posX - newPosX == 1 && posY == newPosY) {
            if (newChar == CHARACTERS[2] || newChar == CHARACTERS[3] || newChar == CHARACTERS[1]) {
                return TRUE;
            }
        }
        if (posX - newPosX == -1 && posY == newPosY) {
            if (newChar == CHARACTERS[4] || newChar == CHARACTERS[5] || newChar == CHARACTERS[1]) {
                return TRUE;
            }
        }
    }
    else if (charc == WATER[2]) {
        if (posY - newPosY == -1 && posX == newPosX) {
            if (newChar == CHARACTERS[3] || newChar == CHARACTERS[0] || newChar == CHARACTERS[4])
                return TRUE;
        }
        if (posX - newPosX == -1 && posY == newPosY) {
            if (newChar == CHARACTERS[1] || newChar == CHARACTERS[4] || newChar == CHARACTERS[5])
                return TRUE;
        }
    }
    else if (charc == WATER[3]) {
        if (posX - newPosX == -1 && posY == newPosY) {
            if (newChar == CHARACTERS[1] || newChar == CHARACTERS[4] || newChar == CHARACTERS[5])
                return TRUE;
        }
        if (posY - newPosY == 1 && posX == newPosX) {
            if (newChar == CHARACTERS[0] || newChar == CHARACTERS[2] || newChar == CHARACTERS[5]) {
                return TRUE;
            }
        }
    }
    else if (charc == WATER[4]) {
        if (posX - newPosX == 1 && posY == newPosY) {
            if (newChar == CHARACTERS[1] || newChar == CHARACTERS[2] || newChar == CHARACTERS[3])
                return TRUE;
        }
        if (posY - newPosY == 1 && posX == newPosX) {
            if (newChar == CHARACTERS[0] || newChar == CHARACTERS[2] || newChar == CHARACTERS[5]) {
                return TRUE;
            }
        }

    }
    else if (charc == WATER[5]) {
        if (posX - newPosX == 1 && posY == newPosY) {
            if (newChar == CHARACTERS[1] || newChar == CHARACTERS[2] || newChar == CHARACTERS[3])
                return TRUE;
        }
        if (posY - newPosY == -1 && posX == newPosX) {
            if (newChar == CHARACTERS[0] || newChar == CHARACTERS[3] || newChar == CHARACTERS[4]) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*================================Funcao que devolve se o caracter enviado e um caracter sem agua======================================*/
BOOL isCharacters(TCHAR caracter, const TCHAR characters[]) {
    for (int i = 0; i < TAMCHAR; i++)
    {
        if (caracter == characters[i])
            return TRUE;
    }
    return FALSE;
}