#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include "../ServidorTP/servidor_cliente.h"
#include "../ServidorTP/Servidor_Monitor.h"

OVERLAPPED preparaOverlap(OVERLAPPED ov) {
    ZeroMemory(&ov, sizeof(ov));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    return ov;
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



int _tmain(int argc, TCHAR* argv[]) {


    HANDLE hPipe;

#ifdef UNICODE 
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    _tprintf(TEXT("Esperar pelo pipe '%s' (WaitNamedPipe)\n"),
        NAMEPIPE);

    if (!WaitNamedPipe(NAMEPIPE, 10000)) {
        _tprintf(TEXT("Timeout namedPipe nao disponivel -> %s\n"), NAMEPIPE);
        exit(-1);
    }


    hPipe = CreateFile(NAMEPIPE, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL| FILE_FLAG_OVERLAPPED, NULL);

    if (hPipe == NULL) {
        _tprintf(TEXT("[ERRO] Ligar ao pipe '%s'! (CreateFile)\n"), NAMEPIPE);
        exit(-1);
    }
    _tprintf(TEXT("Liguei me"));

    DWORD modoAcesso = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(hPipe, &modoAcesso, NULL, NULL)) {
        _tprintf(TEXT("Nao foi possivel mudar o modo de acesso"));
        exit(1);
    }
    
    dadosServ2Cli dados;
    DWORD nLidos, nEscritos;
    OVERLAPPED ov;
    dadosCli2Serv dadosEnviar;
    ZeroMemory(&ov, sizeof(ov));
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!ReadFile(hPipe, &dados, sizeof(dados), &nLidos, &ov)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            _tprintf(TEXT("A espera\n"));
            WaitForSingleObject(ov.hEvent, INFINITE);
            GetOverlappedResult(hPipe, &ov, &nLidos, FALSE);
           
        }
        else {
            _tprintf(TEXT("Erro ao agendar"));
            exit(-1);
        }
    }


    if (nLidos != sizeof(dados)) {
        _tprintf(TEXT("Apenas foram lidos %d de %d"), (int)nLidos, (int)sizeof(dados));
        exit(-1);
    }
    if (dados.val == ESPERAR) {
        _tprintf(TEXT("Sou o jogador 1 e vou dizer quandos jogam\n"));
        _tprintf(TEXT("Num Jogadores: "));
        _tscanf_s(TEXT("%d"), &dadosEnviar.aux);
        //dados.numJogadores = 2;
        ov = preparaOverlap(ov);
        if (!WriteFile(hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(ov.hEvent, INFINITE);
                GetOverlappedResult(hPipe, &ov, &nEscritos, FALSE);
            }
        }
    }
    else if (dados.val == COMECAR) {
        _tprintf(TEXT("Estao à minha espera\n"));
    }
    else if (dados.val == TERMINAR) {
        _tprintf(TEXT("Não é possivel jogar"));
        exit(-1);
    }

    ov = preparaOverlap(ov);
    if (!ReadFile(hPipe, &dados, sizeof(dados), &nLidos, &ov)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            WaitForSingleObject(ov.hEvent, INFINITE);
        }
    }

    if (!(dados.val == COMECAR)) {
        exit(-1);
    }
    //HANDLE hThread = CreateThread(NULL, 0,,hPipe );
    
    while (1) {
        _tprintf(TEXT("Pos x: "));
        _tscanf_s(TEXT("%d"), &dadosEnviar.x);
        _tprintf(TEXT("Pos y: "));
        _tscanf_s(TEXT("%d"), &dadosEnviar.y);

        ov = preparaOverlap(ov);
        if (!WriteFile(hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(ov.hEvent, INFINITE);
                GetOverlappedResult(hPipe,&ov,&nEscritos,FALSE);
            }
            else {
                _tprintf(TEXT("Existiu no agendamento"));
            }
        }
        if (nEscritos != sizeof(dadosEnviar)) {
            _tprintf(TEXT("Existiu um problema"));
        }

        ov = preparaOverlap(ov);
        if (!ReadFile(hPipe, &dados, sizeof(dados), &nLidos, &ov)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(ov.hEvent, INFINITE);
                GetOverlappedResult(hPipe, &ov, &nLidos, FALSE);
            }
            else {
                _tprintf(TEXT("Existiu no agendamento"));
            }
        }
        if (nLidos != sizeof(dados)) {
            _tprintf(TEXT("Existiu um problema na leitura %d"),(int)nLidos);
        }
        else {
            imprimeTab(dados.dados, 0);
        }
        

    }

    //_tprintf(_T("Mensagem Recebida: %s"), dados.msg);
    Sleep(30000);
}