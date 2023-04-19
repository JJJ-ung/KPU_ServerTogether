#include <iostream>
#include <thread>

using namespace std;

volatile int* bound;
volatile bool done_flag;

int g_error;

// Cache Line 짤리는거 실습

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
			g_error++;
	}
}


int main()
{
	//bound = new int{ 0 };

	int mem[32];
	long long addr = reinterpret_cast<long long>(&mem[31]);
	addr = (addr / 64) * 64;
	addr = addr - 2;
	bound = reinterpret_cast<int*>(addr);
	*bound = 0;

	thread t1{ th1 };
	thread t2{ th2 };

	t1.join();
	t2.join();

	cout << "Error = " << g_error << endl;
}