#include <iostream>
#include <WS2tcpip.h>
#include <unordered_map>
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
	Piece() { x = 0, y = 0; }
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
	void Move(char data)
	{
		//WPARAM data;
		//memcpy(&data, buf, sizeof(WPARAM));
		//std::cout << buf << std::endl;
		//std::cout << data << std::endl;
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

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag);

class EXP_OVER {
public:
	WSAOVERLAPPED _wsa_over;
	unsigned long long _s_id;
	WSABUF _wsa_buf;
	char _send_msg[BUFSIZE];
public:
	EXP_OVER(unsigned long long s_id, char num_bytes, const char* mess) : _s_id(s_id)
	{
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		_wsa_buf.buf = _send_msg;
		_wsa_buf.len = num_bytes + 2;

		memcpy(_send_msg + 2, mess, num_bytes);
		_send_msg[0] = num_bytes + 2;
		_send_msg[1] = static_cast<char>(s_id);
	}
	EXP_OVER(unsigned long long s_id)
	{
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		_wsa_buf.buf = _send_msg;
		_wsa_buf.len = 3;

		memset(_send_msg + 2, -1, sizeof(char));
		_send_msg[0] = 3;
		_send_msg[1] = static_cast<char>(s_id);
	}
	~EXP_OVER() {}
};


class SESSION {
private:
	unsigned long long   _id;
	
	WSABUF _send_wsabuf;
	WSAOVERLAPPED _recv_over;
	SOCKET _socket;


public:
	Piece piece; 
	WSABUF _recv_wsabuf;
	char   _recv_buf[BUFSIZE];
	SESSION() {
		cout << "Unexpected Constructor Call Error!\n";
		exit(-1);
	}
	SESSION(int id, SOCKET s) : _id(id), _socket(s) {
		_recv_wsabuf.buf = _recv_buf;    _recv_wsabuf.len = BUFSIZE;
		_send_wsabuf.buf = _recv_buf;    _send_wsabuf.len = 0;
	}
	~SESSION() { closesocket(_socket); }
	void do_recv() {
		DWORD recv_flag = 0;
		ZeroMemory(&_recv_over, sizeof(_recv_over));
		_recv_over.hEvent = reinterpret_cast<HANDLE>(_id);
		WSARecv(_socket, &_recv_wsabuf, 1, 0, &recv_flag, &_recv_over, recv_callback);
	}

	void do_send(unsigned long long sender_id, int num_bytes, const char* buff) {
		EXP_OVER* send_over = new EXP_OVER(sender_id, num_bytes, buff);
		WSASend(_socket, &send_over->_wsa_buf, 1, 0, 0, &send_over->_wsa_over, send_callback);
	}

	void do_send_leave(unsigned long long leaved_id)
	{
		EXP_OVER* send_over = new EXP_OVER(leaved_id);
		WSASend(_socket, &send_over->_wsa_buf, 1, 0, 0, &send_over->_wsa_over, send_callback);
	}
};

unordered_map <unsigned long long, SESSION> clients;

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag)
{
	delete reinterpret_cast<EXP_OVER*>(send_over);
}

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	unsigned long long s_id = reinterpret_cast<unsigned long long>(recv_over->hEvent);
	cout << "Client [" << s_id << "] Sent[" << num_bytes << "bytes] : " << clients[s_id]._recv_buf << endl;
	if (clients[s_id]._recv_buf[0] == -1)
	{
		clients.erase(s_id);
		for (auto& cl : clients)
			cl.second.do_send_leave(s_id);
		return;
	}
	clients[s_id].piece.Move(clients[s_id]._recv_buf[0]);
	Vector2 SendPos = clients[s_id].piece.GetXYPos();
	char buf[BUFSIZE];
	memcpy(buf, &SendPos, sizeof(SendPos));
	for (auto& cl : clients)
		cl.second.do_send(s_id, sizeof(Vector2), buf);
	clients[s_id].do_recv();
	SleepEx(0, true);
}



int main()
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(s_socket, SOMAXCONN);

	INT addr_size = sizeof(server_addr);
	for (int i = 1; ; ++i) {
		SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
		clients.try_emplace(i, i, c_socket);
		std::cout << "[" << i << "]" << " Player Join" << std::endl;

		char buf[BUFSIZE];
		//clients[i].do_send(i, sizeof(Vector2), buf);
		Vector2 SendPos;

		for (auto& cl : clients) {
			SendPos = clients[i].piece.GetXYPos();
			memcpy(buf, &SendPos, sizeof(SendPos));
			cl.second.do_send(i, sizeof(Vector2), buf);
			SendPos = cl.second.piece.GetXYPos();
			memcpy(buf, &SendPos, sizeof(Vector2));
			clients[i].do_send(cl.first, sizeof(Vector2), buf);
		}
		clients[i].do_recv();
		
	}

	closesocket(s_socket);
	WSACleanup();
}