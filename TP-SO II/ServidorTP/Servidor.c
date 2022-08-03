#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include "Agua.h"
#include "Comandos.h"

void init_rand() {
    srand((unsigned)time(NULL));
}

int random_l_h(int min, int max)
{
    return min + rand() % (max - min + 1);
}

DWORD WINAPI threadMonitor(LPVOID dados);
DWORD WINAPI threadComandos(LPVOID dados);
DWORD WINAPI joga(LPVOID d);

DWORD WINAPI iniciaJogo(LPVOID dados);


void CarregaMapaWin(dadosThreadJogador* dadosT, int jogador);
TCHAR sorteiaProximaJogada(dadosJogo tab, int modoJogo, int jogador, const TCHAR characters[]);


DWORD jogaPorNiveis(dadosThreadJogador* dadosT, int jogador, int nJogos);
DWORD jogaMultiJogador(dadosThreadJogador* dadosT, int jogador);

DWORD leDWORDRegEdit(HKEY hKey, TCHAR* name) {
    DWORD value = 0, tam = sizeof(DWORD);
    LSTATUS res;

    res = RegQueryValueEx(hKey, name, 0, 0, (LPBYTE)&value, &tam);

    if (res != ERROR_SUCCESS) {
        _tprintf(TEXT("Nao foi possivel ler o campo %s"), name);
        exit(1);
    }
    _tprintf(TEXT("%s: %d\n"), name, value);
    return value;
}
void writeDWORDRegEdit(HKEY hKey, TCHAR* name, DWORD value) {
    LSTATUS res;
    res = RegSetValueExW(hKey, name, 0, REG_DWORD, (LPBYTE)&value, sizeof(value));
    if (res != ERROR_SUCCESS) {
        _tprintf(TEXT("Nao foi possivel o valor %s "), name);
        exit(1);
    }

}

dadosJogo limparTabuleiro(dadosJogo tab, int modo, int jogador, int* posIIni, int* posJIni, int* posIFinal, int* posJFinal,const TCHAR characters[]) {
    for (int l = 0; l < MAXJOGADORES; l++) {
        for (int i = 0; i < tab.nLinhas; i++) {
            for (int j = 0; j < tab.nColunas; j++) {
                tab.jogador[l].tab[i][j] = TEXT(' ');
            }
        }
    }
 
    int i = random_l_h(0, tab.nLinhas - 1);
    int j = 0, w = 0, z = 0;

    if (i == 0 || i == tab.nLinhas - 1) {
        j = random_l_h(0, tab.nColunas - 1);
    }
    else {
        j = random_l_h(0, 1);
        j = (j == 0 ? 0 : tab.nColunas - 1);
    }

    *posIIni = i;
    *posJIni = j;

    if (i < tab.nLinhas / 2) {
        w = random_l_h(tab.nLinhas / 2, tab.nLinhas - 1);
        if (w == tab.nLinhas - 1) {
            if (j < tab.nColunas / 2) {
                z = random_l_h(tab.nColunas / 2, tab.nColunas - 1);
            }
            else {
                z = random_l_h(0, tab.nColunas / 2);
            }
        }
        else {
            if (j < tab.nColunas / 2) {
                z = tab.nColunas - 1;
            }
        }
    }
    else {
        w = random_l_h(0, tab.nLinhas / 2);
        if (w == 0) {
            if (j < tab.nColunas / 2) {
                z = random_l_h(tab.nColunas / 2, tab.nColunas - 1);
            }
            else {
                z = random_l_h(0, tab.nColunas / 2);
            }
        }
        else {
            if (j < tab.nColunas / 2) {
                z = tab.nColunas - 1;
            }
        }
    }
    
    *posIFinal = w;
    *posJFinal = z;

    for (int l = 0; l < MAXJOGADORES; l++) {
        tab.jogador[l].tab[*posIIni][*posJIni] = WATER[1];
        tab.jogador[l].tab[*posIFinal][*posJFinal] = CHARACTERS[1];
        tab.jogador[l].nextChar = CHARACTERS[0];
    }

    return tab;
}

BOOL verificaPosicao(dadosJogo tab, int jogador, int x, int y, int posXFinal, int posYFinal, const TCHAR characters[]) {
    if (x < 0 || x > tab.nLinhas)
        return FALSE;
    if (y < 0 || y > tab.nColunas)
        return FALSE;
    if (posXFinal == x && posYFinal == y) {
        return FALSE;
    }
    if (tab.jogador[jogador].tab[x][y] == TEXT(' ')) {
        return TRUE;
    }
    for (int i = 0; i < TAMCHAR; i++) {
        if (tab.jogador[jogador].tab[x][y] == CHARACTERS[i]) {
            return TRUE;
        }
    }
    return FALSE;
}

TCHAR sorteiaProximaJogada(dadosJogo tab, int modoJogo, int jogador, const TCHAR characters[]) {
    if (modoJogo == 0) { //Modo random
        return characters[random_l_h(0, TAMCHAR - 1)];
    }
    else {
        int index = 0;
        for (int i = 0; i < TAMCHAR; i++) {
            if (tab.jogador[jogador].nextChar == characters[i])
                index = i;
        }
        index = (index == TAMCHAR - 1 ? 0 : ++index);
        return characters[index];
    }
}
TCHAR rodaPeca(TCHAR peca) {

    for (int i = 0; i < 6; i++)
    {
        if (peca == CHARACTERS[i]) {
            return CHARACTERS[i == 5 ? 0 : (i+1)];
        }
    }
    return TEXT(' ');
}
dadosJogo colocaJogada(dadosJogo tab, int modojogo, int jogador, int x, int y, const TCHAR characters[]) {
    tab.jogador->numeroJogadas++;
    //_tprintf(TEXT("x:%d y:%d"), x, y);
    if (tab.jogador[jogador].tab[x][y] != TEXT(' ')) {
        tab.jogador[jogador].tab[x][y] = rodaPeca(tab.jogador[jogador].tab[x][y]);
        return tab;
    }

    tab.jogador[jogador].tab[x][y] = tab.jogador[jogador].nextChar;
    tab.jogador[jogador].nextChar = sorteiaProximaJogada(tab, modojogo, jogador, characters);
    return tab;
}

void imprimeTab(dadosJogo tab, int jogador) {
    int i, j;
    _tprintf(TEXT("\n\n "));
    _tprintf(TEXT("Jogador: %d"), jogador);
    _tprintf(TEXT("Proxima Jogada: %c\n\n "), tab.jogador[jogador].nextChar);
    for (int j = 0; j < tab.nColunas; j++) {
        _tprintf(TEXT("-"));
    }
    _tprintf(TEXT("\n"));
    for (i = 0; i < tab.nLinhas; i++) {
        _tprintf(TEXT("|"));
        for (j = 0; j < tab.nColunas; j++) {
            _tprintf(TEXT("%c"), tab.jogador[jogador].tab[i][j]);
        }
        _tprintf(TEXT("|  (%d,%d) - (%d,%d)\n"), i, j - tab.nColunas, i, j - 1);
    }
    _tprintf(TEXT(" "));
    for (int l = 0; l < tab.nColunas; l++) {
        _tprintf(TEXT("-"));
    }
    _tprintf(TEXT("\n\n"));
}

