#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace chrono;

// Cache Miss
// 
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
//		cout << "Exec time = " << ms << "ms, ";
//		cout << "Result = " << tmp << std::endl;
//		free(a);
//	}
//}

//// 분기없이 하는 것이 훨씬 성능이 좋다 abs보다 abs2가 더 빠름
//#define abs(x) (((x>0)?(x):(-x)))
//#define abs2(x) (((x>>31)^x) - (x>>31)) // C 표준으로 쉬프트 하면 제일 앞쪽은 첫번째 값이 복사된다.
//constexpr int T_SIZE = 100000000;
//short rand_arr[T_SIZE];
//int main()
//{
//	int sum;
//	for (int i = 0; i < T_SIZE; ++i) rand_arr[i] = rand() - 16384;
//	sum = 0;
//	auto start_t = high_resolution_clock::now();
//	for (int i = 0; i < T_SIZE; ++i) sum += abs(rand_arr[i]);
//	auto du = high_resolution_clock::now() - start_t;
//	cout << "[abs] Time " << duration_cast<milliseconds>(du).count() << " ms\n";
//	cout << "Result : " << sum << endl;
//	sum = 0;
//	start_t = high_resolution_clock::now();
//	for (int i = 0; i < T_SIZE; ++i) sum += abs2(rand_arr[i]);
//	du = high_resolution_clock::now() - start_t;
//	cout << "[abs2] Time " << duration_cast<milliseconds>(du).count() << " ms\n";
//	cout << "Result : " << sum << endl;
//}4
