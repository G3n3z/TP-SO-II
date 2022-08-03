#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include "Monitor.h"
#include <wincon.h>
#include "..\dllFuncoes\funcoes.h"


COORD getPosition() {
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
    return consoleInfo.dwCursorPosition;
}

BOOL isWater(TCHAR c) {
    for (int i = 0; i < TAMCHAR; i++)
    {
        if (c == WATER[i])
            return TRUE;
    }
    return FALSE;
}

void limpaLinha() {
    for (int i = 0; i < 80; i++)
    {
        _tprintf(TEXT(" "));
    }
    _tprintf(TEXT("\r"));
}

void imprimeAzul() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
}

void imprimeBranco() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0007);
}

void imprimeVermelho() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD) 12);
}

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void mudaCursor(COORD coord) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void quebraLinha(COORD* coord, int numLinhas) {
    (*coord).Y += numLinhas;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (*coord));
}

void limpaResultado(COORD coord) {
    COORD lastCoord = getPosition();
    mudaCursor(coord);
    for (size_t i = coord.X; i < 80; i++)
    {
        _tprintf(TEXT(" "));
    }
    mudaCursor(lastCoord);
}

void limpaTabuleiros( int nlinhas) {
    COORD lastCoord = getPosition();
    COORD coord = { 0, 4 };
    mudaCursor(coord);
    for (int i = 0; i < nlinhas + 5; i++)
    {
        for (int j = 0; j < 80; j++)
        {
            _tprintf(TEXT(" "));
        }
        _tprintf(TEXT("\n"));
    }
    mudaCursor(lastCoord);
}

DWORD WINAPI threadLeitura(LPVOID dados);
DWORD WINAPI threadPrimeiraLeitura(LPVOID dados);
DWORD WINAPI threadComandos(LPVOID dados);

int getEstado(dadosMonitor* dadosT) {
    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    int estado = dadosT->shr->estado;
    ReleaseMutex(dadosT->mutexSHR);
    return estado;
}

void imprimeComandos(int numLinhas) {
    COORD coord = { 0, numLinhas + 10};
    mudaCursor(coord);
    _tprintf(TEXT("Comandos:"));
    quebraLinha(&coord, 1);
    _tprintf(TEXT("encerrar"));
    quebraLinha(&coord, 1);
    _tprintf(TEXT("parar agua <jogador> <segundos>"));
    quebraLinha(&coord, 1);
    _tprintf(TEXT("insere bloco <jogador> <linha> <coluna>"));
    quebraLinha(&coord, 1);
    _tprintf(TEXT("ativa aleatorio"));
    quebraLinha(&coord, 1);
    _tprintf(TEXT("desativa aleatorio"));
    quebraLinha(&coord, 1);
    
}

BOOL validaDados(dadosMonitor* dadosT, struct MSGbufferCircular stMsg) {
    BOOL retorno = TRUE;
    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    if (stMsg.player > dadosT->shr->numJogadores-1 || stMsg.player < 0) {
        _tprintf(TEXT("Numero de Jogador Invalido\n"));
        retorno = FALSE;
    }
    if (stMsg.x < 0 || stMsg.x >= dadosT->shr->nLinhas) {
        _tprintf(TEXT("Linha Invalida\n"));
        retorno = FALSE;
    }
    if (stMsg.y < 0 || stMsg.x >= dadosT->shr->nColunas) {
        _tprintf(TEXT("Coluna Invalida\n"));
        retorno = FALSE;
    }
    ReleaseMutex(dadosT->mutexSHR);

    return retorno;
}

BOOL validaDadosAgua(dadosMonitor* dadosT, struct MSGbufferCircular stMsg) {
    BOOL retorno = TRUE;
    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    if (stMsg.player > dadosT->shr->numJogadores - 1 || stMsg.player < 0) {
        _tprintf(TEXT("Numero de Jogador Invalido\n"));
        retorno = FALSE;
    }
    ReleaseMutex(dadosT->mutexSHR);
    return retorno;
}

