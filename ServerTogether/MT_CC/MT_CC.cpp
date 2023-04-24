#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
using namespace std;

volatile int* bound;
volatile bool done_flag;

int g_error;

atomic<int> sum = 0;
#define MAX_THREADS 64
mutex sum_lock;
int num_threads;

void ThreadFunc()
{
	for (int i = 0; i < 50000000/num_threads; ++i)
	{
		sum += 2;
	}
}



void th1()
{
	for (int i = 0; i < 50000000; ++i)
	{
		*bound = -(1 + *bound);
	}
	done_flag = true;
}
void th2()
{
	while (false == done_flag)
	{
		int v = *bound;
		if ((v != 0) && (v != -1))
		{
			g_error++;
			printf("%x ", v);
		}
	}
}

int main()
{
	vector<thread> my_thread;

	
	for (num_threads = 1; num_threads <= MAX_THREADS; num_threads *= 2)
	{
		sum = 0;
		my_thread.clear();
		for (int i = 0; i < num_threads; ++i)
		{
			my_thread.emplace_back(ThreadFunc);
		}
		for (auto& th : my_thread)
			th.join();

		cout << "Number Of Thread :" << num_threads << ", Result is " << sum << std::endl;
	}

	//bound = new int{ 0 };
	//int mem[32];
	//long long addr = reinterpret_cast<long long>(&mem[31]);
	//addr = addr / 64 * 64;
	//addr = addr - 2;
	//bound = reinterpret_cast<int*>(addr);
	//*bound = 0;
	//thread t1{ th1 };
	//thread t2{ th2 };
	//t1.join();
	//t2.join();
	//cout << "ERROR = " << g_error << std::endl;

}