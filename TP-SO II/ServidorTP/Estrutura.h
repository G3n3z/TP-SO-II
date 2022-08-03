#pragma once
#include <tchar.h>

#define NAMEMUTEX				TEXT("mutexSHR")
#define MUTEXBUFFCIRCULAR		TEXT("mutexbuffercircular")
#define NAMEMUTEXIN				TEXT("mutextInterno")
#define NAMESHR					TEXT("memoriaPartilhada")
#define NAMESHR2				TEXT("memoriaPartilhada2")

#define EVENTSERVIDOR2MONITOR   TEXT("eventServMonitor")
#define EVENTWRITE				TEXT("eventEscrita")
#define EVENTSERVIDOR2MONITOR2	TEXT("eventServMonitor2")
#define SEMAPHOREITENS			TEXT("semaphoreitens")
#define SEMAPHOREVAZIO			TEXT("semaphorevazio")
#define REGLINHAS				TEXT("Linhas")
#define REGCOLUNAS				TEXT("Colunas")
#define REGTEMPO				TEXT("Tempo")
#define TAMMSG					256

#define TAMMAX					20
#define TAMCHAR					6
#define MAXJOGADORES			2
#define TAMBUFFCIRC				10

#define TAMSHR sizeof(memPart)
#define SIZEBUFFER sizeof(bufferCircular)

extern TCHAR CHARACTERS[6] = { 0x2550, 0x2551, 0x2554, 0x2557, 0x255D, 0x255A};
extern TCHAR WATER[6] = { 0x2501, 0x2503, 0x250F, 0x2513, 0x251B, 0x2517};
extern TCHAR BLOCO = 0x2592;

typedef struct {
	TCHAR tab[TAMMAX][TAMMAX];
	TCHAR nextChar;
	int numjogador;
	int numeroJogadas;
	int pont;
}tabuleiros;

typedef struct dadosTabuleiro memPart;

struct dadosTabuleiro {
	int nLinhas;
	int nColunas;
	int time;
	int nivel;
	int estado;
	int numJogadores;
	tabuleiros jogador[MAXJOGADORES];
	TCHAR msg[TAMMSG];
};

typedef struct dadosThread dadosThreadJogador;

struct dadosThread{
	int *posXini, *posYini, *posXfinal, *posYfinal;
	int* perdeu;
	int* ganhou;
	int* modoJogo; // 0 - random  1 - seguidas
	int* flagSair;
	int pid;
	int numJogador;
	memPart* shr;
	HANDLE mutexSHR;
	HANDLE EventServ2Moni;
	HANDLE mutexSair;
	HANDLE mutexStruct;
	HANDLE hThread;
	TCHAR namepipeJogador[50];
	HANDLE eventFinal;
	HANDLE waitTimer;
	HANDLE hThreadTimer;
	TCHAR characters[TAMCHAR];
	TCHAR water[TAMCHAR];

};


struct MSGbufferCircular {
	TCHAR msg[TAMMSG];
	int x, y, player;
};

typedef struct dadosBufferCircular bufferCircular;

struct dadosBufferCircular {

	int in, out;
	struct MSGbufferCircular buff[TAMBUFFCIRC];
};

typedef struct {
	dadosThreadJogador *dadosT;
	bufferCircular* buffer;
	HANDLE sem_vazios;
	HANDLE sem_itens;
	HANDLE mutexShr;
}dadosThreadComandos;