BOOL initializeResource(dadosThreadJogador* dadosT, dadosThreadComandos* dadosComandos, HANDLE map) {

    map = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (TAMSHR + SIZEBUFFER), NAMESHR);
    if (map == NULL) {
        _tprintf(_T("\nNão foi possivel criar a memória partilhada"));
        return FALSE;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        _tprintf(_T("Já se encontra um servidor a correr"));
        CloseHandle(map);
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
        _tprintf(_T("Não foi possivel alocar a memoria partilhada2  -- %d\n"), GetLastError());
        CloseHandle(map);
        return FALSE;
    }

    if (dadosComandos->buffer == NULL) {
        _tprintf(_T("Não foi possivel alocar a o buffer circular"));
        CloseHandle(map);
        return FALSE;
    }
    
    dadosT[0].mutexSHR = CreateMutex(NULL, FALSE, NAMEMUTEX);
    if (dadosT[0].mutexSHR == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sincroniza??o mutex"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        return FALSE;
    }

    dadosT[0].mutexSair = CreateMutex(NULL, FALSE, NULL);
    if (dadosT[0].mutexSair == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sincroniza??o mutex SAIR"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT[0].mutexSHR);
        return FALSE;
    }
    dadosT[0].mutexStruct = CreateMutex(NULL, FALSE, NULL);
    if (dadosT[0].mutexStruct == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sincroniza??o mutex SAIR"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT[0].mutexSHR);
        CloseHandle(dadosT[0].mutexSair);

        return FALSE;
    }

    dadosT[0].EventServ2Moni = CreateEvent(NULL, TRUE, FALSE, EVENTSERVIDOR2MONITOR);
    if (dadosT[0].EventServ2Moni == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sincroniza??o Event EVENTSERVIDOR2MONITOR"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT[0].mutexSHR);
        CloseHandle(dadosT[0].mutexSair);
        CloseHandle(dadosT[0].mutexStruct);
        return FALSE;
    }
    dadosT[1].EventServ2Moni = CreateEvent(NULL, TRUE, FALSE, EVENTSERVIDOR2MONITOR2);
    if (dadosT[1].EventServ2Moni == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sincroniza??o Event EVENTMONITOR2SERVIDOR"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT[0].mutexSHR);
        CloseHandle(dadosT[0].mutexSair);
        CloseHandle(dadosT[0].mutexStruct);
        CloseHandle(dadosT[0].EventServ2Moni);
        return FALSE;
    }
    dadosT[0].waitTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (dadosT[0].waitTimer == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de WAITABLETIMER\n"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT[0].mutexSHR);
        CloseHandle(dadosT[0].mutexSair);
        CloseHandle(dadosT[0].mutexStruct);
        CloseHandle(dadosT[0].EventServ2Moni);
        CloseHandle(dadosT[1].EventServ2Moni);
        return FALSE;
    }
    dadosT[1].waitTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (dadosT[1].waitTimer == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de WAITABLETIMER2\n"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT[0].mutexSHR);
        CloseHandle(dadosT[0].mutexSair);
        CloseHandle(dadosT[0].mutexStruct);
        CloseHandle(dadosT[0].EventServ2Moni);
        CloseHandle(dadosT[1].EventServ2Moni);
        CloseHandle(dadosT[0].waitTimer);
        return FALSE;
    }

    dadosComandos->sem_itens = CreateSemaphore(NULL,0,TAMBUFFCIRC,SEMAPHOREITENS);
    if (dadosComandos->sem_itens == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sem_itens\n"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT[0].mutexSHR);
        CloseHandle(dadosT[0].mutexSair);
        CloseHandle(dadosT[0].mutexStruct);
        CloseHandle(dadosT[0].EventServ2Moni);
        CloseHandle(dadosT[1].EventServ2Moni);
        CloseHandle(dadosT[0].waitTimer);
        CloseHandle(dadosT[1].waitTimer);
        return FALSE;
    }

    dadosComandos->sem_vazios = CreateSemaphore(NULL, TAMBUFFCIRC, TAMBUFFCIRC, SEMAPHOREVAZIO);
    if (dadosComandos->sem_vazios == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de sem_vazios\n"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT[0].mutexSHR);
        CloseHandle(dadosT[0].mutexSair);
        CloseHandle(dadosT[0].mutexStruct);
        CloseHandle(dadosT[0].EventServ2Moni);
        CloseHandle(dadosT[1].EventServ2Moni);
        CloseHandle(dadosT[0].waitTimer);
        CloseHandle(dadosT[1].waitTimer);
        CloseHandle(dadosComandos->sem_itens);
        return FALSE;
    }
    dadosComandos->mutexShr = CreateMutex(NULL,FALSE, NAMEMUTEXCONSUMIDORES);
    if (dadosComandos->mutexShr == NULL) {
        _tprintf(_T("Não foi possivel criar o mecanismo de MUTEX do buffer circular\n"));
        UnmapViewOfFile(dadosT[0].shr);
        UnmapViewOfFile(dadosComandos->buffer);
        CloseHandle(map);
        CloseHandle(dadosT[0].mutexSHR);
        CloseHandle(dadosT[0].mutexSair);
        CloseHandle(dadosT[0].mutexStruct);
        CloseHandle(dadosT[0].EventServ2Moni);
        CloseHandle(dadosT[1].EventServ2Moni);
        CloseHandle(dadosT[0].waitTimer);
        CloseHandle(dadosT[1].waitTimer);
        CloseHandle(dadosComandos->sem_itens);
        CloseHandle(dadosComandos->sem_vazios);
        return FALSE;
    }
    
    dadosT[1].mutexSHR = dadosT[0].mutexSHR;
    dadosT[1].mutexSair = dadosT[0].mutexSair;
    dadosT[1].mutexStruct = dadosT[0].mutexStruct;
    return TRUE;
}

BOOL closeResources(dadosThreadJogador* dadosT, dadosThreadComandos* dadosComandos, HANDLE map) {
    for (int i = 0; i < dadosT->shr->numJogadores; i++){
        CloseHandle(dadosT[i].EventServ2Moni);
        CloseHandle(dadosT[i].waitTimer);
    }
    CloseHandle(dadosT[0].mutexSHR);
    CloseHandle(dadosT[0].mutexSair);
    CloseHandle(dadosT[0].mutexStruct);
    UnmapViewOfFile(dadosT[0].shr);
    UnmapViewOfFile(dadosComandos->buffer);
    CloseHandle(map);
    CloseHandle(dadosComandos->sem_itens);
    CloseHandle(dadosComandos->sem_vazios);
return TRUE;
}

