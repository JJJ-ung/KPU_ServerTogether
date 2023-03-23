#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")
const short SERVER_PORT = 4000;
const int BUFSIZE = 256;

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
	Vector2 GetXYPos() { return Vector2{ x, y }; }
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
		if ((y + pos_y) < 640 && (y + pos_y) >= 0)
			y += pos_y;
	}
	void Move(WSABUF buf)
	{
		WPARAM data;
		memcpy(&data, buf.buf, sizeof(WPARAM));
		if (data == VK_UP)
		{
			std::cout << "Move Up" << std::endl;
			MoveUp(-80);
		}
		if (data == VK_DOWN)
		{
			std::cout << "Move Down" << std::endl;
			MoveUp(80);
		}
		if (data == VK_LEFT)
		{
			std::cout << "Move Left" << std::endl;
			MoveRight(-80);
		}
		if (data == VK_RIGHT)
		{
			std::cout << "Move Right" << std::endl;
			MoveRight(80);
		}
	}
};




int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(s_socket, SOMAXCONN);
	Piece piece(80, 80);

	INT addr_size = sizeof(server_addr);
	SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
	for (;;) {
		char recv_buf[BUFSIZE];
		WSABUF mybuf[1];
		mybuf[0].buf = recv_buf; mybuf[0].len = BUFSIZE;
		DWORD recv_byte;
		DWORD recv_flag = 0;
		WSARecv(c_socket, mybuf, 1, &recv_byte, &recv_flag, 0, 0);
		std::cout << "Data Recved" << std::endl;
		std::cout << piece.Get_X() << std::endl;

		piece.Move(mybuf[0]);
		Vector2 SendPos = piece.GetXYPos();
		DWORD sent_byte;
		mybuf[0].buf = (char*)&SendPos;
		mybuf[0].len = sizeof(SendPos);
		WSASend(c_socket, mybuf, 1, &sent_byte, 0, 0, 0);
		std::cout << "Data Send" << std::endl;

	}
	closesocket(c_socket);
	closesocket(s_socket);
	WSACleanup();
}