BOOL retiraArgs(TCHAR* comando, struct MSGbufferCircular* stMsg, int numWords, int numElements) {
    TCHAR* aux = NULL;
    TCHAR aux2[TAMMSG];
    TCHAR* next_Token = NULL;
    aux2[0] = TEXT('\0');
    int array[3] = { 0,0,0 };
    aux = _tcstok_s(comando, TEXT(" "), &next_Token);
    if (aux == NULL) {
        return FALSE;
    }
    copia(aux2, aux, (int)_tcslen(aux));
    for (int i = 1; i < numWords; i++)
    {
        aux = _tcstok_s(NULL, TEXT(" "), &next_Token);
        concatena(aux2, TEXT(" "), (int)_tcslen(TEXT(" ")), (int)_tcslen(aux2), TAMMSG);
        concatena(aux2, aux, (int)_tcslen(aux), (int)_tcslen(aux2), TAMMSG);
    }
    copia(stMsg->msg, aux2, (int)_tcslen(aux2));
    stMsg->msg[_tcslen(stMsg->msg)] = TEXT('\0');
    if (numElements == 0)
        return TRUE;

    for (int i = 0; i < numElements; i++)
    {
        aux = _tcstok_s(NULL, TEXT(" "), &next_Token);
        if (aux != NULL) {
            array[i] = _tstoi(aux);
            if (array[i] < 0)
                return FALSE;
        }
        else if (i < numElements) {
            return FALSE;
        }
    }
    stMsg->player = array[0];
    stMsg->x = array[1];
    stMsg->y = array[2];

    return TRUE;
}

BOOL initializeResources(dadosMonitor* dadosT, dadosThreadComandos* dadosComandos, HANDLE map) {
    
    map = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, NAMESHR);
    if (map == NULL) {
        _tprintf(_T("\nNão foi possivel abrir a memória partilhada"));
        return FALSE; 
    }
    
    char* p = (char*)MapViewOfFile(map, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, (sizeof(dadosJogo) + sizeof(bufferCircular)));
    dadosT[0].shr = (dadosJogo*)p;
    dadosComandos->buffer = (bufferCircular*)(p + sizeof(dadosJogo));
        
    if (dadosT[0].shr == NULL) {
        _tprintf(_T("Não foi possivel alocar a memoria partilhada"));
        CloseHandle(map);
        return FALSE;
    }

    dadosT[1].shr = dadosT[0].shr;
    if (dadosT[1].shr == NULL) {
        _tprintf(_T("Não foi possivel alocar a memoria partilhada"));
        CloseHandle(map);
        return FALSE;
    }
    
    if (dadosComandos->buffer == NULL) {
        _tprintf(_T("N?o foi possivel alocar a o buffer circular"));
        CloseHandle(map);
        return FALSE;
    }
    
    dadosT[0].mutexSHR = OpenMutex(SYNCHRONIZE, FALSE, NAMEMUTEX);
    if (dadosT[0].mutexSHR == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sincronização mutex"));
        UnmapViewOfFile(dadosT->shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);

        return FALSE;
    }

    dadosT[0].mutexSair = CreateMutex(NULL, FALSE, NULL);
    if (dadosT[0].mutexSair == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sincronização mutex SAIR"));
        UnmapViewOfFile(dadosT->shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT->mutexSHR);
        return FALSE;
    }

    dadosT[0].EventServ2Moni = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENTSERVIDOR2MONITOR);
    if (dadosT[0].EventServ2Moni == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sincronização Event EVENTSERVIDOR2MONITOR"));
        UnmapViewOfFile(dadosT->shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT->mutexSHR);
        CloseHandle(dadosT->mutexSair);
        return FALSE;
    }
    
    dadosT[1].EventServ2Moni = OpenEvent(EVENT_ALL_ACCESS, FALSE, EVENTSERVIDOR2MONITOR2);
    if (dadosT[1].EventServ2Moni == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sincronização Event EVENTSERVIDOR2MONITOR2"));
        UnmapViewOfFile(dadosT->shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT->mutexSHR);
        CloseHandle(dadosT->mutexSair);
        CloseHandle(dadosT[0].EventServ2Moni);
        return FALSE;
    }
    
    dadosT[0].mutexEcra = CreateMutex(NULL,FALSE, NULL);
    if (dadosT[0].mutexEcra == NULL) {
        UnmapViewOfFile(dadosT->shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT->mutexSHR);
        CloseHandle(dadosT->mutexSair);
        CloseHandle(dadosT[0].EventServ2Moni);
        CloseHandle(dadosT[1].EventServ2Moni);
        return FALSE;
    }

    dadosT[1].mutexEcra = dadosT[0].mutexEcra;
    dadosComandos->mutexEcra =  dadosT[0].mutexEcra;

    dadosComandos->sem_itens = CreateSemaphore(NULL, 0, TAMBUFFCIRC, SEMAPHOREITENS);
    if (dadosComandos->sem_itens == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sem_itens\n"));
        UnmapViewOfFile(dadosT->shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT->mutexSHR);
        CloseHandle(dadosT->mutexSair);
        CloseHandle(dadosT[0].EventServ2Moni);
        CloseHandle(dadosT[1].EventServ2Moni);
        CloseHandle(dadosT[0].mutexEcra);
        return FALSE;
    }

    dadosComandos->sem_vazios = CreateSemaphore(NULL, TAMBUFFCIRC, TAMBUFFCIRC, SEMAPHOREVAZIO);
    if (dadosComandos->sem_vazios == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sem_vazios\n"));
        UnmapViewOfFile(dadosT->shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT->mutexSHR);
        CloseHandle(dadosT->mutexSair);
        CloseHandle(dadosT[0].EventServ2Moni);
        CloseHandle(dadosT[1].EventServ2Moni);
        CloseHandle(dadosT[0].mutexEcra);
        CloseHandle(dadosComandos->sem_itens);
        return FALSE;
    }
    
    dadosComandos->mutexShr = CreateMutex(NULL, FALSE, NAMEMUTEXPRODUTORES);
    if (dadosComandos->mutexShr == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de MUTEX do buffer circular\n"));
        UnmapViewOfFile(dadosT->shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT->mutexSHR);
        CloseHandle(dadosT->mutexSair);
        CloseHandle(dadosT[0].EventServ2Moni);
        CloseHandle(dadosT[1].EventServ2Moni);
        CloseHandle(dadosT[0].mutexEcra);
        CloseHandle(dadosComandos->sem_itens);
        CloseHandle(dadosComandos->sem_vazios);
        return FALSE;
    }

    
    return TRUE;

}