int _tmain(int argc, TCHAR* argv[]) {

#ifdef UNICODE 
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    HKEY hKey;
    dadosJogo dados;
    dadosThreadJogador dadosT[MAXJOGADORES];
    int numJogadores = 1;

    if (argc != 1 && argc != 4) {
        _tprintf(TEXT("Numero de args invalido. Se nao inserir argumentos será usado os valores padroes caso contrario envie 3 argumentos"));
        _tprintf(TEXT("<N de linhas> <Numero de colunas> <Tempo para a agua começar a fluir>"));
        return 0;
    }

    if (argc == 4) {

        dados.nLinhas = _tstoi(argv[1]);
        dados.nColunas = _tstoi(argv[2]);
        dados.time = _tstoi(argv[3]);
        if (dados.nLinhas <= 0 || dados.nColunas <= 0 || dados.time < 0 || dados.nLinhas > 20 || dados.nColunas > 20) {
            _tprintf(_T("Dados invalidos\n"));
            exit(1);
        }
        _tprintf(_T("Linhas: %d | Colunas %d | Time: %d"), dados.nLinhas, dados.nColunas, dados.time);
    }

    DWORD estado;
    LSTATUS res;

    if (argc == 1) { //Ir ao regEdit buscar os valores
        res = RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\SOII\\TP"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
            NULL, &hKey, &estado);
        if (res == ERROR_SUCCESS) {
            if (estado == REG_CREATED_NEW_KEY) {
                writeDWORDRegEdit(hKey, REGLINHAS, (DWORD)TAMMAX);
                writeDWORDRegEdit(hKey, REGCOLUNAS, (DWORD)TAMMAX);
                writeDWORDRegEdit(hKey, REGTEMPO, (DWORD)20);
                dados.nLinhas = 20;
                dados.nColunas = 20;
                dados.time = 20;
            }
            else {

                dados.nLinhas = (int)leDWORDRegEdit(hKey, REGLINHAS);
                //_tprintf(TEXT("Aqui"));
                dados.nColunas = (int)leDWORDRegEdit(hKey, REGCOLUNAS);
                dados.time = (int)leDWORDRegEdit(hKey, REGTEMPO);
                _tprintf(TEXT("Dados lidos %d %d %d"), dados.nLinhas, dados.nColunas, dados.time);
            }
        }
        RegCloseKey(hKey);
    }

    HANDLE eventFinal = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (eventFinal == NULL) {
        _tprintf(TEXT("Erro ao criar evento final\n"));
        exit(1);
    }

    HANDLE hThreadMonitor;
    HANDLE map;
    DWORD idMonitor;
    dadosThreadComandos dadosComandos;
    int nthreads = 2;

    if (!initializeResource(dadosT, &dadosComandos, &map)) {
        _tprintf(TEXT("Houve problemas a initializar os recursos"));
        exit(1);
    }
    WaitForSingleObject(dadosComandos.mutexShr, INFINITE);
    dadosComandos.buffer->in = 0;
    dadosComandos.buffer->out = 0;
    ReleaseMutex(dadosComandos.mutexShr);

    init_rand();
    int posIInitial, posJInitial, posIFinal, posJFinal;
    int perdeu = 0; int ganhou = 0;
    int flagSair = 0; int modoJogo = 0;
    for (int i = 0; i < MAXJOGADORES; i++) {
        dadosT[i].modoJogo = &modoJogo;
        dadosT[i].flagSair = &flagSair;
        dadosT[i].shr->nLinhas = dados.nLinhas;
        dadosT[i].shr->nColunas = dados.nColunas;
        dadosT[i].shr->time = dados.time;
        dadosT[i].shr->estado = 0;
        dadosT[i].perdeu = &perdeu;
        dadosT[i].ganhou = &ganhou;
        for (int j = 0; j < TAMCHAR; j++) {
            dadosT[i].characters[j] = CHARACTERS[j];
        }
        dadosT[i].posXini = &posIInitial;
        dadosT[i].posYini = &posJInitial;
        dadosT[i].posXfinal = &posIFinal;
        dadosT[i].posYfinal = &posJFinal;
        dadosT[i].eventFinal = eventFinal;
    }
    dadosT->time = dadosT->shr->time;
    dadosComandos.dadosT = dadosT;

    hThreadMonitor = CreateThread(NULL, 0, threadMonitor, (LPVOID)&dadosComandos, 0, &idMonitor);
    if (hThreadMonitor == NULL) {
        _tprintf(TEXT("Não foi possivel iniciar a thread de comunicação com o servidor\n"));
        closeResources(dadosT, &dadosComandos, &map);
        CloseHandle(eventFinal);
        exit(1);
    }

    HANDLE hThreadComandos = CreateThread(NULL, 0, threadComandos, (LPVOID)&dadosT, 0, &idMonitor);
    if (hThreadComandos == NULL) {
        _tprintf(TEXT("Não foi possivel iniciar a thread do admnistrador do servidor\n"));
        closeResources(dadosT, &dadosComandos, &map);
        CloseHandle(eventFinal);
        exit(1);
    }

    OVERLAPPED ov;
    HANDLE hPipe;
    int num = 0;
    dadosServ2Cli dadosIniciais;
    DWORD nLidos, nEscritos, idThreadJogo;
    HANDLE hThreadJogo;
    dadosCli2Serv dadosRecebidos;
    dadosIniciais.dados.nLinhas = dados.nLinhas;
    dadosIniciais.dados.nColunas = dados.nColunas;
    while (1) {
        hPipe = CreateNamedPipe(NAMEPIPE, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE
            , MAXJOGADORES+1, sizeof(dadosCli2Serv), sizeof(dadosServ2Cli), 100000, NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            _tprintf(TEXT("Problemas a criar os pipes"));
            exit(-1);
        }

        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        dadosT->ov = ov;
        if (!ConnectNamedPipe(hPipe, &ov)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                _tprintf(TEXT("\nÀ espera\n"));
                WaitForSingleObject(ov.hEvent, INFINITE);
                if (sair(dadosT)) {
                    break;
                }
            }
            else {
                _tprintf(TEXT("\nProblemas na ligacao\n"));
                break;
            }
        }

        if (num == 0) {
            //copia(dadosIniciais.msg, TEXT("0"), (int)_tcslen(TEXT("0")));
            dadosIniciais.val = ESPERAR;
            OVERLAPPED ov1;
            ZeroMemory(&ov1, sizeof(ov1));
            ov1.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            if(!WriteFile(hPipe, &dadosIniciais, sizeof(dadosServ2Cli), &nEscritos, &ov1)){
                if (GetLastError() == ERROR_IO_PENDING) {
                    _tprintf(TEXT("\nAgendado\n"));
                    WaitForSingleObject(ov1.hEvent, INFINITE);

                }
            }

            ZeroMemory(&ov, sizeof(ov));
            ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            if (!ReadFile(hPipe, &dadosRecebidos, sizeof(dadosCli2Serv), &nLidos, &ov)) {
                if (GetLastError() == ERROR_IO_PENDING) {
                    WaitForSingleObject(ov.hEvent, INFINITE);
                    GetOverlappedResult(hPipe, &ov, &nLidos, FALSE);
                    
                }
                else {
                    _tprintf(TEXT("Erro de ligacao\n"));
                    num = 0;
                    continue;
                }
            }
            numJogadores = dadosRecebidos.aux;
            if (numJogadores == 2) {
                _tprintf(TEXT("\nVamos esperar pelo 2 jogador -> NumJogadores = %d\n"), numJogadores);
                dadosT[0].namePipe = hPipe;
                num++;
                continue;
            }
            else if (numJogadores <= 0 || numJogadores > 2) {
                dadosIniciais.val = TERMINAR;
                WriteFile(hPipe, &dadosIniciais, sizeof(dadosServ2Cli), &nEscritos, NULL);
                //TODO Overlaped
            }
            else {
                dadosT[0].namePipe = hPipe;
            }
                
        }
        else {
            if(numJogadores == 2){
                //_tprintf(TEXT("Aqui"));
                dadosT[1].namePipe = hPipe;
                //copia(dadosIniciais.msg, TEXT("1"), (int)_tcslen(TEXT("1")));
                dadosIniciais.val = COMECAR;
                // WriteFile(hPipe, &dadosIniciais, sizeof(dadosJogo), &nEscritos, NULL);
            }
            else {
                dadosIniciais.val = TERMINAR;
            }
            WriteFile(hPipe, &dadosIniciais, sizeof(dadosServ2Cli), &nEscritos, NULL);
        }
        dadosIniciais.val = COMECAR;
        for (int i = 0; i < numJogadores; i++) {
            
            WriteFile(dadosT[i].namePipe, &dadosIniciais, sizeof(dadosIniciais), &nEscritos, NULL);
        }
        dadosT->shr->numJogadores = numJogadores;
        if ((hThreadJogo = CreateThread(NULL, 0, iniciaJogo, dadosT, 0, &idThreadJogo)) == NULL) {
            _tprintf(TEXT("Erro ao criar a thread para iniciar o jogo\n"));
            continue;
        }
        
        WaitForSingleObject(hThreadJogo, INFINITE);
        for (int i = 0; i < numJogadores; i++) {
            FlushFileBuffers(dadosT[i].namePipe);
            DisconnectNamedPipe(dadosT[i].namePipe);
            CloseHandle(dadosT[i].namePipe);
        }
        num = 0;
        if (sair(dadosT)) {
            break;
        }
        
    }
    _tprintf(TEXT("Sai do ciclo do pipe\n"));





    HANDLE threads[2] = { hThreadMonitor ,hThreadComandos };
    DWORD resultado = WaitForMultipleObjects(nthreads, threads, FALSE, INFINITE);
    
    if (resultado >= WAIT_OBJECT_0 && resultado < WAIT_OBJECT_0 + nthreads) {
        if ((resultado - WAIT_OBJECT_0) == 1) {
            ReleaseSemaphore(dadosComandos.sem_itens, 1, NULL);
            WaitForSingleObject(hThreadMonitor, INFINITE);
        }
        else {
            WaitForSingleObject(hThreadComandos, INFINITE);
        }
            
    }
    _tprintf(TEXT("Sai de tudo e vou fechar recursos\n"));
    closeResources(dadosT, &dadosComandos, &map);
    CloseHandle(hThreadComandos);
    CloseHandle(hThreadMonitor);
    CloseHandle(eventFinal);
    exit(0);
}



