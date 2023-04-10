#include <thread>
#include <iostream>
#include <vector>

using namespace std;

// -------------------------------------------------------------------------------------
// ��Ƽ ������ �⺻ ���
// -------------------------------------------------------------------------------------

void out_ThreadID(int th_Id)
{
	//cout << this_thread::get_id() << endl;

	//cout << th_Id << endl;
	// �߰��߰� ���� ���� ���� : th_id ��� �� endl �ϱ� ������ endl ����ϱ� ���� �ٸ����� ����� ����ع���

	char buff[200];
	sprintf_s(buff, "%d\n", th_Id);
	cout << buff;
}

int main()
{
	//thread t1{ out_ThreadID, 0 };		// �Ķ���� �ֱ�
	//t1.join();

	int num_core = thread::hardware_concurrency();		// ���� �ھ��� ����
	vector<thread> workers;

	for (int i = 0; i < num_core; ++i)
		workers.emplace_back(out_ThreadID, i);
	for (auto& th : workers)
		th.join();
}