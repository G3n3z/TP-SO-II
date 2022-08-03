#pragma once
#include <tchar.h>
#include <Windows.h>

#define TAMMAX					20
#define NAMEMUTEX				TEXT("mutexSHR")
//#define MUTEXBUFFCIRCULAR		TEXT("mutexbuffercircular")

#define NAMESHR					TEXT("memoriaPartilhada")
#define NAMESHR2				TEXT("memoriaPartilhada2")

#define EVENTSERVIDOR2MONITOR   TEXT("eventServMonitor")
#define EVENTSERVIDOR2MONITOR2	TEXT("eventServMonitor2")
#define SEMAPHOREITENS			TEXT("semaphoreitens")
#define SEMAPHOREVAZIO			TEXT("semaphorevazio")

#define MAXJOGADORES			2
#define TAMBUFFCIRC				10
#define TAMMSG					256


typedef struct dadosTabuleiro dadosJogo;

typedef struct {
	TCHAR tab[TAMMAX][TAMMAX];
	TCHAR nextChar;
	int numjogador;
	int numeroJogadas;
	int pont;
	int quantidadeDeParagem;
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
	int modoJogo;
};
#define TAMSHR sizeof(dadosJogo)


typedef struct dadosBufferCircular bufferCircular;

struct MSGbufferCircular {
	TCHAR msg[TAMMSG];
	int x, y, player;
};

struct dadosBufferCircular {

	int in, out;
	struct MSGbufferCircular buff[TAMBUFFCIRC];
};



#define SIZEBUFFER sizeof(bufferCircular)