/*====================================Funcao que carrega um mapa predefinido=======================*/

void CarregaMapaWin(dadosThreadJogador* dadosT, int jogador) {
    if (dadosT->shr->nLinhas < 5 || dadosT->shr->nColunas < 5)
        return;

    dadosT->shr->jogador[jogador].tab[*dadosT->posXini][*dadosT->posYini] = TEXT(' ');
    dadosT->shr->jogador[jogador].tab[*dadosT->posXfinal][*dadosT->posYfinal] = TEXT(' ');
    *dadosT->posXini = dadosT->shr->nLinhas - 1;
    *dadosT->posYini = dadosT->shr->nColunas - 1;
    *dadosT->posXfinal = 0;
    *dadosT->posYfinal = 0;
    dadosT->shr->jogador[jogador].tab[*dadosT->posXini][*dadosT->posYini] = WATER[1];
    dadosT->shr->jogador[jogador].tab[*dadosT->posXfinal][*dadosT->posYfinal] = CHARACTERS[1];
    int posX = *dadosT->posXini;
    int posY = *dadosT->posYini;
    dadosT->shr->jogador[jogador].tab[--posX][posY] = CHARACTERS[3];
    for (int j = --posY; j > 4; j--)
        dadosT->shr->jogador[jogador].tab[posX][posY--] = CHARACTERS[0];
    dadosT->shr->jogador[jogador].tab[posX][posY] = CHARACTERS[5];
    for (int i = --posX; i >= 4; i--)
        dadosT->shr->jogador[jogador].tab[posX--][4] = CHARACTERS[1];
    dadosT->shr->jogador[jogador].tab[3][4] = CHARACTERS[3];
    dadosT->shr->jogador[jogador].tab[3][3] = CHARACTERS[0];
    dadosT->shr->jogador[jogador].tab[3][2] = CHARACTERS[0];
    dadosT->shr->jogador[jogador].tab[3][1] = CHARACTERS[0];
    dadosT->shr->jogador[jogador].tab[3][0] = CHARACTERS[5];
    dadosT->shr->jogador[jogador].tab[2][0] = BLOCO;
    dadosT->shr->jogador[jogador].tab[1][0] = CHARACTERS[1];
}

