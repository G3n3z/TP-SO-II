#pragma once
#include <tchar.h>
#include <windows.h>

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

typedef struct dadosTabuleiro memPart;
typedef struct dadosBufferCircular bufferCircular;

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

struct MSGbufferCircular {
	TCHAR msg[TAMMSG];
	int x, y, player;
};

struct dadosBufferCircular {

	int in, out;
	struct MSGbufferCircular buff[TAMBUFFCIRC];
};

typedef struct {
	//dadosThreadJogador* dadosT;
	bufferCircular* buffer;
	HANDLE sem_vazios;
	HANDLE sem_itens;
	HANDLE mutexShr;
}dadosThreadComandos;
