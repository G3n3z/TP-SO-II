#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"
#include "stdlib.h"
#include "Cliente.h"
#define NAMEGAMEBOARD TEXT("gameboard")

/* ===================================================== */
/* Programa base (esqueleto) para aplica??es Windows     */
/* ===================================================== */
// Cria uma janela de nome "Janela Principal" e pinta fundo de branco
// Modelo para programas Windows:
//  Composto por 2 fun??es: 
//	WinMain()     = Ponto de entrada dos programas windows
//			1) Define, cria e mostra a janela
//			2) Loop de recep??o de mensagens provenientes do Windows
//     TrataEventos()= Processamentos da janela (pode ter outro nome)
//			1) ? chamada pelo Windows (callback) 
//			2) Executa c?digo em fun??o da mensagem recebida

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK TrataEventoGameBoard(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
HWND CreateDialogBox(HWND hwnd, HINSTANCE hInst);
void RegisterDialogClass(HWND hwnd, HINSTANCE hInst);
// Nome da classe da janela (para programas de uma s? janela, normalmente este nome ? 
// igual ao do pr?prio programa) "szprogName" ? usado mais abaixo na defini??o das 
// propriedades do objecto janela
TCHAR szProgName[] = TEXT("Base");
//TCHAR szWindowBoard[] = TEXT("")
// ============================================================================
// FUN??O DE IN?CIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa come?a sempre a sua execu??o na fun??o WinMain()que desempenha
// o papel da fun??o main() do C em modo consola WINAPI indica o "tipo da fun??o" (WINAPI
// para todas as declaradas nos headers do Windows e CALLBACK para as fun??es de
// processamento da janela)
// Par?metros:
//   hInst: Gerado pelo Windows, ? o handle (n?mero) da inst?ncia deste programa 
//   hPrevInst: Gerado pelo Windows, ? sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Gerado pelo Windows, ? um ponteiro para uma string terminada por 0
//              destinada a conter par?metros para o programa 
//   nCmdShow:  Par?metro que especifica o modo de exibi??o da janela (usado em  
//        	   ShowWindow()
HINSTANCE inst;
int cmdShow;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;		
	MSG lpMsg;	
	dadosThreadCli dadosT;
	WNDCLASSEX wcApp;	// WNDCLASSEX ? uma estrutura cujos membros servem para 
			  // definir as caracter?sticas da classe da janela


	wcApp.cbSize = sizeof(WNDCLASSEX);      
	wcApp.hInstance = hInst;		      
	wcApp.lpszClassName = szProgName; 
	wcApp.lpfnWndProc = TrataEventos;     

	wcApp.style = CS_HREDRAW | CS_VREDRAW;  // Estilo da janela: Fazer o redraw se for
											// modificada horizontal ou verticalmente

	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);   // "hIcon" = handler do ?con normal

	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION); // "hIconSm" = handler do ?con pequeno

	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);	// "hCursor" = handler do cursor (rato) 

	wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcApp.cbClsExtra = 0;				// Livre, para uso particular
	wcApp.cbWndExtra = sizeof(dadosThreadCli*);				// Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));//(HBRUSH)GetStockObject(WHITE_BRUSH);
	/*
	WNDCLASSEX wcBoard;

	wcBoard.cbSize = sizeof(WNDCLASSEX);
	wcBoard.hInstance = hInst;
	wcBoard.lpszClassName = _T("janela");
	wcBoard.lpfnWndProc = TrataEventoGameBoard;
	wcBoard.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcBoard.hIcon = NULL;   // "hIcon" = handler do ?con normal

	wcBoard.hIconSm = NULL;
	wcBoard.hCursor = LoadCursor(NULL, IDC_ARROW);

	wcBoard.lpszMenuName = NULL;
	wcBoard.cbClsExtra = 0;				// Livre, para uso particular
	wcBoard.cbWndExtra = sizeof(dadosThreadCli*);				// Livre, para uso particular
	wcBoard.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(100, 200, 200));//
	
	if (!RegisterClassEx(&wcBoard)) {
		return 0;
	}
	*/



	inst = hInst;
	cmdShow = nCmdShow;
	if (!RegisterClassEx(&wcApp))
		return(0);

	

	hWnd = CreateWindow(
		szProgName,			// Nome da janela (programa) definido acima
		TEXT("Jogo dos Tubos"),// Texto que figura na barra do t?tulo
		WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT,		// Posi??o x pixels (default=? direita da ?ltima)
		CW_USEDEFAULT,		// Posi??o y pixels (default=abaixo da ?ltima)
		1500,		// Largura da janela (em pixels)
		830,		// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,	// handle da janela pai (se se criar uma a partir de
						// outra) ou HWND_DESKTOP se a janela for a primeira, 
						// criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da inst?ncia do programa actual ("hInst" ? 
						// passado num dos par?metros de WinMain()
		0);				// N?o h? par?metros adicionais para a janela


	HDC hdc;
	//SystemParametersInfo(SPI_SETMOUSEHOVERWIDTH, 32, NULL, 0);
	//SystemParametersInfo(SPI_SETMOUSEHOVERHEIGHT, 32, NULL, 0);
	DADOSBM dadosBM;

	dadosBM.hBmp[0] = (HBITMAP)LoadImage(NULL, TEXT("empty0.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[1] = (HBITMAP)LoadImage(NULL, TEXT("empty1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[2] = (HBITMAP)LoadImage(NULL, TEXT("empty2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[3] = (HBITMAP)LoadImage(NULL, TEXT("empty3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[4] = (HBITMAP)LoadImage(NULL, TEXT("empty4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[5] = (HBITMAP)LoadImage(NULL, TEXT("empty5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[6] = (HBITMAP)LoadImage(NULL, TEXT("water0.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[7] = (HBITMAP)LoadImage(NULL, TEXT("water1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[8] = (HBITMAP)LoadImage(NULL, TEXT("water2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[9] = (HBITMAP)LoadImage(NULL, TEXT("water3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[10] = (HBITMAP)LoadImage(NULL, TEXT("water4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[11] = (HBITMAP)LoadImage(NULL, TEXT("water5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[12] = (HBITMAP)LoadImage(NULL, TEXT("vazio.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[13] = (HBITMAP)LoadImage(NULL, TEXT("brick.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	dadosBM.hBmp[14] = (HBITMAP)LoadImage(NULL, TEXT("over.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	
	hdc = GetDC(hWnd);
	for (int i = 0; i < 15; i++) {
		dadosBM.hdcBitmaps[i] = CreateCompatibleDC(hdc);
		SelectObject(dadosBM.hdcBitmaps[i], dadosBM.hBmp[i]);
	}
	ReleaseDC(hWnd, hdc);
	dadosBM.tamOfBitMap = 32;
	hdc = GetDC(hWnd);



	ReleaseDC(hWnd, hdc);
	dadosT.mutex = CreateMutex(NULL, FALSE, NULL);
	dadosT.dadosBitMap = &dadosBM;
	dadosT.estado = INATIVO;
	
	SetWindowLongPtr(hWnd, 0, (LONG_PTR)&dadosT);

	  // ============================================================================
	  // 4. Mostrar a janela
	  // ============================================================================
	
	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela, devolvido por 
					  // "CreateWindow"; "nCmdShow"= modo de exibi??o (p.e. 
					  // normal/modal); ? passado como par?metro de WinMain()
	UpdateWindow(hWnd);		// Refrescar a janela (Windows envia ? janela uma 
					  // mensagem para pintar, mostrar dados, (refrescar)? 
					  // 
					  // 
	//HANDLE h = CreateWindow(TEXT("EDIT"), 0, WS_BORDER | WS_CHILD | WS_VISIBLE, 56, 10, 50, 18, hWnd, 0, hInst, 0);
	//ShowWindow(h, nCmdShow);
	//UpdateWindow(h);
	/*/HANDLE hWnd2;
	hWnd2 = CreateWindow(
		NAMEGAMEBOARD,			// Nome da janela (programa) definido acima
		_T(""),// Texto que figura na barra do t?tulo
		WS_OVERLAPPEDWINDOW,	// Estilo da janela (WS_OVERLAPPED= normal)
		100,		// Posi??o x pixels (default=? direita da ?ltima)
		100,		// Posi??o y pixels (default=abaixo da ?ltima)
		100,		// Largura da janela (em pixels)
		200,		// Altura da janela (em pixels)
		(HWND)hWnd,	// handle da janela pai (se se criar uma a partir de
						// outra) ou HWND_DESKTOP se a janela for a primeira, 
						// criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da inst?ncia do programa actual ("hInst" ? 
						// passado num dos par?metros de WinMain()
		0);

	if (hWnd2 == NULL) {
		return 0;
	}

	ShowWindow(hWnd2, nCmdShow);	
	UpdateWindow(hWnd2);
	*/

	// ============================================================================
	// 5. Loop de Mensagens
	// ============================================================================
	// O Windows envia mensagens ?s janelas (programas). Estas mensagens ficam numa fila de
	// espera at? que GetMessage(...) possa ler "a mensagem seguinte"	
	// Par?metros de "getMessage":
	// 1)"&lpMsg"=Endere?o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no  
	//   in?cio de WinMain()):
	//			HWND hwnd		handler da janela a que se destina a mensagem
	//			UINT message		Identificador da mensagem
	//			WPARAM wParam		Par?metro, p.e. c?digo da tecla premida
	//			LPARAM lParam		Par?metro, p.e. se ALT tamb?m estava premida
	//			DWORD time		Hora a que a mensagem foi enviada pelo Windows
	//			POINT pt		Localiza??o do mouse (x, y) 
	// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem
	//   receber as mensagens para todas as
	// janelas pertencentes ? thread actual)
	// 3)C?digo limite inferior das mensagens que se pretendem receber
	// 4)C?digo limite superior das mensagens que se pretendem receber

	// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
	// 	  terminando ent?o o loop de recep??o de mensagens, e o programa 

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);	// Pr?-processamento da mensagem (p.e. obter c?digo 
					   // ASCII da tecla premida)
		DispatchMessage(&lpMsg);	// Enviar a mensagem traduzida de volta ao Windows, que
					   // aguarda at? que a possa reenviar ? fun??o de 
					   // tratamento da janela, CALLBACK TrataEventos (abaixo)
	}

	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam);	// Retorna sempre o par?metro wParam da estrutura lpMsg
}

// ============================================================================
// FUN??O DE PROCESSAMENTO DA JANELA
// Esta fun??o pode ter um nome qualquer: Apenas ? neces?rio que na inicializa??o da
// estrutura "wcApp", feita no in?cio de // WinMain(), se identifique essa fun??o. Neste
// caso "wcApp.lpfnWndProc = WndProc"
//
// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pr?-processadas
// no loop "while" da fun??o WinMain()
// Par?metros:
//		hWnd	O handler da janela, obtido no CreateWindow()
//		messg	Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
//		wParam	O par?metro wParam da estrutura messg (a mensagem)
//		lParam	O par?metro lParam desta mesma estrutura
//
// NOTA:Estes par?metros est?o aqui acess?veis o que simplifica o acesso aos seus valores
//
// A fun??o EndProc ? sempre do tipo "switch..." com "cases" que descriminam a mensagem
// recebida e a tratar.
// Estas mensagens s?o identificadas por constantes (p.e. 
// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h
// ============================================================================

void copia(TCHAR* dest, TCHAR* source, int tam) {
	for (int i = 0; i < tam; i++)
	{
		dest[i] = source[i];
	}
	dest[tam] = TEXT('\0');
}
void concatena(TCHAR* dest, TCHAR* source, int tam, int inicio, int tamDest) {
	int i, j;
	for (i = inicio, j = 0; j < tam && i < tamDest; i++, j++)
	{
		dest[i] = source[j];
	}
	dest[i] = TEXT('\0');
}
TCHAR *modificaNamePipe(TCHAR* ipRecolhido) {

	TCHAR novoIP[256];
	if (_tcslen(ipRecolhido) < 7) {
		return NAMEPIPE;
	}
	copia(novoIP, (TEXT("\\\\")), (int)_tcslen(TEXT("\\\\")));
	concatena(novoIP, ipRecolhido, (int)_tcslen(ipRecolhido), (int)_tcslen(novoIP), sizeof(novoIP)/sizeof(TCHAR));
	concatena(novoIP, TEXT("\\pipe\\namedpipe"), (int)_tcslen(TEXT("\\pipe\\namedpipe")), (int)_tcslen(novoIP), sizeof(novoIP) / sizeof(TCHAR));
	copia(ipRecolhido, novoIP, (int)_tcslen(novoIP));
	return ipRecolhido;
}

HANDLE Connect2NamePipe(TCHAR* namedPipe, HANDLE hPipe) {
	hPipe = CreateFile(NAMEPIPE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL| FILE_FLAG_OVERLAPPED, NULL);
	if (hPipe == INVALID_HANDLE_VALUE) {
		return NULL;
	} 
	DWORD modoAcesso = PIPE_READMODE_MESSAGE;
	if (!SetNamedPipeHandleState(hPipe, &modoAcesso, NULL, NULL)) {
		_tprintf(TEXT("Nao foi possivel mudar o modo de acesso"));
		CloseHandle(hPipe);
		return NULL;
	}
	return hPipe;
}

BOOL changeBitmap(HWND hWnd, int modo, dadosThreadCli* dados) {

	HDC hdc;
	if (modo == 0) {
		for (int i = 0; i < 15; i++) {
			DeleteObject(dados->dadosBitMap->hBmp[i]);
		}

		dados->dadosBitMap->hBmp[0] = (HBITMAP)LoadImage(NULL, TEXT("empty0v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[1] = (HBITMAP)LoadImage(NULL, TEXT("empty1v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[2] = (HBITMAP)LoadImage(NULL, TEXT("empty2v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[3] = (HBITMAP)LoadImage(NULL, TEXT("empty3v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[4] = (HBITMAP)LoadImage(NULL, TEXT("empty4v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[5] = (HBITMAP)LoadImage(NULL, TEXT("empty5v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[6] = (HBITMAP)LoadImage(NULL, TEXT("water0v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[7] = (HBITMAP)LoadImage(NULL, TEXT("water1v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[8] = (HBITMAP)LoadImage(NULL, TEXT("water2v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[9] = (HBITMAP)LoadImage(NULL, TEXT("water3v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[10] = (HBITMAP)LoadImage(NULL, TEXT("water4v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[11] = (HBITMAP)LoadImage(NULL, TEXT("water5v2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[12] = (HBITMAP)LoadImage(NULL, TEXT("vaziov2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[13] = (HBITMAP)LoadImage(NULL, TEXT("brickv2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[14] = (HBITMAP)LoadImage(NULL, TEXT("overv2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		hdc = GetDC(hWnd);
		for (int i = 0; i < 15; i++) {
			dados->dadosBitMap->hdcBitmaps[i] = CreateCompatibleDC(hdc);
			SelectObject(dados->dadosBitMap->hdcBitmaps[i], dados->dadosBitMap->hBmp[i]);
		}
		ReleaseDC(hWnd, hdc);

	}
	if (modo == 1) {
		for (int i = 0; i < 15; i++) {
			DeleteObject(dados->dadosBitMap->hBmp[i]);
		}

		dados->dadosBitMap->hBmp[0] = (HBITMAP)LoadImage(NULL, TEXT("empty0.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[1] = (HBITMAP)LoadImage(NULL, TEXT("empty1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[2] = (HBITMAP)LoadImage(NULL, TEXT("empty2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[3] = (HBITMAP)LoadImage(NULL, TEXT("empty3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[4] = (HBITMAP)LoadImage(NULL, TEXT("empty4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[5] = (HBITMAP)LoadImage(NULL, TEXT("empty5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[6] = (HBITMAP)LoadImage(NULL, TEXT("water0.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[7] = (HBITMAP)LoadImage(NULL, TEXT("water1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[8] = (HBITMAP)LoadImage(NULL, TEXT("water2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[9] = (HBITMAP)LoadImage(NULL, TEXT("water3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[10] = (HBITMAP)LoadImage(NULL, TEXT("water4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[11] = (HBITMAP)LoadImage(NULL, TEXT("water5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[12] = (HBITMAP)LoadImage(NULL, TEXT("vazio.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[13] = (HBITMAP)LoadImage(NULL, TEXT("brick.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		dados->dadosBitMap->hBmp[14] = (HBITMAP)LoadImage(NULL, TEXT("over.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		hdc = GetDC(hWnd);
		for (int i = 0; i < 15; i++) {
			dados->dadosBitMap->hdcBitmaps[i] = CreateCompatibleDC(hdc);
			SelectObject(dados->dadosBitMap->hdcBitmaps[i], dados->dadosBitMap->hBmp[i]);
		}
		ReleaseDC(hWnd, hdc);

	}

	return TRUE;
}

INT_PTR CALLBACK DlgNameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	dadosThreadCli* dadosT = (dadosThreadCli*)GetWindowLongPtr(GetParent(hwnd), 0);
	TCHAR* str;
	UINT nChar = 0;
	HDC hdc;
	switch (msg) {
	case WM_INITDIALOG:
		SetDlgItemText(hwnd, IDC_EDIT1, TEXT("Jogador"));
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			hdc = GetDC(hwnd);
			str = malloc(256 * sizeof(TCHAR));
			if (str != NULL) {
				nChar = GetDlgItemText(hwnd, IDC_EDIT1, str, 256);
				str[nChar] = TEXT('\0');
				copia(dadosT->nomeJogador, str, (int)_tcslen(str));
			}
			free(str);
			ReleaseDC(hwnd, hdc);
			EndDialog(hwnd, IDOK); // janela criada com DialogBox()
			//DestroyWindow(hwnd); // janela criado com CreateDialog()
			return IDOK;
		case IDCANCEL:
			EndDialog(hwnd, IDCANCEL); // janela criada com DialogBox()
			DestroyWindow(hwnd); // janela criado com CreateDialog()
			return TRUE;
		default:
			return TRUE;
		}

	case WM_CLOSE:
		EndDialog(hwnd, IDCANCEL); // janela criada com DialogBox()
		DestroyWindow(hwnd); // janela criado com CreateDialog()
		return TRUE;
	}
	return FALSE;
}


INT_PTR CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam,LPARAM lParam) {
	TCHAR* str;
	UINT nChar = 0;
	HDC hdc;
	switch (msg) {
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			hdc = GetDC(hwnd);
			str = malloc(256 * sizeof(TCHAR));
			if (str != NULL){
				nChar = GetDlgItemText(hwnd, IDC_EDIT1, str, 256);
				str[nChar] = TEXT('\0');
			}
			ReleaseDC(hwnd, hdc);
			EndDialog(hwnd, (INT_PTR)str); // janela criada com DialogBox()
			//DestroyWindow(hwnd); // janela criado com CreateDialog()
			return (INT_PTR)str;
		case IDCANCEL:
			EndDialog(hwnd, IDOK); // janela criada com DialogBox()
			DestroyWindow(hwnd); // janela criado com CreateDialog()
			return TRUE;
		default:
			return TRUE;
		}
	
	case WM_CLOSE:
		EndDialog(hwnd, IDOK); // janela criada com DialogBox()
		DestroyWindow(hwnd); // janela criado com CreateDialog()
		return TRUE;
	}
	return FALSE;
}


OVERLAPPED preparaOverlap(OVERLAPPED ov) {
	ZeroMemory(&ov, sizeof(ov));
	ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	return ov;
}

void imprime(HWND hWnd, TCHAR* str, int posx, int posY) {
	HDC hdc = GetDC(hWnd);

	TextOut(hdc, posx, posY, str, (int)_tcslen(str));
	
	ReleaseDC(hWnd, hdc);
}

void imprimeTCHAR(HWND hWnd, TCHAR c, int posx, int posY) {
	HDC hdc = GetDC(hWnd);

	TextOut(hdc, posx, posY, &c, 1);

	ReleaseDC(hWnd, hdc);
}
int getHBITMAP(TCHAR c, int i, int j) {
	
	if (c == TEXT(' '))
		return 12;

	for (int i = 0; i < 6; i++) {
		if (c == CHARACTER[i]) {
			return i;
		}
	}
	for (int i = 0; i < 6; i++)
	{
		if (c == WATERS[i]) {
			return i+6;
		}
	}
	if (c == BLOCOS) {
		return  13;
	}
	else if (c == TEXT('X')) {
		return 14;
	}
	
	return 12;
	
}
BOOL setTexto(TCHAR* texto, dadosServ2Cli dados) {
	
	TCHAR jogador[5], nivel[5], pont[5], ajudas[5], modo[30];

	if (dados.dados.modoJogo == 0) {
		copia(modo, TEXT("Aleatorio"), (int)_tcslen(TEXT("Aleatorio")));
	}
	else {
		copia(modo, TEXT("Normal"), (int)_tcslen(TEXT("Normal")));
	}

	_itot_s(1+dados.jogador, jogador, sizeof(jogador) / sizeof(TCHAR), 10);
	_itot_s(1+dados.dados.nivel, nivel, sizeof(nivel) / sizeof(TCHAR), 10);
	_itot_s(dados.dados.jogador[dados.jogador].pont, pont, sizeof(pont) / sizeof(TCHAR), 10);
	_itot_s((3-dados.dados.jogador[dados.jogador].quantidadeDeParagem), ajudas, sizeof(ajudas) / sizeof(TCHAR), 10);

	copia(texto, TEXT("Jogador: "), (int)_tcslen(TEXT("Jogador: ")));
	concatena(texto, jogador, (int)_tcslen(jogador), (int)_tcslen(texto), 256);
	concatena(texto, TEXT("\t\tNivel: "), (int)_tcslen(TEXT("\t\tNivel: ")), (int)_tcslen(texto), 256);
	concatena(texto, nivel, (int)_tcslen(nivel), (int)_tcslen(texto), 256);
	concatena(texto, TEXT("\n"), (int)_tcslen(TEXT("\n")), (int)_tcslen(texto), 256);

	concatena(texto, TEXT("Modo: "), (int)_tcslen(TEXT("Modo: ")), (int)_tcslen(texto), 256);
	concatena(texto, modo, (int)_tcslen(modo), (int)_tcslen(texto), 256);
	concatena(texto, TEXT("\t\tPontuação: "), (int)_tcslen(TEXT("\t\tPontuação: ")), (int)_tcslen(texto), 256);
	concatena(texto, pont, (int)_tcslen(pont), (int)_tcslen(texto), 256);
	concatena(texto, TEXT("\n"), (int)_tcslen(TEXT("\n")), (int)_tcslen(texto), 256);

	concatena(texto, TEXT("Proximo Tubo:"), (int)_tcslen(TEXT("Proximo Tubo:")), (int)_tcslen(texto), 256);
	concatena(texto, TEXT("\t\tAjudas: "), (int)_tcslen(TEXT("\t\tAjudas: ")), (int)_tcslen(texto), 256);
	concatena(texto, ajudas, (int)_tcslen(ajudas), (int)_tcslen(texto), 256);

	return TRUE;
}
void CloseResources(dadosThreadCli* dadosT) {
	CloseHandle(dadosT->mutex);
	for (int i = 0; i < 15; i++) {
		DeleteObject(dadosT->dadosBitMap->hBmp[i]);
	}
	if(dadosT->hwnTextField != NULL){
		DestroyWindow(dadosT->hwnTextField);
		CloseHandle(dadosT->hwnTextField);
	}
}
DWORD WINAPI threadSinalVida(LPVOID d) {
	dadosThreadCli* dadosT = (dadosThreadCli*)d;
	HANDLE hWaitTimer;
	LARGE_INTEGER li;
	DWORD result, nEscritos;
	OVERLAPPED ov;
	dadosCli2Serv dadosEnviar;
	li.QuadPart = -10 * 10000000;
	
	hWaitTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	if (hWaitTimer == NULL) {
		return -1;
	}

	dadosT->hWaitSinalVida = hWaitTimer;
	SetWaitableTimer(hWaitTimer, &li, 10 * 1000, NULL, NULL, FALSE);
	while (1) {
		//imprime(dadosT->hwndJanela, TEXT("Depois do Wait"), 20, 750);
		result = WaitForSingleObject(hWaitTimer, INFINITE);
		
		//imprime(dadosT->hwndJanela, TEXT("Depois do Wait"), 20, 800);
		if (result != 0) {
			break;
		}
		
		WaitForSingleObject(dadosT->mutex, INFINITE);
		if (*dadosT->flagSair == 1) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		ReleaseMutex(dadosT->mutex);
		ZeroMemory(&ov, sizeof(ov));
		ov.hEvent = CreateEvent(NULL,TRUE, FALSE, NULL);
		dadosEnviar.val = SINALVIDA;
		if (!WriteFile(dadosT->hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
			if (GetLastError() == ERROR_IO_PENDING) {
				GetOverlappedResult(dadosT->hPipe, &ov, &nEscritos, FALSE);
			}
			else {
				return 0;
			}
		}
		if (nEscritos != sizeof(dadosEnviar)) {
			return 0;
		}
	}

	CloseHandle(hWaitTimer);
	//CloseHandle(dadosT->hWaitSinalVida);
	//SetWindowText(dadosT->hwnTextField, TEXT("SAItimer"));
	return 0;
}


DWORD WINAPI threadLeitura(LPVOID dados) {

	dadosThreadCli* dadosT = (dadosThreadCli*)dados;
	OVERLAPPED ov;
	dadosServ2Cli dadosServ;
	DWORD nLidos;
	 HANDLE hAppMenu, hSubMenu;
	RECT rect;
	int x, y;
	HBITMAP bit = dadosT->dadosBitMap->hBmp[0];
	int index = 0;
	TCHAR texto[256];
	
	ZeroMemory(&ov, sizeof(ov));

	ov = preparaOverlap(ov);
	WaitForSingleObject(dadosT->mutex, INFINITE);
	dadosT->ov = &ov;
	ReleaseMutex(dadosT->mutex);
	if (!ReadFile(dadosT->hPipe, &dadosServ, sizeof(dadosServ), &nLidos, &ov)) {
		if (GetLastError() == ERROR_IO_PENDING) {
			WaitForSingleObject(ov.hEvent, INFINITE);
			GetOverlappedResult(dadosT->hPipe, &ov, &nLidos, FALSE);
			
		}
	}

	if (dadosServ.val == TERMINAR) {
		ExitThread(0);
	}

	HWND hwnd = dadosT->hwndJanela;
	dadosT->estado = ATIVO;

	//SetWindowText(dadosT->hwnTextField, TEXT("Jogador:   \t\tNível:   \r\t\t\tPontuação     \rProximo Tubo:\t\tAjudas: "));
	
	while (1) {

		ov = preparaOverlap(ov);
		WaitForSingleObject(dadosT->mutex, INFINITE);
		if (*dadosT->flagSair == 1) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		dadosT->ov = &ov;
		ReleaseMutex(dadosT->mutex);
		
		if (!ReadFile(dadosT->hPipe, &dadosServ, sizeof(dadosServ), &nLidos, &ov)) {
			if (GetLastError() == ERROR_IO_PENDING) {
				WaitForSingleObject(ov.hEvent, INFINITE);
				GetOverlappedResult(dadosT->hPipe, &ov, &nLidos, FALSE);
			}
		}
		if (nLidos != sizeof(dadosServ)) {
			dadosServ.val = JOGADAVALIDA;
			WaitForSingleObject(dadosT->mutex, INFINITE);
			*dadosT->flagSair = 1;
			ReleaseMutex(dadosT->mutex);
			break;
		}
		if (dadosServ.val == PERDEU) {
			SetWindowText(dadosT->hwnTextField, TEXT("Perdeu"));

			break;
		}
		else if (dadosServ.val == JOGADAVALIDA || dadosServ.val == AGUA){
			x = *dadosT->initX;
			y = *dadosT->initY;
			//x = y = 100;
			//if (*dadosT->memDC != NULL) {
				WaitForSingleObject(dadosT->mutex, INFINITE);
				//hdc = GetDC(dadosT->hwndJanela); //Retirar
				FillRect(*dadosT->memDC, &rect, CreateSolidBrush(RGB(255, 255, 255)));
				
				
				setTexto(texto, dadosServ);
				index = getHBITMAP(dadosServ.dados.jogador[dadosServ.jogador].nextChar, 0,0);
				//imprime(dadosT->hwndJanela, TEXT("Proximo Tubo"), 450, 116);
				BitBlt(*dadosT->memDC, 80, 102, 32, 32, dadosT->dadosBitMap->hdcBitmaps[index], 0, 0, SRCCOPY);

				CopyMemory(&dadosT->dadosServ, &dadosServ, sizeof(dadosServ));
				GetClientRect(dadosT->hwndJanela, &rect);
				
				for (int i = 0; i < dadosServ.dados.nLinhas; i++)
				{	
					x = *dadosT->initX;
					//x = 100;
					for (int j = 0; j < dadosServ.dados.nColunas; j++)
					{
						
						index = getHBITMAP(dadosT->dadosServ.dados.jogador[dadosT->dadosServ.jogador].tab[i][j], i, j);
						
						if (!BitBlt(*dadosT->memDC, x, y, 32, 32, dadosT->dadosBitMap->hdcBitmaps[index], 0, 0, SRCCOPY)) {
							_tprintf(TEXT("Erro"));
						}
						
						x += 32;

					}
					y += 32;
				}
				//DeleteObject(bit);
				//ReleaseDC(dadosT->hwndJanela, hdc);//retirar
				ReleaseMutex(dadosT->mutex);
			
				SetWindowText(dadosT->hwnTextField, texto);
				//imprime(dadosT->hwndJanela, TEXT("Vou imprimir"), 0, 0);
				InvalidateRect(dadosT->hwndJanela, NULL, TRUE);

		}
		else if (dadosServ.val == GANHOU) {
			SetWindowText(dadosT->hwnTextField, TEXT("Parabens Ganhou"));
			
			break;
		}

		else if (dadosServ.val == EMPATE) {
			SetWindowText(dadosT->hwnTextField, TEXT("Empatamos"));
			break;
		}
		else if (dadosServ.val == NOVONIVEL) {
			SetWindowText(dadosT->hwnTextField, TEXT("Passou de Nivel"));
		}
		else if (dadosServ.val == ENCERRAR) {
			break;
		}
		else if (dadosServ.val == SUSPENDER) {
			dadosT->estado = INATIVO;
			SetWindowText(dadosT->hwnTextField, TEXT("O Servidor mandou pausar o Jogo"));
		}
		else if (dadosServ.val == RETOMAR) {
			dadosT->estado = ATIVO;
			SetWindowText(dadosT->hwnTextField, TEXT("O Servidor mandou Retomar o Jogo"));
		}
		else if (dadosServ.val == DESISTIR) {
			SetWindowText(dadosT->hwnTextField, TEXT("Parabens Ganhou\nO adversário Desistiu"));
			break;
		}

	}

	//CloseHandle(dadosT->mutex);
	
	//dadosT->mutex = NULL;
	dadosT->estado = INATIVO;
	dadosT->hPipe = NULL;
	dadosT->dadosServ.dados.estado = 0;
	//SetEvent(dadosT->hWaitSinalVida);
	
	WaitForSingleObject(dadosT->mutex, INFINITE);
	if (*dadosT->flagSair) {
		ReleaseMutex(dadosT->mutex);
		return 0;
	}
	*dadosT->flagSair = 1;
	ReleaseMutex(dadosT->mutex);
	LARGE_INTEGER li;
	li.QuadPart = -10000000;
	SetWaitableTimer(dadosT->hWaitSinalVida, &li, 1000, NULL, NULL, FALSE);
//	FlushFileBuffers(dadosT->hPipe);
	DisconnectNamedPipe(dadosT->hPipe);
	CloseHandle(dadosT->hPipe);
	WaitForSingleObject(dadosT->hTreadSinalVida, INFINITE);
	//SetWindowText(dadosT->hwnTextField, TEXT("SAI"));
	CloseHandle(dadosT->hTreadSinalVida);

	if (dadosServ.val == GANHOU) {
		MessageBox(hwnd, TEXT("Parabens Ganhou"), TEXT("Fim de Jogo"), MB_OK | MB_ICONINFORMATION);
	}
	else if (dadosServ.val == PERDEU) {
		MessageBox(hwnd, TEXT("Perdeu o Jogo"), TEXT("Fim de Jogo"), MB_OK | MB_ICONEXCLAMATION);
	}
	if (dadosServ.val == ENCERRAR) {
		SendMessage(dadosT->hwndJanela, WM_CLOSE, 1, 0);
	}
	else {
		hAppMenu = GetMenu(dadosT->hwndJanela);
		hSubMenu = GetSubMenu(hAppMenu, 0);
		EnableMenuItem(hSubMenu, ID_FILE_NEWGAME, MF_BYCOMMAND | MF_ENABLED);
	}
	return 0;
}


LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	int result = 0;
	static TCHAR c;
	HDC hdc;
	RECT rect;
	PAINTSTRUCT pcs;
	int x;
	TCHAR* namedPipe = NULL;
	static TCHAR* ipRecolhido;
	BOOL exists = FALSE;
	static BOOL isTracking, waterStop;
	static HANDLE hPipe, hThread;

	static dadosServ2Cli dados;
	DWORD nLidos, nEscritos, idThread, idThreadSinalVida;
	OVERLAPPED ov;
	static dadosCli2Serv dadosEnviar;
	//static dadosThreadCli dadosT;
	static HANDLE hWnd2, hAppMenu, hSubMenu;
	static HANDLE hText, hWaitTimer;
	dadosThreadCli* dadosT = (dadosThreadCli*)GetWindowLongPtr(hWnd, 0);
	static HBITMAP hBitmapDB;
	static HDC memDC = NULL;
	static HDC bmpDC;
	static int width, height, initX, initY, posX, posY, posxAgua, posYAgua, lastCelX, lastCelY;
	static TRACKMOUSEEVENT tme;
	static int flag;
	if (dadosT != NULL) {
		dadosT->flagSair = &flag;
	}
	ZeroMemory(&ov, sizeof(ov));
	switch (messg) {
	case WM_CREATE:
		hText = CreateWindow(TEXT("Static"), NULL, WS_CHILD | WS_VISIBLE, 48, 48, 300, 54, hWnd, 0, (HINSTANCE)GetModuleHandle(NULL), 0);
		if (!hText) {
			MessageBox(NULL, TEXT("Failed To CREATE STATIC TEXT."), TEXT("Error"), MB_OK | MB_ICONERROR);
		}
		if (!SetWindowText(hText, TEXT("Bem Vindo! \rPor favor conecte-se ao Servidor"))) {
			MessageBox(NULL, TEXT("Failed To Set Text."), TEXT("Error"), MB_OK | MB_ICONERROR);
		}
		GetClientRect(hWnd, &rect);
		width = rect.right;
		height = rect.bottom;
		
		isTracking = FALSE;
		
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_FILE_NEWGAME:
			; INT_PTR res = DialogBoxParam(inst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, DlgNameProc,0);
			if (res != IDOK) {
				break;
			}
			do {
				exists = FALSE;
				ipRecolhido = (TCHAR*)DialogBoxParam(inst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc, (LPARAM)&x);
				if (ipRecolhido != NULL) {
					namedPipe = modificaNamePipe(ipRecolhido);
					if (WaitNamedPipe(namedPipe, 1000)) {
						ipRecolhido = NULL;
						exists = TRUE;

					}
					else {
						MessageBox(hWnd, TEXT("Servidor inserido não disponivel"), TEXT(""), MB_OK | MB_ICONINFORMATION);
					}
				}
			} while (ipRecolhido != NULL);
			if (exists) {
				if ((hPipe = Connect2NamePipe(namedPipe, hPipe)) != NULL) {
					ZeroMemory(&ov, sizeof(ov));
					ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
					if (!ReadFile(hPipe, &dados, sizeof(dados), &nLidos, &ov)) {
						if (GetLastError() == ERROR_IO_PENDING) {
							imprime(hWnd, TEXT("A espera\n"), 30, 30);
							WaitForSingleObject(ov.hEvent, INFINITE);
							GetOverlappedResult(hPipe, &ov, &nLidos, FALSE);

						}
						else {
							imprime(hWnd, TEXT("Erro ao agendar"), 10, 10);

						}
					}


					if (nLidos != sizeof(dados)) {
						imprime(hWnd, TEXT("Erro de tamanho de leitura"), 50, 50);

					}
					if (dados.val == ESPERAR) {
						//imprime(hWnd, TEXT("Sou o jogador 1 e vou dizer quandos jogam\n"), 10, 10);
						do {
							exists = FALSE;
							ipRecolhido = (TCHAR*)DialogBoxParam(inst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, DlgProc, (LPARAM)&x);
							if (ipRecolhido != NULL) {
								dadosEnviar.aux = _tstoi(ipRecolhido);
								if (dadosEnviar.aux <= 0 || dadosEnviar.aux > 2) {
									MessageBox(hWnd, TEXT("Apenas pode ser inserido valores entre 1 e 2"), TEXT(""), MB_OK);
									ipRecolhido = NULL;
									continue;
								}
							}
						} while (ipRecolhido == NULL);

						ov = preparaOverlap(ov);
						if (!WriteFile(hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
							if (GetLastError() == ERROR_IO_PENDING) {
								WaitForSingleObject(ov.hEvent, INFINITE);
								GetOverlappedResult(hPipe, &ov, &nEscritos, FALSE);
							}
						}
						if (dadosEnviar.aux == 2)
							SetWindowText(hText, TEXT("Vamos aguardar pelo 2 jogador"));
					}
					else if (dados.val == COMECAR) {
						//imprime(hWnd, TEXT("Estao à minha espera\n"), 20, 20);
						SetWindowText(hText, TEXT("Estavam à minha espera para comecar"));

					}
					else if (dados.val == TERMINAR) {
						//imprime(hWnd, TEXT("Não é possivel jogar\n"), 20, 20);
						SetWindowText(hText, TEXT("Não foi jogar neste momento."));
					}

					//dadosT.dadosBitMap = dadosBM;
					dadosT->hPipe = hPipe;
					dadosT->hwndJanela = hWnd;
					initX = (width / 2) - (dadosT->dadosBitMap->tamOfBitMap * (dados.dados.nColunas / 2));
					initY = (height / 2) - (dadosT->dadosBitMap->tamOfBitMap * (dados.dados.nLinhas / 2));
					dadosT->initX = &initX;
					dadosT->initY = &initY;
					dadosT->hwnTextField = hText;

					tme.cbSize = sizeof(tme);
					tme.hwndTrack = hWnd;
					tme.dwFlags = TME_HOVER | TME_LEAVE;
					tme.dwHoverTime = 3000;
					isTracking = TRUE;
					TrackMouseEvent(&tme);
					
					flag = 0;
					dadosT->flagSair = &flag;
					
					dadosT->hTreadSinalVida = CreateThread(NULL, 0, threadSinalVida, dadosT, 0, &idThreadSinalVida);

					hThread = CreateThread(NULL, 0, threadLeitura, dadosT, 0, &idThread);
					hAppMenu = GetMenu(hWnd);
					hSubMenu = GetSubMenu(hAppMenu, 0);
					EnableMenuItem(hSubMenu, ID_FILE_NEWGAME, MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);
				}
				else {
					SetWindowText(hText, TEXT("Não foi possivel estabelecer ligacao com o Servidor"));
				}
			}
			break;
		case ID_FILE_EXIT:
			
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case ID_MODE_LIGHT:
			changeBitmap(hWnd, 0, dadosT);
			break;

		case ID_MODE_DARK:
			changeBitmap(hWnd, 1, dadosT);
			break;

		}
		break;

	case WM_LBUTTONDOWN: {
		posX = GET_X_LPARAM(lParam);
		posY = GET_Y_LPARAM(lParam);
		if (dadosT->estado == INATIVO) {
			break;
		}
		WaitForSingleObject(dadosT->mutex, INFINITE);
		if (posX < *dadosT->initX || posX >((*dadosT->initX) + dadosT->dadosServ.dados.nColunas * dadosT->dadosBitMap->tamOfBitMap)) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		if (posY< *dadosT->initY || posY >((*dadosT->initY) + dadosT->dadosServ.dados.nLinhas * dadosT->dadosBitMap->tamOfBitMap)) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		ov = preparaOverlap(ov);
		dadosEnviar.x = (int)(posY - initY) / 32;
		dadosEnviar.y = (int)(posX - initX) / 32;
		dadosEnviar.val = JOGADAVALIDA;
		if (!WriteFile(hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
			if (GetLastError() == ERROR_IO_PENDING) {
				WaitForSingleObject(ov.hEvent, INFINITE);
				GetOverlappedResult(hPipe,&ov, &nEscritos,FALSE);
			}
		}
		if (sizeof(dadosEnviar) != nEscritos) {
			//imprime(hWnd, TEXT("Erro no envio das coordenadas"), 100, 0);
		}
		//InvalidateRect(hWnd, NULL, FALSE);
		ReleaseMutex(dadosT->mutex);
		break;
	}
	

	case WM_MOUSEMOVE: {
		

		if (!isTracking) {
			break;
		}

		posX = GET_X_LPARAM(lParam);
		posY = GET_Y_LPARAM(lParam);
		if (waterStop) {
			
			if (GET_Y_LPARAM(lParam) - (dadosEnviar.x * 32 + initY)  > 32  || GET_Y_LPARAM(lParam) - (dadosEnviar.x * 32 + initY) < 0
				|| GET_X_LPARAM(lParam) - (dadosEnviar.y * 32 + initX) > 32 || GET_X_LPARAM(lParam) - (dadosEnviar.y * 32 + initX) < 0) {
				
				dadosEnviar.val = MOUSEMOVE;
				ov = preparaOverlap(ov);
				if (!WriteFile(hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
					if (GetLastError() == ERROR_IO_PENDING) {
						WaitForSingleObject(ov.hEvent, INFINITE);
						GetOverlappedResult(hPipe, &ov, &nEscritos, FALSE);
					}
				}
				waterStop = FALSE;
				//imprime(hWnd, TEXT("Saiu da Celula"), 20, 600);
			}
			else {
				//imprime(hWnd, TEXT("Dentro da Celula"), 20, 650);
			}
			
		}

		if (posX < initX || posX >(initX + dadosT->dadosServ.dados.nColunas * dadosT->dadosBitMap->tamOfBitMap)) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		if (posY< initY || posY >(initY + dadosT->dadosServ.dados.nLinhas * dadosT->dadosBitMap->tamOfBitMap)) {
			ReleaseMutex(dadosT->mutex);
			break;
		}

		
//		SystemParametersInfo(SPI_SETMOUSEHOVERWIDTH, 32, NULL, 0);
//		SystemParametersInfo(SPI_SETMOUSEHOVERHEIGHT, 32, NULL, 0);

		tme.cbSize = sizeof(tme);
		tme.hwndTrack = hWnd;
		tme.dwFlags = TME_HOVER;
		tme.dwHoverTime = 2000;
		TrackMouseEvent(&tme);
		


		break;
	}
	case WM_RBUTTONDOWN: {
		
		
		posX = GET_X_LPARAM(lParam);
		posY = GET_Y_LPARAM(lParam);
		if (dadosT->estado == INATIVO) {
			break;
		}
		WaitForSingleObject(dadosT->mutex, INFINITE);
		if (posX < (*dadosT->initX) || posX >((*dadosT->initX) + dadosT->dadosServ.dados.nColunas * dadosT->dadosBitMap->tamOfBitMap)) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		if (posY< (*dadosT->initY) || posY > ((*dadosT->initY) + dadosT->dadosServ.dados.nLinhas * dadosT->dadosBitMap->tamOfBitMap)) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		ov = preparaOverlap(ov);
		dadosEnviar.x = (int)(posY - initY) / 32;
		dadosEnviar.y = (int)(posX - initX) / 32;
		dadosEnviar.val = RIGHT;
		if (!WriteFile(hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
			if (GetLastError() == ERROR_IO_PENDING) {
				WaitForSingleObject(ov.hEvent, INFINITE);
				GetOverlappedResult(hPipe, &ov, &nEscritos, FALSE);
			}
		}
		if (sizeof(dadosEnviar) != nEscritos) {
			imprime(hWnd, TEXT("Erro no envio das coordenadas"), 100, 0);
		}
		//InvalidateRect(hWnd, NULL, FALSE);
		ReleaseMutex(dadosT->mutex);
		break;
	}
	case WM_MOUSEHOVER: {

		if (!isTracking) {
			break;
		}
		//imprime(hWnd, TEXT("recebeu mouse over"), 20, 400);



		//imprime(hWnd, TEXT("passaram 2 segundos"), 20, 450);
		posX = GET_X_LPARAM(lParam);
		posY = GET_Y_LPARAM(lParam);
		WaitForSingleObject(dadosT->mutex, INFINITE);
		if (posX < *dadosT->initX || posX >((*dadosT->initX) + dadosT->dadosServ.dados.nColunas * dadosT->dadosBitMap->tamOfBitMap)) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		if (posY< *dadosT->initY || posY >((*dadosT->initY) + dadosT->dadosServ.dados.nLinhas * dadosT->dadosBitMap->tamOfBitMap)) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		dadosEnviar.x = (int)(posY - initY) / 32;
		dadosEnviar.y = (int)(posX - initX) / 32;
		if (dadosT->dadosServ.xAgua != dadosEnviar.x || dadosT->dadosServ.yAgua != dadosEnviar.y) {
			ReleaseMutex(dadosT->mutex);
			break;
		}
		ov = preparaOverlap(ov);
		
		dadosEnviar.val = MOUSEOVER;
		ReleaseMutex(dadosT->mutex);
		if (!WriteFile(hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov)) {
			if (GetLastError() == ERROR_IO_PENDING) {
				WaitForSingleObject(ov.hEvent, INFINITE);
				GetOverlappedResult(hPipe, &ov, &nEscritos, FALSE);
			}
		}
		if (nEscritos != sizeof(dadosEnviar)) {
			imprime(hWnd, TEXT("Problemas a enviar agua"), 20, 500);
		}else
			imprime(hWnd, TEXT("Enviou agua"), 20, 500);
		waterStop = TRUE;

		break;
	}
					  

	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &pcs);
		GetClientRect(hWnd, &rect);

		if (memDC == NULL) {

			memDC = CreateCompatibleDC(hdc);
			hBitmapDB = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			SelectObject(memDC, hBitmapDB);
			DeleteObject(hBitmapDB);

			dadosT->memDC = &memDC;

			FillRect(memDC, &rect, CreateSolidBrush(RGB(255, 255, 255)));

		}
		WaitForSingleObject(dadosT->mutex, INFINITE);
		if (!BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY)) {
			//imprime(hWnd, TEXT("Erro na impressao"), 0, 0);

		}
		ReleaseMutex(dadosT->mutex);
		
		
		EndPaint(hWnd, &pcs);


		break;
	}
	case WM_SIZE: {


		if(dadosT != NULL && dadosT->estado == ATIVO){
			WaitForSingleObject(dadosT->mutex, INFINITE);
			//memDC = NULL;
			GetClientRect(hWnd, &rect);
			if ((height / 2) - (dadosT->dadosBitMap->tamOfBitMap * (dadosT->dadosServ.dados.nLinhas / 2)) <= 102 ) {
				ReleaseMutex(dadosT->mutex);
				break;
			}
			width = rect.right;
			height = rect.bottom;
			initX = (width / 2) - (dadosT->dadosBitMap->tamOfBitMap * (dadosT->dadosServ.dados.nColunas / 2));
			initY = (height / 2) - (dadosT->dadosBitMap->tamOfBitMap * (dadosT->dadosServ.dados.nLinhas / 2));
			ReleaseMutex(dadosT->mutex);
		}
		break;
	}
	case WM_CLOSE:

		if(wParam != 1){
			result = MessageBox(hWnd, TEXT("Deseja mesmo sair?"), TEXT("DE SAIDA?"), MB_YESNOCANCEL);
			// Destruir a janela e terminar o programa 
							// "PostQuitMessage(Exit Status)"		
			switch (result) {
			case IDCANCEL:
				return 0;
			case IDYES:
				
				if (dadosT->estado == ATIVO) {
					WaitForSingleObject(dadosT->mutex, INFINITE);
					flag = 1;
					ReleaseMutex(dadosT->mutex);
					SetEvent(dadosT->ov->hEvent);
					ZeroMemory(&ov, sizeof(ov));
					ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
					dadosEnviar.val = DESISTIR;
					WriteFile(hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov);
					FlushFileBuffers(hPipe);
					CloseHandle(hPipe);
					WaitForSingleObject(hThread,INFINITE);
					
				}else if(dados.val == ESPERAR && dadosEnviar.aux == 2 && dadosT->hPipe != NULL && dadosT->estado == INATIVO) {
					WaitForSingleObject(dadosT->mutex, INFINITE);
					flag = 1;
					ReleaseMutex(dadosT->mutex);
					SetEvent(dadosT->ov->hEvent);
					ZeroMemory(&ov, sizeof(ov));
					ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
					HANDLE hPipeAux = CreateFile(NAMEPIPE, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
					dadosEnviar.val = DESISTIR;
					WriteFile(hPipe, &dadosEnviar, sizeof(dadosEnviar), &nEscritos, &ov);
					CloseHandle(hPipe);
				}
				CloseResources(dadosT);
				PostQuitMessage(0);

				break;
			case IDNO:
				return 0;
			}
		}
		else {
			MessageBox(hWnd, TEXT("O Servidor mandou encerrar"), TEXT(""), MB_OK);
			CloseResources(dadosT);
			PostQuitMessage(0);
		}
	case WM_DESTROY:

		PostQuitMessage(0);
		break;

	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar")
		// n?o ? efectuado nenhum processamento, apenas se segue o "default" do Windows
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;  // break tecnicamente desnecess?rio por causa do return
	}
	return 0;



	
}



