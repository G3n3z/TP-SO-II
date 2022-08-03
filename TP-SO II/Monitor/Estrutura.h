#pragma once
#include <tchar.h>

#define TAMMAX					20
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
typedef struct dadosTabuleiro memPart;
#define TAMCHAR					6
#define MAXJOGADORES			2
#define TAMBUFFCIRC				10

typedef struct {
	TCHAR tab[TAMMAX][TAMMAX];
	TCHAR nextChar;
	int numjogador;
	int numeroJogadas;
	int pont;
}tabuleiros;

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
#define TAMSHR sizeof(memPart)

typedef struct dados dadosMonitor;

struct dados{
	memPart* shr;
	HANDLE mutexSHR;
	HANDLE EventServ2Moni;
	HANDLE eventMoni2Serv;
	int* flagSair;
	int jogador;
	HANDLE mutexSair;
};


typedef struct dadosBufferCircular bufferCircular;

struct MSGbufferCircular {
	TCHAR msg[TAMMSG];
	int x, y, player;
};

struct dadosBufferCircular {

	int in, out;
	struct MSGbufferCircular buff[TAMBUFFCIRC];
};

typedef struct {
	bufferCircular* buffer;
	HANDLE sem_vazios;
	HANDLE sem_itens;
	HANDLE mutexShr;
}dadosThreadComandos;

#define SIZEBUFFER sizeof(bufferCircular)