/*====================================Thread que lanca as threads dos jogadores e que comeca um novo jogo=======================*/
DWORD WINAPI iniciaJogo(LPVOID dados) {
    dadosThreadJogador* dadosT = (dadosThreadJogador*)dados;
    int terminou = 0;
    int nivel = 1;
    HANDLE hThreadJogador[MAXJOGADORES];
    DWORD idThreadJogador[MAXJOGADORES], nEscritos;
    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    int numJogadores = dadosT->shr->numJogadores;
    dadosT->shr->time = dadosT->time;
    ReleaseMutex(dadosT->mutexSHR);
    TCHAR msg[TAMMSG], resultado[10];
    dadosServ2Cli dadosEnviar[2];
    OVERLAPPED ov;
    LARGE_INTEGER li;

    while (terminou == 0) {
        dadosT->shr->msg[0] = TEXT('\0');
        *dadosT->ganhou = 0;
        *dadosT->perdeu = 0;
        for (int i = 0; i < MAXJOGADORES; i++) {
            dadosT[i].namepipeJogador[0] = TEXT('\0');
        }
        //Sleep(500);
        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        dadosT->shr->estado = 0;
        if (numJogadores == 2) {
            *dadosT->shr = limparTabuleiro(*dadosT->shr, 0, 0, dadosT->posXini, dadosT->posYini, dadosT->posXfinal, dadosT->posYfinal, CHARACTERS);
            dadosT[1].posXini = dadosT[0].posXini;
            dadosT[1].posYini = dadosT[0].posYini;
            dadosT[1].posXfinal = dadosT[0].posXfinal;
            dadosT[1].posYfinal = dadosT[0].posYfinal;

        }
        
        dadosT->shr->numJogadores = numJogadores;
        ReleaseMutex(dadosT->mutexSHR);
        SetEvent(dadosT->EventServ2Moni);
        ResetEvent(dadosT->EventServ2Moni);


        for (int i = 0; i < numJogadores; i++) {

            WaitForSingleObject(dadosT[i].mutexSHR, INFINITE);
            dadosT[i].numJogador = i;
            dadosT[i].shr->jogador[i].numjogador = i;         
            dadosT[i].shr->jogador[i].numeroJogadas = 0; //Numero de Jogadas
            dadosT[i].shr->jogador[i].pont = 0;
            dadosT[i].shr->nivel = 0;
            dadosT[i].shr->jogador[i].quantidadeDeParagem = 0;
            ReleaseMutex(dadosT[i].mutexSHR);


            hThreadJogador[i] = CreateThread(NULL, 0, joga, (LPVOID)&dadosT[i], 0, &idThreadJogador[i]);
            if (hThreadJogador[i] == NULL) {
                _tprintf(TEXT("Erro ao criar a thread %d\n"), i);
                break;
            }
            dadosT[i].hThread = hThreadJogador[i];
        }
        if (numJogadores == 1) {
            WaitForSingleObject(hThreadJogador[0], INFINITE);
        }
        else {
            DWORD res = WaitForMultipleObjects(numJogadores, hThreadJogador, FALSE, INFINITE);
            if (*dadosT->perdeu > 2) {
                li.QuadPart = -10000;
                SetWaitableTimer(dadosT[(res - WAIT_OBJECT_0 + 1) % 2].waitTimer, &li, 100, NULL, NULL, FALSE);
            }
            //SetEvent(dadosT[(res - WAIT_OBJECT_0 + 1) % 2].eventFinal);
            //Sleep(100);
            //ResetEvent(dadosT[(res - WAIT_OBJECT_0 + 1) % 2].eventFinal);
            WaitForSingleObject(hThreadJogador[(res - WAIT_OBJECT_0 + 1) % 2], INFINITE);

        }
       

        

        ResetEvent(dadosT->eventFinal);
        SetEvent(dadosT[0].EventServ2Moni);
        SetEvent(dadosT[1].EventServ2Moni);
        Sleep(100);
        ResetEvent(dadosT[0].EventServ2Moni);
        ResetEvent(dadosT[1].EventServ2Moni);

        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        dadosT->shr->estado = 0;
        ReleaseMutex(dadosT->mutexSHR);

        WaitForSingleObject(dadosT->mutexStruct, INFINITE);
        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        if (*dadosT->flagSair) {
            _tprintf(TEXT("O servidor mandou encerrar"));
            copia(msg, TEXT("encerrar"), (int)_tcslen(TEXT("encerrar")));
            for (int i = 0; i < numJogadores; i++) {
                dadosEnviar[i].val = ENCERRAR;
            }
        }
        else if (numJogadores == 2) {

            if (*dadosT->perdeu == 3) {
                dadosEnviar[1].val = DESISTIR;
                copia(msg, TEXT("O jogador "), (int)_tcslen(TEXT("O jogador ")));
                _itot_s(((*dadosT->perdeu - 2) % 2), resultado, sizeof(resultado) / sizeof(TCHAR), 10);
                concatena(msg, TEXT(" desistiu"), (int)_tcslen(TEXT(" desistiu")), (int)_tcslen(msg), TAMMSG);
            }
            else if (*dadosT->perdeu == 4) {
                dadosEnviar[0].val = DESISTIR;
                copia(msg, TEXT("O jogador "), (int)_tcslen(TEXT("O jogador ")));
                _itot_s(((*dadosT->perdeu - 2) % 2), resultado, sizeof(resultado) / sizeof(TCHAR), 10);
                concatena(msg, resultado, (int)_tcslen(resultado), (int)_tcslen(msg), TAMMSG);
                concatena(msg, TEXT(" desistiu"), (int)_tcslen(TEXT(" desistiu")), (int)_tcslen(msg), TAMMSG);
            }

            else if (*dadosT->perdeu == 0 && (dadosT->shr->jogador[0].pont == dadosT->shr->jogador[1].pont)) {
                //_tprintf(TEXT("%d - %d -pont: %d - %d"), *dadosT->ganhou, *dadosT->perdeu, dadosT->shr->jogador[0].pont, dadosT->shr->jogador[1].pont);
                copia(msg, TEXT("Empate"), (int)_tcslen(TEXT("Empate")));
                dadosEnviar[0].val = EMPATE;
                dadosEnviar[1].val = EMPATE;
            }
            else if (*dadosT->ganhou != 0) {
                //_tprintf(TEXT("ganhou %d"), *dadosT->ganhou);
                copia(msg, TEXT("Ganhou o jogador "), (int)_tcslen(TEXT("Ganhou o jogador ")));
                _itot_s((((*dadosT->ganhou)) -1), resultado, sizeof(resultado) / sizeof(TAMCHAR), 10);
                concatena(msg, resultado, (int)_tcslen(resultado), (int)_tcslen(msg), TAMMSG);
                dadosEnviar[(*dadosT->ganhou) - 1].val = GANHOU;
                dadosEnviar[(*dadosT->ganhou) % 2].val = PERDEU;
            }
            else if (*dadosT->perdeu != 0) {
                //_tprintf(TEXT("Perdeu %d"), *dadosT->perdeu);
                copia(msg, TEXT("Ganhou o jogador "), (int)_tcslen(TEXT("Ganhou o jogador ")));
                _itot_s((((*dadosT->perdeu)) % 2), resultado, sizeof(resultado)/sizeof(TAMCHAR), 10);
                concatena(msg, resultado, (int)_tcslen(resultado), (int)_tcslen(msg), TAMMSG);
                dadosEnviar[(*dadosT->perdeu) - 1].val = PERDEU;
                dadosEnviar[(*dadosT->perdeu) % 2].val = GANHOU;
            }

        }
        else {
            if (*dadosT->ganhou != 0) {
                copia(msg, TEXT("Ganhou"), (int)_tcslen(TEXT("Ganhou")));
                dadosEnviar[0].val = GANHOU;
            }
            else {
                copia(msg, TEXT("Perdeu"), (int)_tcslen(TEXT("Perdeu")));
                dadosEnviar[0].val = PERDEU;
            }
        }

        msg[_tcslen(msg)] = TEXT('\0');
        _tprintf(TEXT("%s"), msg);
        ReleaseMutex(dadosT->mutexStruct);
        
        copia(dadosT->shr->msg, msg, (int)_tcslen(msg));
        ReleaseMutex(dadosT->mutexSHR);
       
        SetEvent(dadosT[0].EventServ2Moni);
        SetEvent(dadosT[1].EventServ2Moni);
        Sleep(100);
        ResetEvent(dadosT[0].EventServ2Moni);
        ResetEvent(dadosT[1].EventServ2Moni);
        imprimeComandos();
        WaitForSingleObject(dadosT->mutexStruct, INFINITE);
        if (*dadosT->perdeu >= 3) {
            ZeroMemory(&ov, sizeof(ov));
            ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            WriteFile(dadosT[(*dadosT->perdeu - 2) % 2].namePipe, &dadosEnviar[(*dadosT->perdeu - 2) % 2], sizeof(dadosEnviar), &nEscritos, &ov);

        }else{
           
            for (int i = 0; i < numJogadores; i++) {
                ZeroMemory(&ov, sizeof(ov));
                ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                CopyMemory(&dadosEnviar[i].dados, dadosT->shr, sizeof(dadosJogo));
                dadosEnviar->jogador = i;
                dadosEnviar->xAgua = i;
                dadosEnviar->yAgua = i;
                dadosEnviar->ganhou = FALSE;
                WriteFile(dadosT[i].namePipe, &dadosEnviar[i], sizeof(dadosEnviar), &nEscritos, &ov);
            }
        }
        ReleaseMutex(dadosT->mutexStruct);
        break;
        //Ler do name pipe e ver quantos jogadores sao

    }
    for (int i = 0; i < numJogadores; i++) {
        CloseHandle(hThreadJogador[i]);
    }

    ExitThread(0);
}

