#pragma once
#include "../ServidorTP/servidor_cliente.h"


enum ESTADO { ATIVO, INATIVO };

typedef struct {
	//bitmap
	HBITMAP hBmp[15];
	HDC memDC; // copia do device context que esta em memoria, tem de ser inicializado a null
	HBITMAP hBitmapDB; // copia as caracteristicas da janela original para a janela que vai estar em memoria
	int tamOfBitMap;
	HDC hdcBitmaps[15];

}DADOSBM;


typedef struct {
	dadosServ2Cli dadosServ;
	HANDLE hPipe, mutex;
	DADOSBM* dadosBitMap;
	HDC bmpDC;
	HDC* memDC;
	HWND hwndJanela;
	HWND hwnTextField;
	HDC hdc;
	int* initX, * initY;
	enum ESTADO estado;
	HANDLE hTreadSinalVida, hWaitSinalVida;
	int* flagSair;
	TCHAR nomeJogador[256];
	OVERLAPPED* ov;
}dadosThreadCli;