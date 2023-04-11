
/****************************************************
 * 간단한 동기화
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
 * 비주얼의 최적화 사기를 뮤텍스로 파훼하기
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

// 성능때문에 락 안쓰고 만들려고 했는데 이것조차 락으로 떡칠을 했다
*/

/****************************************************
 * 비주얼의 최적화 사기를 volatile로 파훼하기
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
  * 비주얼의 최적화 사기를 volatile 포인터로 파훼하기
 ****************************************************/

/*
#include "Include.h"

//volatile int* send_data = nullptr;
// * 프로그램 터짐
// * send data가 가지고 있는 데이터는 volatile, 읽고 쓰는건 최적화를 하지 않음
// * 포인터 변수는 volatile이 아님, 주소를 담는 일은 최적화 당함
// * int가 volatile이 아니라 포인터가 volatile이어야 한다.
int* volatile send_data = nullptr;
// 이러면 포인터가 volatile이 된다.

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
 * 피터슨 알고리즘
****************************************************/

/*
#include "Include.h"

volatile int victim;
volatile bool p_flag[2] = { false, false };

volatile int sum = 0;

void p_lock(int tid)
{
	int other = 1 - tid;	// 딴놈은 누구냐
	p_flag[tid] = true;
	victim = tid;
	while ((p_flag[other] == true) && (victim == tid));	// 딴놈이 쓰면 기다리자
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

// 오동작 한다.
// 알고리즘도 제대로고 컴파일러도 잘못 없다.
// 범인은 CPU */

/****************************************************
 * out of order 막아서 순서 뒤섞이는거 막기
****************************************************/

#include "Include.h"

volatile int victim;
volatile bool p_flag[2] = { false, false };

volatile int sum = 0;

void p_lock(int tid)
{
	int other = 1 - tid;	// 딴놈은 누구냐
	p_flag[tid] = true;
	victim = tid;
	atomic_thread_fence(memory_order_seq_cst);
	while ((p_flag[other] == true) && (victim == tid));	// 딴놈이 쓰면 기다리자
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