/*=======================Funcao que faz com que a thread va para o singleplayer ou multiplayer============================*/

DWORD WINAPI joga(LPVOID d) {

    dadosThreadJogador* dadosT = (dadosThreadJogador*)d;

    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    int jogador = dadosT->numJogador;
    int numJogadores = dadosT->shr->numJogadores;
    int time = dadosT->shr->time;
    //_tprintf(TEXT("\nJogador %d\n\n"), dadosT->shr->jogador[jogador].numjogador);
    
    dadosT->shr->msg[0] = TEXT('\0');
    ReleaseMutex(dadosT->mutexSHR);

    if (numJogadores == 1) {
        jogaPorNiveis(dadosT, jogador, time);
    }
    else {
        //jogaPorNiveis(dadosT, jogador, 1);
        jogaMultiJogador(dadosT, jogador);
    }
    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    copia(dadosT->shr->msg, TEXT("sair"), (int)_tcslen(TEXT("sair")));
    dadosT->shr->estado = 0;
    ReleaseMutex(dadosT->mutexSHR);
    //_tprintf(TEXT("Sai do joga\n"));

    return 0;

}

BOOL colocaCelulaBranca(dadosJogo* tab, int x, int y, int jogador, int posxIni, int posyIni, int posXFinal, int posYFinal) {

    if (x == posxIni && y == posyIni)
        return FALSE;
    if (x == posXFinal && y == posYFinal)
        return FALSE;
    for (int i = 0; i < TAMCHAR; i++) {
        if (tab->jogador[jogador].tab[x][y] == CHARACTERS[i]) {
            tab->jogador[jogador].tab[x][y] = TEXT(' ');
            return TRUE;
        }
    }
    return FALSE;
}

/*===============================Funcao do jogador a single player==================================*/


