#include <thread>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

// -------------------------------------------------------------------------------------
// 성능 측정하기
// -------------------------------------------------------------------------------------


/*
int main()
{
	int sum = 0;

	auto start_time = high_resolution_clock::now();
	for (int i = 0; i < 50000000; ++i)
		sum = sum + 2;
	// release에서 0ms 나오는 이유 : 컴파일러가 최적화로 위에꺼 씹고 sum = 100000000 으로 바꿔버려서
	// 중단점 걸고 디스어셈블리 (alt+F8) 해보면 알 수 있음
	auto end_time = high_resolution_clock::now();

	auto exec_time = end_time - start_time;
	auto exec_ms = duration_cast<milliseconds>(exec_time).count();

	cout << "Sum = " << sum << ", Exec Time = " << exec_ms << "ms" << endl;
}
*/


/*
volatile int sum;
// 컴파일러가 최적화 못하게 하는 키워드

int main()
{
	auto start_time = high_resolution_clock::now();
	for (int i = 0; i < 50000000; ++i)
		sum += 2;
	auto end_time = high_resolution_clock::now();

	auto exec_time = end_time - start_time;
	auto exec_ms = duration_cast<milliseconds>(exec_time).count();

	cout << "Sum = " << sum << ", Exec Time = " << exec_ms << "ms" << endl;
}
*/

/*
// 멀티 쓰레드로 해보자

volatile int sum = 0;

void worker()
{
	for (int i = 0; i < 50000000 / 2; ++i)
		sum += 2;
}

int main()
{
	auto start_time = high_resolution_clock::now();

	int num_core = 2;		// 현재 코어의 갯수
	vector<thread> workers;

	for (int i = 0; i < num_core; ++i)
		workers.emplace_back(worker);
	for (auto& th : workers)
		th.join();

	auto end_time = high_resolution_clock::now();

	auto exec_time = end_time - start_time;
	auto exec_ms = duration_cast<milliseconds>(exec_time).count();

	cout << "Sum = " << sum << ", Exec Time = " << exec_ms << "ms" << endl;
}

// 쓰레드 2개 : 시간도 늘어나고 결과도 이상해진다.
 */


// 쓰레드 갯수를 바꿔가면서 해보자

volatile int sum = 0;

void worker(int num_th)
{
	// 목표값을 쓰레드 갯수만큼 나눠서 분담하기
	for (int i = 0; i < 50000000 / num_th; ++i)
		sum += 2;
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
