#include <thread>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

// -------------------------------------------------------------------------------------
// ���� �����ϱ�
// -------------------------------------------------------------------------------------


/*
int main()
{
	int sum = 0;

	auto start_time = high_resolution_clock::now();
	for (int i = 0; i < 50000000; ++i)
		sum = sum + 2;
	// release���� 0ms ������ ���� : �����Ϸ��� ����ȭ�� ������ �ð� sum = 100000000 ���� �ٲ������
	// �ߴ��� �ɰ� �𽺾���� (alt+F8) �غ��� �� �� ����
	auto end_time = high_resolution_clock::now();

	auto exec_time = end_time - start_time;
	auto exec_ms = duration_cast<milliseconds>(exec_time).count();

	cout << "Sum = " << sum << ", Exec Time = " << exec_ms << "ms" << endl;
}
*/


/*
volatile int sum;
// �����Ϸ��� ����ȭ ���ϰ� �ϴ� Ű����

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
// ��Ƽ ������� �غ���

volatile int sum = 0;

void worker()
{
	for (int i = 0; i < 50000000 / 2; ++i)
		sum += 2;
}

int main()
{
	auto start_time = high_resolution_clock::now();

	int num_core = 2;		// ���� �ھ��� ����
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

// ������ 2�� : �ð��� �þ�� ����� �̻�������.
 */


// ������ ������ �ٲ㰡�鼭 �غ���

volatile int sum = 0;

void worker(int num_th)
{
	// ��ǥ���� ������ ������ŭ ������ �д��ϱ�
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