DWORD jogaPorNiveis(dadosThreadJogador* dadosT, int jogador, int nJogos) {
    int nivel = 0;
    BOOL terminou = FALSE;
    int posx = 0, posy = 0;
    
    dadosJogo dados;
    HANDLE events[2];
    events[0] = dadosT->eventFinal;
    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    CopyMemory(&dados, dadosT->shr, sizeof(dadosJogo));
    dadosT->shr->nivel = 0;
    nivel = dadosT->shr->nivel;
    int numJogadores = dadosT->shr->numJogadores;
    //dadosT->shr->jogador[jogador].pont = 0;
    ReleaseMutex(dadosT->mutexSHR);
    int modo = *dadosT->modoJogo;
    int posXIni, posYIni, posXfinal, posYfinal;
    LARGE_INTEGER li;
    int time;
    int pont;
    DWORD idTimer, nLidos, nEnviados, resultWait;
    BOOL a = FALSE;
    dadosCli2Serv dadosReceber;
    OVERLAPPED ov;
    dadosServ2Cli dadosEnviar;
    dadosEnviar.jogador = jogador;
    dadosEnviar.dados.modoJogo = *dadosT->modoJogo;
    while ((nJogos--)) {
        

        dados = limparTabuleiro(dados, modo, jogador, &posXIni, &posYIni, &posXfinal, &posYfinal, CHARACTERS);
        WaitForSingleObject(dadosT->mutexStruct, INFINITE);
        *dadosT->posXini = posXIni;
        *dadosT->posYini = posYIni;
        *dadosT->posXfinal = posXfinal;
        *dadosT->posYfinal = posYfinal; 
        *dadosT->ganhou = 0;
        *dadosT->perdeu = 0;
        ReleaseMutex(dadosT->mutexStruct);
        
        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        pont = dadosT->shr->jogador[jogador].pont;
        CopyMemory(dadosT->shr->jogador + jogador, &dados.jogador[jogador], sizeof(tabuleiros));
        dadosT->shr->jogador[jogador].pont = pont;
        time = dadosT->shr->time;
        //CarregaMapaWin(dadosT, jogador); // Carrega mapa de jogo pre definido para ganhar
        dadosT->shr->msg[0] = TEXT('\0');
        dadosT->shr->estado = 1;
        //imprimeTab((*dadosT->shr), 0);
        ReleaseMutex(dadosT->mutexSHR);

        dadosT->hThreadTimer = CreateThread(NULL, 0, colocaAgua, dadosT, CREATE_SUSPENDED, &idTimer);
        if (dadosT->hThreadTimer == NULL) {
            _tprintf(TEXT("Nao criou a thread da agua\n"));
            ExitThread(0);
        }
        li.QuadPart = (long long) -time * 10000000;
        //_tprintf(TEXT("\nTime: %d"), time);

        SetWaitableTimer(dadosT->waitTimer, &li, (time * 1000), NULL, NULL, FALSE);
        ResumeThread(dadosT->hThreadTimer);
        
        

        CopyMemory(&dadosEnviar.dados, dadosT->shr, sizeof(dados));

        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        events[1] = ov.hEvent;
        dadosEnviar.val = JOGADAVALIDA;
        if (!WriteFile(dadosT->namePipe, &dadosEnviar, sizeof(dadosEnviar), &nEnviados, &ov)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                resultWait = WaitForMultipleObjects(2, events, FALSE, INFINITE);
               
               
                GetOverlappedResult(dadosT->namePipe, &ov, &nEnviados, FALSE);
                
            }
        }
        if (nEnviados != sizeof(dadosEnviar)) {
            
        }
        
        while (1) {
            ZeroMemory(&ov, sizeof(ov));
            ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            events[1] = ov.hEvent;
            SetEvent(dadosT->EventServ2Moni);
            Sleep(100);
            ResetEvent(dadosT->EventServ2Moni);

            if (!ReadFile(dadosT->namePipe, &dadosReceber, sizeof(dadosReceber), &nLidos, &ov)) {
                if (GetLastError() == ERROR_IO_PENDING) {
                   resultWait = WaitForMultipleObjects( (DWORD)2, events, FALSE, 15000);
                   if (resultWait == WAIT_TIMEOUT) {
                       nJogos = 0;
                       WaitForSingleObject(dadosT->mutexSair, INFINITE);
                       *dadosT->perdeu = 1;
                      
                       _tprintf(TEXT("TIMEOUT\n"));
                       
                       ReleaseMutex(dadosT->mutexSair);
                   }
                   if (resultWait - WAIT_OBJECT_0 == 0) {
                       GetOverlappedResult(dadosT->namePipe, &ov, &nLidos, FALSE);
                   }
                  

                }
                else {
                    
                    WaitForSingleObject(dadosT->mutexStruct, INFINITE);
                    *dadosT->perdeu = 1;
                    ReleaseMutex(dadosT->mutexStruct);
                }
            }

            if (sair(dadosT)) {
                nJogos = 0;
                //Por mesnagemno campo mensagem;
                break;
            }
            if (perdeu(dadosT)) {
                
                nJogos = 0;
                break;
            }
            if (ganhou(dadosT)) {
                ResetEvent(dadosT->eventFinal);
                break;
            }
            if (dadosReceber.val == SINALVIDA) {
                _tprintf(TEXT("Sinal de vida recebido do jogador %d\n"), jogador);
                continue;
            }
            else if (dadosReceber.val == DESISTIR) {
                _tprintf(TEXT("O jogador desistiu %d\n"), jogador);
                WaitForSingleObject(dadosT->mutexStruct, INFINITE);
                *dadosT->perdeu = jogador+1;
                nJogos = 0;
                li.QuadPart = -1000000;
                SetWaitableTimer(dadosT->waitTimer, &li, 100, NULL, NULL, FALSE);
                ReleaseMutex(dadosT->mutexStruct);
                break;
                
            }
            WaitForSingleObject(dadosT->mutexSHR, INFINITE);
            dadosT->shr->modoJogo = *dadosT->modoJogo;
            CopyMemory(&dados, dadosT->shr, sizeof(dadosJogo));
            ReleaseMutex(dadosT->mutexSHR);
            if (dadosReceber.val == MOUSEOVER) {
                WaitForSingleObject(dadosT->mutexSHR, INFINITE);
                if (dadosT->shr->jogador[jogador].quantidadeDeParagem < 3) {
                    dadosT->shr->jogador[jogador].quantidadeDeParagem++;
                    CancelWaitableTimer(dadosT[jogador].waitTimer);
                }
                ReleaseMutex(dadosT->mutexSHR);
                continue;
            }
            else if (dadosReceber.val == MOUSEMOVE) {
                WaitForSingleObject(dadosT->mutexStruct, INFINITE);
                
                li.QuadPart = time * 10000000;
                ReleaseMutex(dadosT->mutexStruct);
                SetWaitableTimer(dadosT[jogador].waitTimer, &li, time*1000, NULL, NULL, FALSE);
                continue;
            }
            else if (dadosReceber.val == RIGHT) {
                if (colocaCelulaBranca(&dados, dadosReceber.x, dadosReceber.y, jogador, *dadosT->posXini, *dadosT->posYini, *dadosT->posXfinal, *dadosT->posYfinal)) {
                    dadosEnviar.val = JOGADAVALIDA;
                }
                else {
                    _tprintf(TEXT("Jogada invalidas"));
                    dadosEnviar.val = JOGADAINVALIDA;
                }
            }
            else{
                
                if (!verificaPosicao(dados, jogador, dadosReceber.x, dadosReceber.y, *dadosT->posXfinal, *dadosT->posYfinal, CHARACTERS)) {
                    _tprintf(TEXT("Posicoes invalidas"));
                    dadosEnviar.val = JOGADAINVALIDA;
                }else{
                    dadosEnviar.val = JOGADAVALIDA;
                    dados = colocaJogada(dados, *dadosT->modoJogo, jogador, dadosReceber.x, dadosReceber.y, CHARACTERS);
                }
            }
            //ReleaseMutex(dadosT->mutexAcessoAoEcra);
            //dados = colocaJogada(dados, *dadosT->modoJogo, jogador, dadosReceber.x, dadosReceber.y, CHARACTERS);
            WaitForSingleObject(dadosT->mutexSHR, INFINITE);
            CopyMemory(dadosT->shr, &dados, sizeof(dados));
            CopyMemory(&dadosEnviar.dados, &dados, sizeof(dados));
            ReleaseMutex(dadosT->mutexSHR);
            ZeroMemory(&ov, sizeof(ov));
            ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            events[1] = ov.hEvent;
            dadosEnviar.dados.modoJogo = *dadosT->modoJogo;
            if (!WriteFile(dadosT->namePipe, &dadosEnviar, sizeof(dadosEnviar), &nEnviados, &ov)) {
                if (GetLastError() == ERROR_IO_PENDING) {
                    resultWait = WaitForMultipleObjects( 2, events, FALSE, INFINITE);
                    //if (resultWait - WAIT_OBJECT_0 == 0) {
                    //    _tprintf(TEXT("Agendado"));
                    //    GetOverlappedResult(dadosT->namePipe, &ov, &nEnviados, FALSE);
                    //}
                    GetOverlappedResult(dadosT->namePipe, &ov, &nEnviados, FALSE);
                }
                else {
                    _tprintf(TEXT("Erro na leitura"));
                    WaitForSingleObject(dadosT->mutexStruct, INFINITE);
                    *dadosT->perdeu = 1;
                    nJogos = 0;
                    ReleaseMutex(dadosT->mutexStruct);
                    break;
                }
            }
            if (nEnviados != sizeof(dadosEnviar)) {
                //_tprintf(TEXT("Problemas no envio"));
            }
            //imprimeTab(dados, jogador);
            

        }
        //_tprintf(TEXT("À espera da agua\n"));
        

       
        WaitForSingleObject(dadosT->hThreadTimer, INFINITE);
        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        dadosT->shr->time = dadosT->shr->time - 1;
        if (ganhou) {
            dadosT->shr->nivel++;
        }
            
        ReleaseMutex(dadosT->mutexSHR);
        
        a = TRUE;
        CancelWaitableTimer(dadosT->waitTimer);
        CloseHandle(dadosT->hThreadTimer);
        Sleep(1000);
    }
   //_tprintf(TEXT("Vou sair do singleplayer\n"));
   return 0;
}

/*===================================Funcao do jogador a multi player======================================*/