void closeResources(dadosMonitor* dadosT, dadosThreadComandos* dadosComandos, HANDLE map) {
    UnmapViewOfFile(dadosT->shr);
    UnmapViewOfFile(dadosComandos->buffer);
    CloseHandle(map);
    CloseHandle(dadosT->mutexSHR);
    CloseHandle(dadosT->mutexSair);
    CloseHandle(dadosT[0].EventServ2Moni);
    CloseHandle(dadosT[1].EventServ2Moni);
    CloseHandle(dadosT[0].mutexEcra);
    CloseHandle(dadosComandos->sem_itens);
    CloseHandle(dadosComandos->sem_vazios);
}


void imprimeTab(dadosJogo tab, int jogador) {
    COORD coord = { jogador * 45, 4 };
    COORD lastCoord = getPosition();

    int i, j;
    mudaCursor(coord);
    _tprintf(TEXT("Jogador: %d"), jogador);
    quebraLinha(&coord, 1);
    _tprintf(TEXT("Pontuacao: %d - Proxima Jogada: %c "), tab.jogador[jogador].pont, tab.jogador[jogador].nextChar);
    quebraLinha(&coord,2);
    
    for (int j = 0; j < tab.nColunas; j++) {
        _tprintf(TEXT("-"));
    }
    //_tprintf(TEXT("\n"));
    quebraLinha(&coord, 1);
    for (i = 0; i < tab.nLinhas; i++) {
        _tprintf(TEXT("|"));
        for (j = 0; j < tab.nColunas; j++) {
            if (tab.jogador[jogador].tab[i][j] == BLOCO) {
                imprimeVermelho();
                _tprintf(TEXT("%c"), tab.jogador[jogador].tab[i][j]);
                imprimeBranco();
            }
            else if (isWater(tab.jogador[jogador].tab[i][j])) {
                imprimeAzul();
                _tprintf(TEXT("%c"), tab.jogador[jogador].tab[i][j]);
                imprimeBranco();
            }
            else
                _tprintf(TEXT("%c"), tab.jogador[jogador].tab[i][j]);
        }
        _tprintf(TEXT("|  (%d,%d) - (%d,%d)"), i,j-tab.nColunas, i,j-1);
        quebraLinha(&coord, 1);
    }
    _tprintf(TEXT(" "));
    for (int l = 0; l < tab.nColunas; l++) {
        _tprintf(TEXT("-"));
    }
    quebraLinha(&coord, 2);

    mudaCursor(lastCoord);
}


