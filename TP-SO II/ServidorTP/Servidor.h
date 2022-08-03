#pragma once
#include "Servidor_Monitor.h"
#define NAMEMUTEXCONSUMIDORES	TEXT("mutextShrC")
#define REGLINHAS				TEXT("Linhas")
#define REGCOLUNAS				TEXT("Colunas")
#define REGTEMPO				TEXT("Tempo")
#define TAMMSG					256

#define TAMMAX					20
#define TAMCHAR					6


#define TAMSHR sizeof(dadosJogo)
#define SIZEBUFFER sizeof(bufferCircular)

static const TCHAR CHARACTERS[6] = { 0x2550, 0x2551, 0x2554, 0x2557, 0x255D, 0x255A };
static const TCHAR WATER[6] = { 0x2501, 0x2503, 0x250F, 0x2513, 0x251B, 0x2517 };
static const TCHAR BLOCO = 0x2592;

typedef struct dadosThread dadosThreadJogador;

struct dadosThread{
	int *posXini, *posYini, *posXfinal, *posYfinal;
	int* perdeu;
	int* ganhou;
	int* modoJogo; // 0 - random  1 - seguidas
	int* flagSair;
	int pid;
	int numJogador;
	int time;
	int sinalVida;
	dadosJogo* shr;
	HANDLE mutexSHR;
	HANDLE EventServ2Moni;
	HANDLE mutexSair;
	HANDLE mutexStruct;
	HANDLE hThread;
	TCHAR namepipeJogador[50];
	HANDLE eventFinal;
	HANDLE waitTimer;
	HANDLE hThreadTimer;
	TCHAR characters[TAMCHAR]; //retirar
	TCHAR water[TAMCHAR];  //retirar
	HANDLE namePipe;
	OVERLAPPED ov;
	
};


typedef struct {
	dadosThreadJogador* dadosT;
	bufferCircular* buffer;
	HANDLE sem_vazios;
	HANDLE sem_itens;
	HANDLE mutexShr;
}dadosThreadComandos;

