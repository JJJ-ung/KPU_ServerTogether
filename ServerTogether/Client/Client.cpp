#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")
const char* SERVER_ADDR = "127.0.0.1";
const short SERVER_PORT = 4000;
const int BUFSIZE = 256;

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



int main()
{
	wcout.imbue(locale("korean")); // 한글 출력을 위함
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0); //TCP 로 소켓 생성
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	int ret = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	for (;;) {
		char buf[BUFSIZE];
		cout << "Enter Message : "; cin.getline(buf, BUFSIZE);
		DWORD sent_byte;
		WSABUF mybuf[1];
		mybuf[0].buf = buf; 
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
		DWORD recv_byte;
		DWORD recv_flag = 0;
		WSARecv(s_socket, mybuf_r, 1, &recv_byte, &recv_flag, 0, 0);
		cout << "Server Sent [" << recv_byte << "bytes] : " << recv_buf << endl;
	}
	closesocket(s_socket);
	WSACleanup();
}