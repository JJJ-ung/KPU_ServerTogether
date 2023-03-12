#include <Windows.h>
#include "resource.h"

#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "msimg32.lib")


//#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

class Piece
{
	int x = 0;
	int y = 0;
public:
	Piece() {}
	Piece(int pos_x, int pos_y) : x(pos_x), y(pos_y)
	{

	}
	~Piece() {}

	int Get_X() { return x; }
	int Get_Y() { return y; }
	void SetPos(int pos_x, int pos_y)
	{
		x = pos_x;
		y = pos_y;
	}

};



HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Chess";
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND hWnd;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{

	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_SYSMENU, 0, 0, 650, 680, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc, memdc;
	static HBITMAP Board, Knight;
	static Piece piece(80, 80);
	RECT r = { 0, 0, 640, 640 };
	int mx, my;
	switch (iMessage) {
	case WM_CREATE:
		Board = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		Knight = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		break;

	case WM_LBUTTONDOWN:
		mx = LOWORD(lParam);
		my = HIWORD(lParam);

		if (mx >= 0 && mx < 80)
		{
			mx = 10;
		}
		else if (mx >= 80 && mx < 160)
		{
			mx = 90;
		}
		else if (mx >= 160 && mx < 240)
		{
			mx = 170;
		}
		else if (mx >= 240 && mx < 320)
		{
			mx = 250;
		}
		else if (mx >= 320 && mx < 400)
		{
			mx = 330;

		}
		else if (mx >= 400 && mx < 480)
		{
			mx = 410;
		}
		else if (mx >= 480 && mx < 560)
		{
			mx = 490;
		}
		else if (mx >= 560 && mx < 640)
		{
			mx = 570;
		}


		if (my >= 0 && my < 80)
		{
			my = 10;
		}
		else if (my >= 80 && my < 160)
		{
			my = 90;
		}
		else if (my >= 160 && my < 240)
		{
			my = 170;
		}
		else if (my >= 240 && my < 320)
		{
			my = 250;
		}
		else if (my >= 320 && my < 400)
		{
			my = 330;
		}
		else if (my >= 400 && my < 480)
		{
			my = 410;
		}
		else if (my >= 480 && my < 560)
		{
			my = 490;
		}
		else if (my >= 560 && my < 640)
		{
			my = 570;
		}


		piece.SetPos(mx, my);
		InvalidateRect(hWnd, NULL, false);
		break;
	case WM_KEYDOWN:
		break;
	case WM_PAINT:
		static HBITMAP BackBit, oldBackBit;
	
		hdc = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hdc); // 메모리DC 생성
		oldBackBit = (HBITMAP)SelectObject(memdc, Board); //비트맵 선택

		BitBlt(hdc, 0, 0, 640, 640, memdc, 0, 0, SRCCOPY); //복사 및 출력

		SelectObject(memdc, Knight);
		BitBlt(hdc, piece.Get_X(), piece.Get_Y(), 640, 640, memdc, 0, 0, SRCCOPY); //복사 및 출력

		SelectObject(memdc, oldBackBit);
		DeleteObject(BackBit);
		EndPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
		break;
	case WM_TIMER:
		break;
	case WM_DESTROY:	
		PostQuitMessage(0);
		return 0;
		break;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}