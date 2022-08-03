#include "Comandos.h"

void imprimeComandos() {
    _tprintf(TEXT("\n\nsuspender -> Suspende o jogo a  decorrer\n"));
    _tprintf(TEXT("retomar -> Retoma o jogo suspenso\n"));
    _tprintf(TEXT("listar -> Lista os jogadores no momento\n"));
    _tprintf(TEXT("iniciar jogo -> iniciar um novo jogo\n"));
    _tprintf(TEXT("encerrar -> encerrar tudo\n\n"));

}

/*====================================Funcoes usadas nas threads de leitura de comandos do teclado da parte do servidor=======================*/
void retomaJogo(dadosThreadJogador* dados) {
    WaitForSingleObject(dados->mutexStruct, INFINITE);
    WaitForSingleObject(dados->mutexSHR, INFINITE);
    int i;
    LARGE_INTEGER li;
    OVERLAPPED ov;
    DWORD nEscritos;
    dadosServ2Cli dadosEnviar;
    if (dados->shr->estado == 2) {
        for (i = 0; i < dados->shr->numJogadores; i++)
        {
            if (dados[i].shr->estado == 2) {
                ResumeThread(dados[i].hThread);
                ResumeThread(dados[i].hThreadTimer);
                li.QuadPart = -dados[i].shr->time * 10000000;
                SetWaitableTimer(dados[i].waitTimer, &li, dados[i].shr->time * 1000, NULL, NULL, FALSE);
                ZeroMemory(&ov, sizeof(ov));
                ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                dadosEnviar.val = RETOMAR;
                WriteFile(dados[i].namePipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov);
            }
        }
        dados->shr->estado = 1;
        _tprintf(TEXT("Jogo Retomado\n"));
    }
    else {
        _tprintf(TEXT("O jogo não se encontra suspenso\n"));
    }

    ReleaseMutex(dados->mutexSHR);
    ReleaseMutex(dados->mutexStruct);
}

void suspendeJogo(dadosThreadJogador* dados) {
   
    OVERLAPPED ov;
    DWORD nEscritos;
    dadosServ2Cli dadosEnviar;
    WaitForSingleObject(dados->mutexStruct, INFINITE);
    WaitForSingleObject(dados->mutexSHR, INFINITE);

    int i;
    DWORD count = 0;
    if (dados->shr->estado == 1) {
        for (i = 0; i < dados->shr->numJogadores; i++)
        {
            count = SuspendThread(dados[i].hThread);
            count = SuspendThread(dados[i].hThreadTimer);
            ZeroMemory(&ov, sizeof(ov));
            ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            dadosEnviar.val = SUSPENDER;
            WriteFile(dados[i].namePipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov);
        }
        dados->shr->estado = 2;
    }
    else if (dados->shr->estado == 0) {
        _tprintf(TEXT("O jogo ainda nao comecou\n"));
    }
    else {
        _tprintf(TEXT("O jogo ja esta suspenso\n"));
    }

    ReleaseMutex(dados->mutexSHR);
    ReleaseMutex(dados->mutexStruct);
}

void listaJogadores(dadosThreadJogador* dados) {
    WaitForSingleObject(dados->mutexStruct, INFINITE);
    WaitForSingleObject(dados->mutexSHR, INFINITE);
    int i;
    if (dados->shr->estado > 0) {
        for (i = 0; i < dados->shr->numJogadores; i++)
        {
            _tprintf(TEXT("Jogador %d com pontuacao %d\n"), dados[i].numJogador, dados[i].shr->jogador[i].pont);
        }
    }
    else
        _tprintf(TEXT("De momento não existem jogadores"));
    ReleaseMutex(dados->mutexSHR);
    ReleaseMutex(dados->mutexStruct);
}



