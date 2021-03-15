// Main.cpp

#include <ctime>
#include <iostream>
#include <random>
#include <windows.h>

using std::cout;
using std::endl;

constexpr int N = 20;
constexpr int OFFSET_L1 = 4 * 1024;
constexpr int OFFSET_L2 = 64 * 1024;
constexpr int OFFSET_L3 = 512 * 1024;

typedef union
{
	int value;
	char bytes[64];
} element;

void init(element * const &data, int level, int offset)
{
	for (int i = 0; i < level * offset; i++)
	{
		data[i].value = i;
	}

	std::shuffle(data, data + level * offset + 1, std::default_random_engine(time(NULL)));
}

//void init(element * const &data, int level, int offset)
//{
//	/*for (int i = 0; i < level * offset - 1; i++)
//	{
//		data[i].value = i + 1;
//	}*/
//	
//	for (int i = 0; i < level; i++)
//	{
//		for (int j = 0; j < offset; j++)
//		{
//			if (i + 1 != level)
//			{
//				data[i * offset + j].value = (i + 1) * offset + j;
//			}
//			else
//			{
//				data[i * offset + j].value = j + 1;
//			}
//		}
//	}
//
//	data[level * offset - 1].value = 0;
//
//	//std::shuffle(data, data + level * offset + 1, std::default_random_engine(time(NULL)));
//}

element data[N * OFFSET_L3];

int main()
{
	srand(time(NULL));

	for (int cache = 1; cache <= 3; cache++)
	{
		cout << "L" << cache << " cache:\n";

		int offset;
		cache == 1 ? offset = OFFSET_L1 : (cache == 2 ? offset = OFFSET_L2 : offset = OFFSET_L3);
		for (int i = 2; i < N; i++)
		{
			init(data, i, offset);
			//element t = {};
			element t = { rand() % (i * offset - 2) + 1 };

			auto time = GetTickCount();
			for (unsigned long long j = 0; j < 70000000; j++)
			{
				t = data[t.value];
			}
			cout << i << "\tways?:\t" << GetTickCount() - time << endl;
		}

		cout << endl;
	}

	return 0;
}
