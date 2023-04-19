#include "Include.h"

/****************************************************
 * Atomic Int ����ϱ�
****************************************************/

/*
atomic<int> sum = 0;

//volatile int sum = 0;

void worker(int num_th)
{
	for (int i = 0; i < 50000000 / num_th; ++i)
		sum += 2;
	// sum = sum + 2 >> atomic �����ص� data race
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
 * ���ͽ� �˰��� Atomic���� ����� �ǵ��� �ϱ�
****************************************************/

//volatile int victim;
atomic<int> victim;
//volatile bool p_flag[2] = { false, false };
atomic<bool> p_flag[2] = { false, false };

volatile int sum = 0;

void p_lock(int tid)
{
	int other = 1 - tid;	// ������ ������
	p_flag[tid] = true;
	victim = tid;

	///****************************************************
	// ������ϰ� �ϸ� ����� �� �ȴ� ! ! !
	// atomic_thread_fence(memory_order_seq_cst);
	///****************************************************

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
