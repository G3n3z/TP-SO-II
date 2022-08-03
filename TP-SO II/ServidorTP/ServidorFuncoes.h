#pragma once
#include "servidor_cliente.h"
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include "Servidor.h"
#include "..\dllFuncoes\funcoes.h"
BOOL sair(dadosThreadJogador* dadosT);
BOOL ganhou(dadosThreadJogador* dadosT);
BOOL perdeu(dadosThreadJogador* dadosT);
