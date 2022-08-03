#pragma once

#include "../ServidorTP/Servidor_Monitor.h"
#define NAMEMUTEXPRODUTORES			TEXT("mutextShrP")
#define TAMCHAR						6
extern TCHAR WATER[TAMCHAR] = { 0x2501, 0x2503, 0x250F, 0x2513, 0x251B, 0x2517 };
extern TCHAR BLOCO = 0x2592;

typedef struct dados dadosMonitor;

struct dados{
	dadosJogo* shr;
	HANDLE mutexSHR;
	HANDLE EventServ2Moni;
	int* flagSair;
	int jogador;
	HANDLE mutexSair;
	HANDLE mutexEcra;
	
};

typedef struct {
	bufferCircular* buffer;
	HANDLE sem_vazios;
	HANDLE sem_itens;
	HANDLE mutexShr;
	HANDLE mutexEcra;
	dadosMonitor* dadosT;
}dadosThreadComandos;
