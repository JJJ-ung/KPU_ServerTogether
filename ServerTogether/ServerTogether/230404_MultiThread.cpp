#include <thread>
#include <iostream>
#include <vector>

using namespace std;

// -------------------------------------------------------------------------------------
// 멀티 쓰레드 기본 사용
// -------------------------------------------------------------------------------------

void out_ThreadID(int th_Id)
{
	//cout << this_thread::get_id() << endl;

	//cout << th_Id << endl;
	// 중간중간 빵꾸 나는 이유 : th_id 출력 후 endl 하기 때문에 endl 출력하기 전에 다른놈이 끼어들어서 출력해버림

	char buff[200];
	sprintf_s(buff, "%d\n", th_Id);
	cout << buff;
}

int main()
{
	//thread t1{ out_ThreadID, 0 };		// 파라미터 넣기
	//t1.join();

	int num_core = thread::hardware_concurrency();		// 현재 코어의 갯수
	vector<thread> workers;

	for (int i = 0; i < num_core; ++i)
		workers.emplace_back(out_ThreadID, i);
	for (auto& th : workers)
		th.join();
}