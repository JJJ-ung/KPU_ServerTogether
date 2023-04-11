#include <thread>
#include <iostream>
#include <vector>
#include <chrono>

#include <mutex>

using namespace std;
using namespace chrono;

mutex gl;

constexpr int MAX_THREADS = 16;

volatile int sum = 0;

//volatile int t_sum[MAX_THREADS];
//volatile int t_sum[MAX_THREADS * 16];	// ������ �ϳ����� �ٸ� ĳ�� ������ ������ ����

struct t
{
	volatile alignas(64) int num;
};
t t_sum[MAX_THREADS];

void worker(int num_th, int th_id)
{
	// 1. loop�ȿ� lock �ɱ�
	/*
	for (int i = 0; i < 50000000 / num_th; ++i)
	{
		gl.lock();
		sum += 2;
		gl.unlock();
	}
	 */

	 // -------------------------------------------------------------------------------------

	// 2. loop�ۿ� lock �ɱ�
	// �ٵ� ��Ƽ������� ������ ������ ���� -> �ϳ��ϳ� ���ʴ�� �����尡 ó�� �Ǵϱ�
	/*
	gl.lock();
	for (int i = 0; i < 50000000 / num_th; ++i)
	{
		sum += 2;
	}
	gl.unlock();
	 */

	 // -------------------------------------------------------------------------------------

	// 3. data race ���� lock �ɱ�
	/*
	volatile int local_sum = 0;
	for (int i = 0; i < 50000000 / num_th; ++i)
		local_sum = local_sum + 2;
	gl.lock();
	sum = sum + local_sum;
	gl.unlock();
	 */

	// -------------------------------------------------------------------------------------

	// 4. mutex ������� �ʰ� array�� ���갪 �־�ΰ� �������� ��ġ��
	/*
	for (int i = 0; i < 50000000 / num_th; ++i)
		t_sum[th_id] = t_sum[th_id] + 2;
	 */

	 // -------------------------------------------------------------------------------------

	 // 5. �����帶�� �ٸ� ĳ�ö����� ���� ���� : �ӵ��� �������µ� �޸� ���� ������
	/*
	for (int i = 0; i < 50000000 / num_th; ++i)
		t_sum[th_id * 16] = t_sum[th_id * 16] + 2;
	 */

	// -------------------------------------------------------------------------------------

	// 6. alignas
	for (int i = 0; i < 50000000 / num_th; ++i)
		t_sum[th_id].num = t_sum[th_id].num + 2;

}

int main()
{
	for (int num_core = 1; num_core <= MAX_THREADS; num_core *= 2)
	{
		sum = 0;
		for (auto& v : t_sum) v.num = 0;
		auto start_time = high_resolution_clock::now();

		vector<thread> workers;
		for (int i = 0; i < num_core; ++i)
			workers.emplace_back(worker, num_core, i);
		for (auto& th : workers)
			th.join();
		for (auto v : t_sum)
			sum = sum + v.num;

		auto end_time = high_resolution_clock::now();

		auto exec_time = end_time - start_time;
		auto exec_ms = duration_cast<milliseconds>(exec_time).count();

		cout << num_core << "Threads, Sum = " << sum << ", Exec Time = " << exec_ms << "ms" << endl;
	}
}
