#pragma once

#define NAMEPIPE TEXT("\\\\.\\pipe\\namedpipe")
#define NAMEPIPEWITHOUTIP TEXT("\\pipe\\namedpipe")

#include "Servidor_Monitor.h"

static const TCHAR CHARACTER[6] = { 0x2550, 0x2551, 0x2554, 0x2557, 0x255D, 0x255A };
static const TCHAR WATERS[6] = { 0x2501, 0x2503, 0x250F, 0x2513, 0x251B, 0x2517 };
static const TCHAR BLOCOS = 0x2592;


enum validacao{ESPERAR, COMECAR, JOGADAINVALIDA, JOGADAVALIDA, TERMINAR, AGUA, NOVONIVEL, MOUSEOVER, MOUSEMOVE, RIGHT, SINALVIDA, DESISTIR, ENCERRAR, PERDEU, GANHOU, EMPATE, SUSPENDER, RETOMAR};

typedef struct {
	dadosJogo dados;
	int jogador;
	BOOL ganhou;
	enum validacao val;
	int xAgua, yAgua;
}dadosServ2Cli;

typedef struct {
	int x, y, aux;
	enum validacao val;
}dadosCli2Serv;