DWORD jogaMultiJogador(dadosThreadJogador* dadosT, int jogador) {
    int nivel = 0;
    BOOL terminou = FALSE;
    int posx = 0, posy = 0;
    HANDLE events[2];
    events[0] = dadosT->eventFinal;
    dadosJogo dados;
    dadosCli2Serv dadosReceber;
    OVERLAPPED ov;
    DWORD resultWait, nEscritos, nLidos;
    dadosServ2Cli dadosEnviar;
    dadosEnviar.jogador = jogador;

    WaitForSingleObject(dadosT->mutexSHR, INFINITE);
    CopyMemory(&dados, dadosT->shr, sizeof(dadosJogo));
    int numJogadores = dadosT->shr->numJogadores;
    int time = dadosT->shr->time;
    dadosT->shr->jogador[jogador].pont = 0;
    dadosT->shr->estado = 1;
    
    //CarregaMapaWin(dadosT, jogador); // Carrega mapa de jogo pre definido para ganhar
    ReleaseMutex(dadosT->mutexSHR);
    
    LARGE_INTEGER li;
    DWORD idTimer;
    dadosT->hThreadTimer = CreateThread(NULL, 0, colocaAgua, dadosT, CREATE_SUSPENDED, &idTimer);
    if (dadosT->hThreadTimer == NULL) {
        _tprintf(TEXT("Nao criou a thread da agua\n"));
        ExitThread(0);
    }
    li.QuadPart = -time * 10000000;
    //_tprintf(TEXT("\nTime: %d"), time);
    SetWaitableTimer(dadosT->waitTimer, &li, (time * 1000), NULL, NULL, FALSE);
    ResumeThread(dadosT->hThreadTimer);
    dadosEnviar.dados.modoJogo = *dadosT->modoJogo;
    //_tprintf(TEXT("jogador: %d"), jogador);

    CopyMemory(&dadosEnviar.dados, dadosT->shr, sizeof(dadosEnviar.dados));
    ZeroMemory(&ov, sizeof(ov));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    events[1] = ov.hEvent;
    dadosEnviar.val = JOGADAVALIDA;
   
    if (!WriteFile(dadosT->namePipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            resultWait = WaitForMultipleObjects(2, events, FALSE, INFINITE);
            if (resultWait - WAIT_OBJECT_0 == 1) {
                GetOverlappedResult(dadosT->namePipe, &ov, &nEscritos, FALSE);
            }
            else {
                _tprintf(TEXT("Erro"));

            }

        }
    }
    if (nEscritos != sizeof(dadosEnviar)) {
       // _tprintf(TEXT("Problemas no envio"));
        
    }
    
    while (1) {
      
        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        events[1] = ov.hEvent;
        SetEvent(dadosT->EventServ2Moni);
        Sleep(100);
        ResetEvent(dadosT->EventServ2Moni);

        if (!ReadFile(dadosT->namePipe, &dadosReceber, sizeof(dadosReceber), &nLidos, &ov)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                resultWait = WaitForMultipleObjects((DWORD)2, events, FALSE, 15000);
                if (resultWait == WAIT_TIMEOUT) {
                    WaitForSingleObject(dadosT->mutexSair, INFINITE);
                    *dadosT->perdeu = jogador + 3;

                    _tprintf(TEXT("TIMEOUT\n"));

                    ReleaseMutex(dadosT->mutexSair);
                }
                if (resultWait - WAIT_OBJECT_0 == 0) {
                    GetOverlappedResult(dadosT->namePipe, &ov, &nLidos, FALSE);
                }


            }
            else {

                WaitForSingleObject(dadosT->mutexStruct, INFINITE);
                *dadosT->perdeu = jogador + 3;
                ReleaseMutex(dadosT->mutexStruct);
            }
        }

        if (sair(dadosT)) {
            break;
        }
        if (perdeu(dadosT)) {
            break;
        }
        if (ganhou(dadosT)) {
            ResetEvent(dadosT->eventFinal);
            break;
        }
        if (dadosReceber.val == SINALVIDA) {
            _tprintf(TEXT("Sinal de vida recebido do jogador %d\n"), jogador);
            continue;
        }
        else if (dadosReceber.val == DESISTIR) {
            _tprintf(TEXT("O jogador desistiu %d\n"), jogador);
            WaitForSingleObject(dadosT->mutexStruct, INFINITE);
            *dadosT->perdeu = jogador + 3;
            li.QuadPart = -1000000;
            SetWaitableTimer(dadosT->waitTimer, &li, 100, NULL, NULL, FALSE);
            ReleaseMutex(dadosT->mutexStruct);
            break;

        }
        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        dadosT->shr->modoJogo = *dadosT->modoJogo;
        CopyMemory(&dados, dadosT->shr, sizeof(dadosJogo));
        ReleaseMutex(dadosT->mutexSHR);
        if (dadosReceber.val == MOUSEOVER) {
            WaitForSingleObject(dadosT->mutexSHR, INFINITE);
            if (dadosT->shr->jogador[jogador].quantidadeDeParagem < 3) {
                dadosT->shr->jogador[jogador].quantidadeDeParagem++;
                CancelWaitableTimer(dadosT->waitTimer);
            }
            ReleaseMutex(dadosT->mutexSHR);
            continue;
        }
        else if (dadosReceber.val == MOUSEMOVE) {
            WaitForSingleObject(dadosT->mutexStruct, INFINITE);

            li.QuadPart = time * 10000000;
            ReleaseMutex(dadosT->mutexStruct);
            SetWaitableTimer(dadosT->waitTimer, &li, time * 1000, NULL, NULL, FALSE);
            continue;
        }
        else if (dadosReceber.val == RIGHT) {
            if (colocaCelulaBranca(&dados, dadosReceber.x, dadosReceber.y, jogador, *dadosT->posXini, *dadosT->posYini, *dadosT->posXfinal, *dadosT->posYfinal)) {
                dadosEnviar.val = JOGADAVALIDA;
            }
            else {
                _tprintf(TEXT("Jogada invalidas"));
                dadosEnviar.val = JOGADAINVALIDA;
            }
        }
        else {

            if (!verificaPosicao(dados, jogador, dadosReceber.x, dadosReceber.y, *dadosT->posXfinal, *dadosT->posYfinal, CHARACTERS)) {
                _tprintf(TEXT("Posicoes invalidas"));
                dadosEnviar.val = JOGADAINVALIDA;
            }
            else {
                dadosEnviar.val = JOGADAVALIDA;
                dados = colocaJogada(dados, *dadosT->modoJogo, jogador, dadosReceber.x, dadosReceber.y, CHARACTERS);
            }
        }
        //ReleaseMutex(dadosT->mutexAcessoAoEcra);
        //dados = colocaJogada(dados, *dadosT->modoJogo, jogador, dadosReceber.x, dadosReceber.y, CHARACTERS);
        WaitForSingleObject(dadosT->mutexSHR, INFINITE);
        CopyMemory(dadosT->shr, &dados, sizeof(dados));
        CopyMemory(&dadosEnviar.dados, &dados, sizeof(dados));
        ReleaseMutex(dadosT->mutexSHR);
        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        events[1] = ov.hEvent;
        dadosEnviar.dados.modoJogo = *dadosT->modoJogo;
        if (!WriteFile(dadosT->namePipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                resultWait = WaitForMultipleObjects(2, events, FALSE, INFINITE);
                //if (resultWait - WAIT_OBJECT_0 == 0) {
                //    _tprintf(TEXT("Agendado"));
                //    GetOverlappedResult(dadosT->namePipe, &ov, &nEnviados, FALSE);
                //}
                GetOverlappedResult(dadosT->namePipe, &ov, &nEscritos, FALSE);
            }
            else {
                _tprintf(TEXT("Erro na leitura"));
                WaitForSingleObject(dadosT->mutexStruct, INFINITE);
                *dadosT->perdeu = 1;
                
                ReleaseMutex(dadosT->mutexStruct);
                break;
            }
        }
        if (nEscritos != sizeof(dadosEnviar)) {
            //_tprintf(TEXT("Problemas no envio"));
        }
        //imprimeTab(dados, jogador);


    }
    //_tprintf(TEXT("A espera da thread de agua %d\n"), jogador);
    li.QuadPart = -1000;
    SetWaitableTimer(dadosT->waitTimer,&li,100, NULL, NULL, FALSE);

    WaitForSingleObject(dadosT->hThreadTimer, INFINITE);
    //_tprintf(TEXT("Vou sair do jogo %d\n"), jogador);
    CloseHandle(dadosT->hThreadTimer);
    return 0;
}

