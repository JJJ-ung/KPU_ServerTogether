#include <thread>
#include <iostream>
#include <mutex>
using namespace std;


volatile int victim;
volatile bool p_flag[2] = {false, false};
volatile int sum = 0;

void p_lock(int tid)
{
	int other = 1 - tid;
	p_flag[tid] = true;
	victim = tid;
	atomic_thread_fence(memory_order_seq_cst);
	while ((p_flag[other] == true) && (victim == tid));
}

void p_unlock(int tid)
{
	p_flag[tid] = false;
}

void workder(int tid)
{
	for (int i = 0; i < 25000000; ++i)
	{
		p_lock(tid);
		sum = sum + 2;
		p_unlock(tid);

	}
}
void sender()
{

}

int main()
{
	thread r{ workder,0	};
	thread s{ workder,1 };
	r.join();
	s.join();
	cout << " SUM : " << sum << endl;
}