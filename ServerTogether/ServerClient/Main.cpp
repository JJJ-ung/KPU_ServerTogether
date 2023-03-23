//#include <Windows.h>
#include <iostream>
#include <WS2tcpip.h>
#include "resource.h"

#pragma comment (lib, "WS2_32.LIB")
const char* SERVER_ADDR = "127.0.0.1";
const short SERVER_PORT = 4000;
const int BUFSIZE = 256;

#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

struct Vector2
{
	int x;
	int y;
};
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
	void MoveRight(int pos_x)
	{
		if ((x + pos_x) < 640 && (x + pos_x) >= 0)
			x += pos_x;
	}
	void MoveUp(int pos_y)
	{
		if((y + pos_y) < 640 && (y + pos_y) >= 0)
			y += pos_y;
	}
	void Move(char* buf)
	{
		WPARAM data = (WPARAM)*buf;

		if (data == VK_UP)
		{
			MoveUp(-80);
		}
		if (data == VK_DOWN)
		{
			MoveUp(80);
		}
		if (data == VK_LEFT)
		{
			MoveRight(-80);
		}
		if (data == VK_RIGHT)
		{
			MoveRight(80);
		}
	}
};


void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러 " << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
}

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Chess";
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
SOCKET s_socket;
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

	std::wcout.imbue(std::locale("korean")); // 한글 출력을 위함
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	 //TCP 로 소켓 생성
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);

	int ret = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (0 != ret)
	{
		int err_no = WSAGetLastError();
		error_display("connect : ", err_no);
	}
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

	DWORD recv_byte;
	DWORD recv_flag = 0;
	int mx, my;
	int ret = 0;
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
		char buf[BUFSIZE];

		memset(buf, wParam, sizeof(WPARAM));
		DWORD sent_byte;
		WSABUF mybuf[1];
		mybuf[0].buf = (char*) & wParam;
		mybuf[0].len = static_cast<ULONG>(strlen(buf)) + 1;
		ret = WSASend(s_socket, mybuf, 1, &sent_byte, 0, 0, 0);

		if (0 != ret)
		{
			int err_no = WSAGetLastError();
			error_display("WSASend : ", err_no);
		}


		char recv_buf[BUFSIZE];
		WSABUF mybuf_r[1];
		mybuf_r[0].buf = recv_buf; mybuf_r[0].len = BUFSIZE;

		WSARecv(s_socket, mybuf_r, 1, &recv_byte, &recv_flag, 0, 0);
		Vector2 RecvedData;
		memcpy(&RecvedData, mybuf_r[0].buf, sizeof(RecvedData));
		std::cout << "X : " << RecvedData.x << ", Y : " << RecvedData.y << std::endl;
		piece.SetPos(RecvedData.x, RecvedData.y);
		InvalidateRect(hWnd, NULL, false);

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