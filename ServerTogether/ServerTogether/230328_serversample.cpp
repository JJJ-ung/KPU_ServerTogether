#include <iostream>
#include <array>
#include <unordered_map>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "protocol.h"

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
using namespace std;
constexpr int MAX_USER = 10;		// �ִ� ����

enum COMP_TYPE  { OP_ACCEPT, OP_RECV, OP_SEND };	// ��� �Ϸᰡ �Ȱǰ�
class OVER_EXP {
public:
	WSAOVERLAPPED _over;	// ���� �ּҸ� ������ �ϴϱ� �� �տ� �־�߸� ��
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	OVER_EXP()
	{
		// ���� �ʱ�ȭ
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		// send������ ����� ������ (���۸� ������ �ϴϱ�)
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
	//bool in_use;	// ���� ������ ��������� �ƴ��� : �迭���� ���Ǵ°� Ȯ�ο뵵
	int _id;
	SOCKET _socket;
	short	x, y;
	char	_name[NAME_SIZE];

	int		_prev_remain;	// ��Ŷ �������� �߷��� ���
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
			// �������� ���� �κк��Ͱ� ������, ���� ���̵� �׿� ���缭
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, 
			& _recv_over._over, 0);
	}

	void do_send(void *packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char *>(packet) };
			// ���� ������ �����ڿ��� ��� ����, �ѱ�⸸ �ϸ� ��
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
	void send_move_packet(size_t c_id);	// �־��� ���̵��� ��ġ ������ ������ ��
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
			// ���� üũ�ϴ� ��
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP *>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT)
			{
				// ������ �����Ŵϱ� �׳� ���α׷� �׿�����
				cout << "Accept Error";
				exit(-1);
			}
			else {
				// Ŭ���� ������ >> disconnet �˷��־�� ��
				cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;		// send���� new�Ѱ� �������� �׿��ּ���
				continue;
			}
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {	// accept�� ������ : ������ �����, �����̳ʿ� �ְ�, ���� �ٽ� accept �ް�
			int client_id = get_new_client_id();
			if (client_id != -1) {
				//SESSION client{ client_id, c_socket };
				clients.try_emplace(client_id, client_id, c_socket);
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket),
					h_iocp, client_id, 0);
				// ���⼭ ���� ��������ϱ� �ٽ� �������� ���� ������־�� ��
				clients[client_id].do_recv();
			}
			else {
				cout << "Max user exceeded.\n";
				closesocket(c_socket);
			}
			c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
				// ���⼭ ���� ���� �����
			ZeroMemory(&a_over._over, sizeof(a_over._over));
			AcceptEx(server, c_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);
			break;
		}
		case OP_RECV: {
			int remain_data = clients[key]._prev_remain + num_bytes;	// ������ �����ؼ� ó��
			char* p = ex_over->_send_buf;	// �� �� �ּҰ� �����ϱ�~ p�� ��Ŷ�� ���� ��ġ��
			while (remain_data > 0) {
				int packet_size = p[0];	// ��� ��Ŷ�� �� ù ��� ������ �������
				if (packet_size <= remain_data) {	// ��Ŷ �����ŭ �� ������ ������ �� �ִ°�
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {	// ��Ŷ ������ ������ �� ������ �̵� ��Ű��
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