int _tmain(int argc, TCHAR* argv[]) {



#ifdef UNICODE 
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif
    system("cls");
    
    dadosMonitor dadosT[MAXJOGADORES];
    dadosThreadComandos dadosComandos;
    HANDLE map;

    if (!initializeResources(dadosT, &dadosComandos, &map)) {
        _tprintf(TEXT("Houve problemas a initializar os recursos"));
        exit(1);
    }
    DWORD idMonitor, idComandos;
    
    dadosT[1].mutexSair = dadosT[0].mutexSair;
    dadosComandos.dadosT = dadosT;   

    HANDLE hThreadMonitor = CreateThread(NULL, 0, threadPrimeiraLeitura, (LPVOID)&dadosT, 0, &idMonitor);
    if (hThreadMonitor == NULL) {
        
        _tprintf(TEXT("Não foi possivel iniciar a thread de comunicação com o servidor\n"));
        closeResources(dadosT, &dadosComandos, map);
        exit(1);
    }
   
    HANDLE hThreadComandos = CreateThread(NULL, 0, threadComandos, (LPVOID)&dadosComandos, 0, &idComandos);
    if (hThreadComandos == NULL) {
        _tprintf(TEXT("Não foi possivel iniciar a thread de comandos\n"));
        closeResources(dadosT, &dadosComandos, map);
        CloseHandle(hThreadMonitor);
        exit(1);
    }

    HANDLE hThreads[2] = { hThreadMonitor ,hThreadComandos };
    
    DWORD resultado = WaitForMultipleObjects(2, hThreads, FALSE, INFINITE);
    if (resultado >= WAIT_OBJECT_0 && resultado < WAIT_OBJECT_0 + 2) {
        if ((resultado - WAIT_OBJECT_0) == 1) {
            WaitForSingleObject(hThreadMonitor, INFINITE);
            CloseHandle(hThreadComandos);
        }
        else {
            CloseHandle(hThreadComandos);
            //WaitForSingleObject(hThreadComandos, INFINITE);
        }
    }

    closeResources(dadosT, &dadosComandos, map);
    CloseHandle(hThreadMonitor);
    
    exit(0);
}

