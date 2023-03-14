#include "Include.h"

// >> Cache Miss <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//constexpr int CACHE_LINE_SIZE = 32;
//int main()
//{
//	for (int i = 0; i < 20; ++i) {
//		const int size = 1024 << i;
//		long long* a = (long long*)malloc(size);
//		unsigned int index = 0;
//		long long tmp = 0;
//		const int num_data = size / 8;
//		auto start = high_resolution_clock::now();
//		for (int j = 0; j < 100000000; ++j) {
//			tmp += a[index % num_data];
//			index += CACHE_LINE_SIZE * 11;
//		}
//		auto dur = high_resolution_clock::now() - start;
//
//		auto ms = duration_cast<milliseconds>(dur).count();
//		cout << "MEM size = " << size / 1024 << "KB, ";
//		cout << "Exec Time = " << ms << "ms, ";
//		cout << "Result = " << tmp << endl;
//		free(a);
//	}
//}


// >> Pipeline Stall <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// Conditional Branch�� ������ ������ �����ȴ�

constexpr int T_SIZE = 10000000;
short rand_arr[T_SIZE];

#define abs(x) ((x < 0)?(-x):x)
#define abs2(x) ((x>>31)^x - (x>>31))

int main()
{
	int sum;
	for (int i = 0; i < T_SIZE; ++i) rand_arr[i] = rand() - 16384;
	sum = 0;
	auto start_t = high_resolution_clock::now();
	for (int i = 0; i < T_SIZE; ++i) sum += abs(rand_arr[i]);
	auto du = high_resolution_clock::now() - start_t;
	cout << "[abs] Time " << duration_cast<milliseconds>(du).count() << " ms\n";
	cout << "Result : " << sum << endl;
	sum = 0;
	start_t = high_resolution_clock::now();
	for (int i = 0; i < T_SIZE; ++i) sum += abs2(rand_arr[i]);
	du = high_resolution_clock::now() - start_t;
	cout << "[abs2] Time " << duration_cast<milliseconds>(du).count() << " ms\n";
	cout << "Result : " << sum << endl;
}

/*
 * Release ���� �������� ���� ���� �ֵ�.
 * ������ �ϋ��� ���־��� ����ȭ�� �ϴµ�, abs�� ���밪 ����°��ΰ� �˾������� �˾Ƽ� �ع���
 */