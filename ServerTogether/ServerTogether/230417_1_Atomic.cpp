#include "Include.h"

/****************************************************
 * Atomic Int 사용하기
****************************************************/

/*
atomic<int> sum = 0;

//volatile int sum = 0;

void worker(int num_th)
{
	for (int i = 0; i < 50000000 / num_th; ++i)
		sum += 2;
	// sum = sum + 2 >> atomic 선언해도 data race
}

int main()
{
	for (int num_core = 1; num_core <= 16; num_core *= 2)
	{
		sum = 0;

		vector<thread> workers;
		for (int i = 0; i < num_core; ++i)
			workers.emplace_back(worker, num_core);
		for (auto& th : workers)
			th.join();

		cout << num_core << "Threads, Sum = " << sum << endl;
	}
}
 */


/****************************************************
 * 피터슨 알고리즘 Atomic으로 제대로 되도록 하기
****************************************************/

//volatile int victim;
atomic<int> victim;
//volatile bool p_flag[2] = { false, false };
atomic<bool> p_flag[2] = { false, false };

volatile int sum = 0;

void p_lock(int tid)
{
	int other = 1 - tid;	// 딴놈은 누구냐
	p_flag[tid] = true;
	victim = tid;

	///****************************************************
	// 아토믹하게 하면 제대로 잘 된다 ! ! !
	// atomic_thread_fence(memory_order_seq_cst);
	///****************************************************

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