DWORD WINAPI threadComandos(LPVOID dados) {
    dadosThreadJogador* dadosT = (dadosThreadJogador*)dados;
    TCHAR comando[TAMMSG];
    HANDLE hThreadJogo = NULL;
    DWORD  nEscritos;
    LARGE_INTEGER li;
    dadosServ2Cli dadosEnviar;
    OVERLAPPED ov;
    do {
        //WaitForSingleObject(dadosT->mutexAcessoAoEcra, INFINITE);
        imprimeComandos();
        _tprintf(TEXT("\nQual o comando a executar: "));
        _fgetts(comando, TAMMSG - 1, stdin);
        comando[_tcslen(comando) - 1] = '\0';
        //ReleaseMutex(dadosT->mutexAcessoAoEcra);

        if (!_tcscmp(comando, TEXT("encerrar"))) {
            WaitForSingleObject(dadosT->mutexSair, INFINITE);
            *dadosT->flagSair = 1;
            ReleaseMutex(dadosT->mutexSair);
            WaitForSingleObject(dadosT->mutexSHR, INFINITE);
            copia(dadosT->shr->msg, comando, (int)_tcslen(comando));
            if (dadosT->shr->estado == 2) {
                retomaJogo(dadosT);
            }
            ReleaseMutex(dadosT->mutexSHR);
            SetEvent(dadosT->eventFinal);
            li.QuadPart = -1 * 1000000;

            for (int k = 0; k < MAXJOGADORES; k++) {
                //_tprintf(TEXT("Enviou evento 1"));
                SetEvent(dadosT[k].EventServ2Moni);
                //Sleep(100);
                ResetEvent(dadosT[k].EventServ2Moni);
                SetWaitableTimer(dadosT[k].waitTimer, &li, 1000, NULL, NULL, FALSE);

            }
            WaitForSingleObject(dadosT->mutexSHR, INFINITE);
            if (dadosT->shr->estado == 1) {

                ReleaseMutex(dadosT->mutexSHR);
                if (hThreadJogo != NULL)
                    WaitForSingleObject(hThreadJogo, INFINITE);
                for (int k = 0; k < MAXJOGADORES; k++) {
                    ZeroMemory(&ov, sizeof(ov));
                    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                    dadosEnviar.val = ENCERRAR;
                    WriteFile(dadosT[k].namePipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov);
                }
                break;
            }
            ReleaseMutex(dadosT->mutexSHR);

            break;
        }
        else if (!_tcscmp(comando, TEXT("listar"))) {
            _tprintf(TEXT("listar\n"));
            listaJogadores(dadosT);
        }
        else if (!_tcscmp(comando, TEXT("suspender"))) {
            _tprintf(TEXT("suspender\n"));
            suspendeJogo(dadosT);
        }
        else if (!_tcscmp(comando, TEXT("retomar"))) {
            _tprintf(TEXT("retomar\n"));
            retomaJogo(dadosT);
        }
        else {
            _tprintf(TEXT("Comando Invalido\n"));

        }

        //SetEvent(dadosT->EventServ2Moni);
        //ResetEvent(dadosT->EventServ2Moni);
    } while (1);
    CloseHandle(hThreadJogo);
    SetEvent(dadosT->ov.hEvent);
    //_tprintf(TEXT("THREAD COMANDOS ACABOU\n"));
    return 0;
}



/*====================================Thread que recebe informacao do buffercircular=======================*/

DWORD WINAPI threadMonitor(LPVOID d) {
    dadosThreadComandos* dados = (dadosThreadComandos*)d;
    //_tprintf(TEXT("Iniciou a thread\n"));
    struct MSGbufferCircular stMsg;
    LARGE_INTEGER li;
    while (1) {
        WaitForSingleObject(dados->sem_itens, INFINITE);
        WaitForSingleObject(dados->mutexShr, INFINITE);
        //_tprintf(TEXT("\nRecebeu comando do monitor\n"));

        CopyMemory(&stMsg, dados->buffer->buff + dados->buffer->out, sizeof(stMsg));
        dados->buffer->out = (dados->buffer->out + 1) % TAMBUFFCIRC;
        ReleaseMutex(dados->mutexShr);
        ReleaseSemaphore(dados->sem_vazios, 1, NULL);

        WaitForSingleObject(dados->dadosT->mutexSair, INFINITE);
        if (*dados->dadosT->flagSair) {
            ReleaseMutex(dados->dadosT->mutexSair);
            break;
        }
        ReleaseMutex(dados->dadosT->mutexSair);

        if (!_tcscmp(stMsg.msg, TEXT("parar agua"))) { //parar agua [jogador] [segundos]
            _tprintf(TEXT("O monitor mandou parar a agua %d segundos no jogador %d\n"), stMsg.x, stMsg.player);
            WaitForSingleObject(dados->dadosT->mutexSHR, INFINITE);
            li.QuadPart = -(long long)stMsg.x * 10000000;
            SetWaitableTimer(dados->dadosT[stMsg.player].waitTimer, &li, (dados->dadosT[stMsg.player].shr->time * 1000), NULL, NULL, FALSE);
            ReleaseMutex(dados->dadosT->mutexSHR);
        }
        else if (!_tcscmp(stMsg.msg, TEXT("insere bloco"))) { // insere bloco [jogador] [pos x] [pos y]
            _tprintf(TEXT("O monitor mandou inserir um bloco na celula %d,%d do jogador %d\n"), stMsg.x, stMsg.y, stMsg.player);
            WaitForSingleObject(dados->dadosT->mutexSHR, INFINITE);
            if (stMsg.x >= 0 || stMsg.x < dados->dadosT->shr->nLinhas && stMsg.y >= 0 || stMsg.y < dados->dadosT->shr->nColunas)
                dados->dadosT->shr->jogador[stMsg.player].tab[stMsg.x][stMsg.y] = BLOCO;
            ReleaseMutex(dados->dadosT->mutexSHR);
        }
        else if (!_tcscmp(stMsg.msg, TEXT("ativa aleatorio"))) {
            _tprintf(TEXT("O monitor mandou ativa o modo aleatorio"));
            WaitForSingleObject(dados->dadosT->mutexStruct, INFINITE);
            *dados->dadosT->modoJogo = 0;
            ReleaseMutex(dados->dadosT->mutexStruct);
        }
        else if (!_tcscmp(stMsg.msg, TEXT("desativa aleatorio"))) {
            _tprintf(TEXT("O monitor mandou desativar o modo aleatorio"));
            WaitForSingleObject(dados->dadosT->mutexStruct, INFINITE);
            *dados->dadosT->modoJogo = 1;
            ReleaseMutex(dados->dadosT->mutexStruct);
        }
    }
    //_tprintf(TEXT("Thread monitor acabou\n"));

    //Esta thread vai esperar que um evento seja despertado pelo monitor para fazer alguma a??o

    return 0;
}