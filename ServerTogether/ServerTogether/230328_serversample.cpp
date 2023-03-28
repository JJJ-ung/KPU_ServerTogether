#include <iostream>
#include <array>
#include <unordered_map>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
constexpr int MAX_USER = 10;		// 최대 동접

enum COMP_TYPE  { OP_ACCEPT, OP_RECV, OP_SEND };	// 어떤게 완료가 된건가
class OVER_EXP {
public:
	WSAOVERLAPPED _over;	// 같은 주소를 가져야 하니까 맨 앞에 있어야만 함
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	OVER_EXP()
	{
		// 변수 초기화
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		// send용으로 사용할 생성자 (버퍼를 보내야 하니까)
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

class SESSION {
	OVER_EXP _recv_over;

public:
	//bool in_use;	// 지금 세션이 사용중인지 아닌지 : 배열에서 사용되는거 확인용도
	int _id;
	SOCKET _socket;
	short	x, y;
	char	_name[NAME_SIZE];

	int		_prev_remain;	// 패킷 재조립때 잘려진 찌꺼기
public:
	SESSION(int id, SOCKET sock) : _id(id), _socket(sock), x(0), y(0), _prev_remain(0)
	{
		_name[0] = 0;
	}

	SESSION()
	{
		cout << "ERROR" << endl;
		exit(-1);
	}

	~SESSION()
	{
		closesocket(_socket);
	}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
			// 지난번에 받은 부분부터가 시작점, 버퍼 길이도 그에 맞춰서
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, 
			& _recv_over._over, 0);
	}

	void do_send(void *packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char *>(packet) };
			// 버퍼 관리는 생성자에서 모두 해줌, 넘기기만 하면 됨
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}
	void send_login_info_packet()
	{
		SC_LOGIN_INFO_PACKET p;
		p.id = _id;
		p.size = sizeof(SC_LOGIN_INFO_PACKET);
		p.type = SC_LOGIN_INFO;
		p.x = x;
		p.y = y;
		do_send(&p);
	}
	void send_move_packet(size_t c_id);	// 넣어준 아이디의 위치 정보를 보내는 것
};

//array<SESSION, MAX_USER> clients;
unordered_map<size_t, SESSION> clients;
	// 64bit, 32bit wat da gat da : size_t is good

void SESSION::send_move_packet(size_t c_id)
{
	SC_MOVE_PLAYER_PACKET p;
	p.id = static_cast<short>(c_id);
	p.size = sizeof(SC_MOVE_PLAYER_PACKET);
	p.type = SC_MOVE_PLAYER;
	p.x = clients[c_id].x;
	p.y = clients[c_id].y;
	do_send(&p);
}

int get_new_client_id()
{
	//for (int i = 0; i < MAX_USER; ++i)
	//	if (clients[i].in_use == false)
	//		return i;
	for (int i = 0; i < MAX_USER; ++i)
		if (clients.count(i) == 0) return i;
	return -1;
}

void process_packet(size_t c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		strcpy_s(clients[c_id]._name, p->name);	// name copy from packet
		clients[c_id].send_login_info_packet();

			// send new guy info to other clients
		SC_ADD_PLAYER_PACKET add_packet;
		add_packet.id = static_cast<short>(c_id);
		strcpy_s(add_packet.name, p->name);
		add_packet.size = sizeof(add_packet);
		add_packet.type = SC_ADD_PLAYER;
		add_packet.x = clients[c_id].x;
		add_packet.y = clients[c_id].y;
			// reuse packet because same info
		for (auto& pl : clients) {
			if (pl.second._id == c_id) continue;	// if new guy, skip loop
			pl.second.do_send(&add_packet);
		}
			// send other player info to new guy
		for (auto& pl : clients) {
			if (pl.second._id == c_id) continue;
			SC_ADD_PLAYER_PACKET add_packet;
			add_packet.id = pl.second._id;
			strcpy_s(add_packet.name, pl.second._name);
			add_packet.size = sizeof(add_packet);
			add_packet.type = SC_ADD_PLAYER;
			add_packet.x = pl.second.x;
			add_packet.y = pl.second.y;
			clients[c_id].do_send(&add_packet);
		}
		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
		short x = clients[c_id].x;
		short y = clients[c_id].y;
		switch (p->direction) {
		case 0: if (y > 0) y--; break;
		case 1: if (y < W_HEIGHT - 1) y++; break;
		case 2: if (x > 0) x--; break;
		case 3: if (x < W_WIDTH - 1) x++; break;
		}
		clients[c_id].x = x;
		clients[c_id].y = y;
			// send move info to all clients
		for (auto &pl : clients) 
			pl.second.send_move_packet(c_id);
		break;
	}
	}
}

void disconnect(int c_id)
{
	// send all clients to remove player
	for (auto& pl : clients) {
		if (pl.second._id == c_id) continue;
		// if remove guy is me, skip
		SC_REMOVE_PLAYER_PACKET p;
		p.id = c_id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_PLAYER;
		pl.second.do_send(&p);
	}
	clients.erase(c_id);	// erase + destructor (close socket is here)
	//closesocket(clients[c_id]._socket);
}

int main()
{
	HANDLE h_iocp;	// iocp handle

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	SOCKET server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(server, SOMAXCONN);

	int addr_size = sizeof(SOCKADDR_IN);
	int client_id = 0;

	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server), h_iocp, 9999, 0);
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	OVER_EXP a_over;
	a_over._comp_type = OP_ACCEPT;
	AcceptEx(server, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);

	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
			// 에러 체크하는 값
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP *>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT)
			{
				// 서버가 맛간거니까 그냥 프로그램 죽여야해
				cout << "Accept Error";
				exit(-1);
			}
			else {
				// 클라쪽 에러임 >> disconnet 알려주어야 함
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;		// send에서 new한거 잊지말고 죽여주세요
				continue;
			}
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {	// accept를 받으면 : 세션을 만들고, 컨테이너에 넣고, 새로 다시 accept 받고
			int client_id = get_new_client_id();
			if (client_id != -1) {
				//SESSION client{ client_id, c_socket };
				clients.try_emplace(client_id, client_id, c_socket);
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket),
					h_iocp, client_id, 0);
				// 여기서 소켓 사용했으니까 다시 받으려면 새로 만들어주어야 함
				clients[client_id].do_recv();
			}
			else {
				cout << "Max user exceeded.\n";
				closesocket(c_socket);
			}
			c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				// 여기서 소켓 새로 만들기
			ZeroMemory(&a_over._over, sizeof(a_over._over));
			AcceptEx(server, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);
			break;
		}
		case OP_RECV: {
			int remain_data = clients[key]._prev_remain + num_bytes;	// 찌꺼기까지 포함해서 처리
			char* p = ex_over->_send_buf;	// 맨 앞 주소가 같으니까~ p는 패킷의 시작 위치임
			while (remain_data > 0) {
				int packet_size = p[0];	// 모든 패킷의 맨 첫 멤버 변수가 사이즈라서
				if (packet_size <= remain_data) {	// 패킷 사이즈만큼 다 있을때 조립할 수 있는거
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {	// 패킷 조각을 버퍼의 맨 앞으로 이동 시키기
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
			break;
		}
		case OP_SEND:
			delete ex_over;
			break;
		}
	}
	closesocket(server);
	WSACleanup();
}
