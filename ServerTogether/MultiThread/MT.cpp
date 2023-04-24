#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <atomic>
using namespace std;
using namespace chrono;

constexpr int MAX_THREADS = 16;

volatile int sum = 0;
int num_core;
struct tt
{
	volatile alignas(64) int num;
};
volatile int t_sum;
mutex gl;
atomic_int ml;
void caslock()
{
	int lock_value = 0;
	while (false == atomic_compare_exchange_strong(&ml, &lock_value, 1)) lock_value = 0;
}
void casunlock()
{
	ml = 0;
}

void out_thid(int num_thread,int th_id)
{

	for (int i = 0; i < 50000000/ num_thread; ++i)
	{
		gl.lock();
		//caslock();
		t_sum += 2;
		//casunlock();
		gl.unlock();
	}
}
int main()
{
	for (num_core = 1; num_core <= 16; num_core *= 2)
	{
		t_sum = 0;
		auto start_t = high_resolution_clock::now();

		vector<thread> workers;
		for (int i = 0; i < num_core; ++i)
		{
			workers.emplace_back(out_thid, num_core,i);
		}

		for (auto& th : workers)
		{
			th.join();
		}

		auto end_t = high_resolution_clock::now();
		auto exec_t = end_t - start_t;
		auto exec_ms = duration_cast<milliseconds>(exec_t).count();
		cout << num_core <<"threads, Sum = " << t_sum << " exec time = " << exec_ms << endl;
	}
}
