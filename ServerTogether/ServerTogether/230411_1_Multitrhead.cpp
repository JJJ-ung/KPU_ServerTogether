
/****************************************************
 * ������ ����ȭ
****************************************************/

/*
#include "Include.h"

int send_data = 0;
bool send_flag = false;

void receiver()
{
	while (false == send_flag);
	cout << send_data;
}

void sender()
{
	send_data = 999;
	send_flag = true;
}

int main()
{
	thread r{ receiver };
	thread s{ sender };
	r.join();
	s.join();
}
*/

/****************************************************
 * ���־��� ����ȭ ��⸦ ���ؽ��� �����ϱ�
****************************************************/

/*
#include "Include.h"

int send_data = 0;
bool send_flag = false;
mutex ll;

void receiver()
{
	ll.lock();
	while (false == send_flag);
	cout << send_data;
	ll.unlock();
}

void sender()
{
	ll.lock();
	send_data = 999;
	send_flag = true;
	ll.unlock();
}

int main()
{
	thread r{ receiver };
	thread s{ sender };
	r.join();
	s.join();
}

// ���ɶ����� �� �Ⱦ��� ������� �ߴµ� �̰����� ������ ��ĥ�� �ߴ�
*/

/****************************************************
 * ���־��� ����ȭ ��⸦ volatile�� �����ϱ�
****************************************************/

/*
#include "Include.h"

volatile int send_data = 0;
volatile bool send_flag = false;

void receiver()
{
	while (false == send_flag);
	cout << send_data;
}

void sender()
{
	send_data = 999;
	send_flag = true;
}

int main()
{
	thread r{ receiver };
	thread s{ sender };
	r.join();
	s.join();
}
 */

 /****************************************************
  * ���־��� ����ȭ ��⸦ volatile �����ͷ� �����ϱ�
 ****************************************************/

/*
#include "Include.h"

//volatile int* send_data = nullptr;
// * ���α׷� ����
// * send data�� ������ �ִ� �����ʹ� volatile, �а� ���°� ����ȭ�� ���� ����
// * ������ ������ volatile�� �ƴ�, �ּҸ� ��� ���� ����ȭ ����
// * int�� volatile�� �ƴ϶� �����Ͱ� volatile�̾�� �Ѵ�.
int* volatile send_data = nullptr;
// �̷��� �����Ͱ� volatile�� �ȴ�.

void receiver()
{
	while (nullptr == send_data);
	cout << *send_data;
	delete send_data;
}

void sender()
{
	send_data = new int(999);
}

int main()
{
	thread r{ receiver };
	thread s{ sender };
	r.join();
	s.join();
}
*/

/****************************************************
 * ���ͽ� �˰���
****************************************************/

/*
#include "Include.h"

volatile int victim;
volatile bool p_flag[2] = { false, false };

volatile int sum = 0;

void p_lock(int tid)
{
	int other = 1 - tid;	// ������ ������
	p_flag[tid] = true;
	victim = tid;
	while ((p_flag[other] == true) && (victim == tid));	// ������ ���� ��ٸ���
}

void p_unlock(int tid)
{
	p_flag[tid] = false;
}

void worker(int tid)
{
	for(int i = 0; i < 25000000; ++i)
	{
		p_lock(tid);
		sum = sum + 2;
		p_unlock(tid);
	}
}

int main()
{
	thread t1{ worker, 0 };
	thread t2{ worker, 1 };
	t1.join();
	t2.join();
	cout << sum << endl;
}

// ������ �Ѵ�.
// �˰��� ����ΰ� �����Ϸ��� �߸� ����.
// ������ CPU */

/****************************************************
 * out of order ���Ƽ� ���� �ڼ��̴°� ����
****************************************************/

#include "Include.h"

volatile int victim;
volatile bool p_flag[2] = { false, false };

volatile int sum = 0;

void p_lock(int tid)
{
	int other = 1 - tid;	// ������ ������
	p_flag[tid] = true;
	victim = tid;
	atomic_thread_fence(memory_order_seq_cst);
	while ((p_flag[other] == true) && (victim == tid));	// ������ ���� ��ٸ���
}

void p_unlock(int tid)
{
	p_flag[tid] = false;
}

void worker(int tid)
{
	for (int i = 0; i < 25000000; ++i)
	{
		p_lock(tid);
		sum = sum + 2;
		p_unlock(tid);
	}
}

int main()
{
	thread t1{ worker, 0 };
	thread t2{ worker, 1 };
	t1.join();
	t2.join();
	cout << sum << endl;
}