DWORD WINAPI threadComandos(LPVOID dados) {
    dadosThreadComandos* dadosComandos = (dadosThreadComandos*)dados;

    int flag = 0;
    dadosComandos->dadosT[0].flagSair = &flag;
    dadosComandos->dadosT[1].flagSair = &flag;

    WaitForSingleObject(dadosComandos->dadosT->mutexSHR, INFINITE);
    int numLinhas = dadosComandos->dadosT->shr->nLinhas;
    int numColunas = dadosComandos->dadosT->shr->nColunas;
    ReleaseMutex(dadosComandos->dadosT->mutexSHR);

    TCHAR comando[TAMMSG];
    struct MSGbufferCircular stMsg;
    int estado;
    int i = 0;
    TCHAR c;
    COORD lastCoord;

    WaitForSingleObject(dadosComandos->dadosT->mutexEcra, INFINITE);
    imprimeComandos(numLinhas);
    _tprintf(TEXT("Digita Comando:\n"));
    lastCoord = getPosition();
    ReleaseMutex(dadosComandos->dadosT->mutexEcra);

    while (1) {

        WaitForSingleObject(dadosComandos->dadosT->mutexEcra, INFINITE);
        imprimeComandos(numLinhas);
        _tprintf(TEXT("Digita Comando:\n"));
        ReleaseMutex(dadosComandos->dadosT->mutexEcra);

        comando[0] = TEXT('\0');
        i = 0;
        mudaCursor(lastCoord);
        limpaLinha();
        mudaCursor(lastCoord);
        while (1)
        {
            c = (TCHAR)_fgettc(stdin);
            if (c == TEXT('\n'))
                break;
            comando[i++] = c;

        }

        comando[i] = TEXT('\0');
 
        toLower(comando);
        limpaLinha();
        estado = getEstado(dadosComandos->dadosT);

        if (_tcscmp(comando, TEXT("encerrar")) == 0) {
            WaitForSingleObject(dadosComandos->dadosT[0].mutexSair, INFINITE);
            flag = 1;
            ReleaseMutex(dadosComandos->dadosT[0].mutexSair);
            SetEvent(dadosComandos->dadosT[0].EventServ2Moni);
            ResetEvent(dadosComandos->dadosT[0].EventServ2Moni);
            SetEvent(dadosComandos->dadosT[1].EventServ2Moni);
            ResetEvent(dadosComandos->dadosT[1].EventServ2Moni);
            break;
        }
        if (estado == 0) {
            WaitForSingleObject(dadosComandos->dadosT->mutexEcra, INFINITE);
            _tprintf(TEXT("Não existe nenhum jogo ativo neste momento\n"));
            ReleaseMutex(dadosComandos->dadosT->mutexEcra);
            continue;
        }
        else if (estado == 2) {
            WaitForSingleObject(dadosComandos->dadosT->mutexEcra, INFINITE);
            _tprintf(TEXT("Jogo em Pausa\n"));
            ReleaseMutex(dadosComandos->dadosT->mutexEcra);
            continue;
        }
        if (!_tcsncmp(comando, TEXT("parar agua"), 10)) {
            if (!retiraArgs(comando, &stMsg, 2, 2)) {
                WaitForSingleObject(dadosComandos->dadosT->mutexEcra, INFINITE);
                _tprintf(TEXT("Comando Invalido\n"));
                ReleaseMutex(dadosComandos->dadosT->mutexEcra);
                continue;
            }
            else {
                if (!validaDadosAgua(dadosComandos->dadosT, stMsg)) {
                    continue;
                }
            }
        }
        else if (!_tcsncmp(comando, TEXT("insere bloco"), 12)) {
            if (!retiraArgs(comando, &stMsg, 2, 3)) {
                WaitForSingleObject(dadosComandos->dadosT->mutexEcra, INFINITE);
                _tprintf(TEXT("Comando Invalido\n"));
                ReleaseMutex(dadosComandos->dadosT->mutexEcra);
                continue;
            }
            else {
                if (!validaDados(dadosComandos->dadosT, stMsg)) {
                    continue;
                }
            }
        }
        else if (!_tcsncmp(comando, TEXT("ativa aleatorio"), 15)) {
            if (!retiraArgs(comando, &stMsg, 2, 0)) {
                WaitForSingleObject(dadosComandos->dadosT->mutexEcra, INFINITE);
                _tprintf(TEXT("Comando Invalido\n"));
                ReleaseMutex(dadosComandos->dadosT->mutexSair);
                continue;
            }
        }
        else if (!_tcsncmp(comando, TEXT("desativa aleatorio"), 19)) {
            if (!retiraArgs(comando, &stMsg, 2, 0)) {
                WaitForSingleObject(dadosComandos->dadosT->mutexSair, INFINITE);
                _tprintf(TEXT("Comando Invalido\n"));
                ReleaseMutex(dadosComandos->dadosT->mutexEcra);
                continue;
            }
        }
        else {
            WaitForSingleObject(dadosComandos->dadosT->mutexEcra, INFINITE);
            _tprintf(TEXT("Comando não reconhecido\n"));
            ReleaseMutex(dadosComandos->dadosT->mutexEcra);
            continue;
        }
        WaitForSingleObject(dadosComandos->sem_vazios, INFINITE);

        WaitForSingleObject(dadosComandos->mutexShr, INFINITE);
        CopyMemory(dadosComandos->buffer->buff + dadosComandos->buffer->in, &stMsg, sizeof(stMsg));
        dadosComandos->buffer->in = (dadosComandos->buffer->in + 1) % TAMBUFFCIRC;
        ReleaseMutex(dadosComandos->mutexShr);

        ReleaseSemaphore(dadosComandos->sem_itens, 1, NULL);

    }
    ExitThread(0);
}

