#include "Include.h"

int sum = 0;

atomic_int ml = 0; // my lock

void casLock()
{
	int lock_value = 0;
	while (false == atomic_compare_exchange_strong(&ml, &lock_value, 1))
		lock_value = 0;	// ml을 0에서 1로 바꾸기
}

void casUnlock()
{
	ml = 0;
}

void worker(int num_th)
{
	// 목표값을 쓰레드 갯수만큼 나눠서 분담하기
	for (int i = 0; i < 50000000 / num_th; ++i)
	{
		casLock();
		sum += 2;
		casUnlock();
	}
}

int main()
{
	for (int num_core = 1; num_core <= 16; num_core *= 2)
	{
		sum = 0;
		auto start_time = high_resolution_clock::now();

		vector<thread> workers;
		for (int i = 0; i < num_core; ++i)
			workers.emplace_back(worker, num_core);
		for (auto& th : workers)
			th.join();

		auto end_time = high_resolution_clock::now();

		auto exec_time = end_time - start_time;
		auto exec_ms = duration_cast<milliseconds>(exec_time).count();

		cout << num_core << "Threads, Sum = " << sum << ", Exec Time = " << exec_ms << "ms" << endl;
	}
}
