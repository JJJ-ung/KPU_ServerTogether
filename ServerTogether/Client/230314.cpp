#include <iostream>
#include <WS2tcpip.h>
using namespace std;

#pragma comment (lib, "WS2_32.LIB")
	// 32인 이유 : 32비트라는게 아니고 16비트가 아니라는 뜻임 (16비트만 아니면 이거 모두 사용), 16비트는 옛날옛날

const char* SERVER_ADDR = "127.0.0.1";
	// 127.0.0.1 : 내 컴퓨터의 주소 (남의꺼 연결하려면 남의꺼 IP주소로)

const short SERVER_PORT = 4000;
const int BUFSIZE = 256;

int main()
{
	wcout.imbue(locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	for (;;) {
		char buf[BUFSIZE];
		cout << "Enter Message : "; cin.getline(buf, BUFSIZE);
		DWORD sent_byte;
		WSABUF mybuf;
		mybuf.buf = buf; mybuf.len = static_cast<ULONG>(strlen(buf)) + 1;
		WSASend(234, &mybuf, 1, &sent_byte, 0, 0, 0);
		char recv_buf[BUFSIZE];
		WSABUF mybuf_r;
		mybuf_r.buf = recv_buf; mybuf_r.len = BUFSIZE;
		DWORD recv_byte;
		DWORD recv_flag = 0;
		WSARecv(s_socket, &mybuf_r, 1, &recv_byte, &recv_flag, 0, 0);
		cout << "Server Sent [" << recv_byte << "bytes] : " << recv_buf << endl;
	}
	WSACleanup();
}