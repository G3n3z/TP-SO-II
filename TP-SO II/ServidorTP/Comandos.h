#pragma once
#include "ServidorFuncoes.h"

DWORD WINAPI threadComandos(LPVOID dados);
void imprimeComandos();
DWORD WINAPI threadMonitor(LPVOID d);
void listaJogadores(dadosThreadJogador* dados);
void suspendeJogo(dadosThreadJogador* dados);
void retomaJogo(dadosThreadJogador* dados);