DWORD WINAPI threadPrimeiraLeitura(LPVOID d) {
    int numJogadores;
    dadosMonitor* dadosT = (dadosMonitor*)d;
    HANDLE events[MAXJOGADORES] = { dadosT[0].EventServ2Moni, dadosT[1].EventServ2Moni };
    HANDLE hThreads[MAXJOGADORES];
    DWORD idThread[MAXJOGADORES];
    COORD coordResultado = {30, 2}, lastCoord;
   
    while (1)
    {   
        DWORD res = WaitForMultipleObjects(MAXJOGADORES, events, FALSE, INFINITE);
        if (!(res >= WAIT_OBJECT_0 && res < WAIT_OBJECT_0 + MAXJOGADORES)) {
            _tprintf(TEXT("Aconteceu algo"));
            continue;
        }
 
        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        numJogadores = dadosT->shr->numJogadores;
        if (_tcslen(dadosT->shr->msg) > 0) {
            if (!_tcscmp(dadosT->shr->msg, TEXT("encerrar"))) {
                WaitForSingleObject(dadosT->mutexSair, INFINITE);
                *dadosT->flagSair = 1;
                ReleaseMutex(dadosT->mutexSair);
                break;
            }
            else if(!_tcsncmp(dadosT->shr->msg, TEXT("Ganhou"),6) || !_tcsncmp(dadosT->shr->msg, TEXT("Perdeu"),6) || !_tcsncmp(dadosT->shr->msg, TEXT("Empate"), 6)){
                WaitForSingleObject(dadosT->mutexEcra,INFINITE);
                lastCoord = getPosition();
                mudaCursor(coordResultado);
                imprimeVermelho();
                
                _tprintf(TEXT("%s"), dadosT->shr->msg);
                imprimeBranco();
                mudaCursor(lastCoord);
                ReleaseMutex(dadosT->mutexEcra);
            }
        }
       
        if(dadosT->shr->estado == 1){
            ReleaseMutex(dadosT->mutexSHR);
            limpaResultado(coordResultado);
            limpaTabuleiros(dadosT->shr->nLinhas);
            for (int i = 0; i < numJogadores; i++) {
                dadosT[i].jogador = i;
                hThreads[i] = CreateThread(NULL,0, threadLeitura, &dadosT[i],0,&idThread[i]);
                if (hThreads[i] == NULL) {
                    _tprintf(TEXT("Não foi possivel criar a thread"));
                    break;
                }
            }
        }
        ReleaseMutex(dadosT->mutexSHR);
        WaitForMultipleObjects(numJogadores, hThreads, TRUE, INFINITE);
        
        WaitForSingleObject(dadosT->mutexSair, INFINITE);
        if (*dadosT->flagSair == 1) {
            ReleaseMutex(dadosT->mutexSair);
            break;
        }
        ReleaseMutex(dadosT->mutexSair);
    }

    for (int i = 0; i < numJogadores; i++) {
        CloseHandle(hThreads[i]);
    }
 
    ExitThread(0);
}

DWORD WINAPI threadLeitura(LPVOID dados) {
    dadosMonitor* dadosT = (dadosMonitor*)dados;
    dadosJogo tab;
    int jogador = dadosT->jogador;

    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    if (dadosT->shr->estado == 1) {
        WaitForSingleObject(dadosT->mutexEcra, INFINITE);

        imprimeTab(*dadosT->shr, jogador);
        ReleaseMutex(dadosT->mutexEcra);
    }
    ReleaseMutex(dadosT->mutexSHR);
    while (1) {
        WaitForSingleObject(dadosT->EventServ2Moni, INFINITE);
        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        CopyMemory(&tab, dadosT->shr, sizeof(dadosJogo));
        ReleaseMutex(dadosT->mutexSHR);
        WaitForSingleObject(dadosT->mutexSair, INFINITE);
        if (*dadosT->flagSair == 1) {
            ReleaseMutex(dadosT->mutexSair);
            break;
        }
        ReleaseMutex(dadosT->mutexSair);

        if (_tcslen(tab.msg) > 1) {
            if (!_tcscmp(tab.msg, TEXT("encerrar"))) {
                WaitForSingleObject(dadosT->mutexEcra, INFINITE);
                _tprintf(TEXT("O servidor mandou encerrar\n"));
                ReleaseMutex(dadosT->mutexEcra);
                WaitForSingleObject(dadosT->mutexSair, INFINITE);
                *dadosT->flagSair = 1;
                ReleaseMutex(dadosT->mutexSair);
                break;
            }
            else if (!_tcscmp(tab.msg, TEXT("sair"))) {
                WaitForSingleObject(dadosT->mutexSHR, INFINITE);
                dadosT->shr->estado = 0;
                ReleaseMutex(dadosT->mutexSHR);
                break;
            }
        }
        
        WaitForSingleObject(dadosT->mutexEcra, INFINITE);
        imprimeTab(tab, jogador);
        ReleaseMutex(dadosT->mutexEcra);
    }
 
    ExitThread(0);
}
