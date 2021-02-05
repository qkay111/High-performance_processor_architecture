// Main.cpp

#include <cmath>
#include <ctime>
#include <iostream>
#include <random>
#include <Windows.h>

#include <immintrin.h>

#define FLOAT_FOURTH_PTR_MEM_SET(ptr, iVal, jVal, kVal, itVal)\
	for (int i = 0; i < iVal; i++)\
	{\
		for (int j = 0; j < jVal; j++)\
		{\
			for (int k = 0; k < kVal; k++)\
			{\
				for (int it = 0; it < itVal; it++)\
				{\
					ptr[i][j][k][it] = 0.0f;\
				}\
			}\
		}\
	}\

#define FLOAT_FOURTH_PTR_SHOW(ptr, iVal, jVal, kVal, itVal)\
	for (int i = 0; i < iVal; i++)\
	{\
		for (int j = 0; j < jVal; j++)\
		{\
			for (int k = 0; k < kVal; k++)\
			{\
				for (int it = 0; it < itVal; it++)\
					cout << ptr[i][j][k][it] << '\t';\
				cout << endl;\
			}\
			cout << endl << endl;\
		}\
		cout << endl;\
	}

constexpr int BL = 1;
constexpr int BM = 1;
constexpr int BN = 1;

constexpr int SL = 8;
constexpr int SM = 8;
constexpr int SN = 8;

using std::cout;
using std::endl;

template <typename T, int highRang>
inline void dynamicAllocation(void** ptr, int rang, int* len)
{
	if (rang == 0)
	{
		*ptr = malloc(len[rang] * sizeof(T));
		return;
	}
	else
	{
		*ptr = malloc(len[rang] * sizeof(void*));
	}

	for (int i = 0; i < len[rang]; i++)
	{
		dynamicAllocation<T, -1>((void**)((void**)*ptr + i), rang - 1, len);
	}

	if (highRang != -1 && rang == highRang)
	{
		delete[] len;
	}
}

template <int highRang>
inline void dynamicFree(void** ptr, int rang, int* len)
{
	if (rang == 0)
	{
		delete[] *ptr;
		return;
	}

	for (int i = 0; i < len[rang]; i++)
	{
		dynamicFree<-1>((void**)((void**)*ptr + i), rang - 1, len);
	}

	delete[] *ptr;
	*ptr = nullptr;

	if (highRang != -1 && rang == highRang)
	{
		delete[] len;
	}
}

template <typename T, int highRang>
inline void dynamiRandomEachElem(void* ptr, int rang, int* len)
{
	if (rang == 0)
	{
		for (int i = 0; i < len[rang]; i++)
		{
			*((T*)ptr + i) = (T)rand();
		}

		return;
	}

	for (int i = 0; i < len[rang]; i++)
	{
		dynamiRandomEachElem<T, -1>((void**)*((void**)ptr + i), rang - 1, len);
	}

	if (highRang != -1 && rang == highRang)
	{
		delete[] len;
	}
}

#define STATIC_VECTOR
#ifndef STATIC_VECTOR

int main()
{
	float ****firMatrix = nullptr;
	float ****secMatrix = nullptr;
	float ****resMatrix1 = nullptr;
	float ****resMatrix2 = nullptr;

	dynamicAllocation<float, 3>((void**)&firMatrix, 3, new int[4]{ SM, SL, BM, BL });
	dynamicAllocation<float, 3>((void**)&secMatrix, 3, new int[4]{ SN, SM, BN, BM });
	dynamicAllocation<float, 3>((void**)&resMatrix1, 3, new int[4]{ SM, SM, BM, BM });
	dynamicAllocation<float, 3>((void**)&resMatrix2, 3, new int[4]{ SM, SM, BM, BM });

	srand(time(NULL));
	dynamiRandomEachElem<float, 3>(firMatrix, 3, new int[4]{ SM, SL, BM, BL });
	dynamiRandomEachElem<float, 3>(secMatrix, 3, new int[4]{ SN, SM, BN, BM });
	FLOAT_FOURTH_PTR_MEM_SET(resMatrix1, BM, BM, SM, SM);
	FLOAT_FOURTH_PTR_MEM_SET(resMatrix2, BM, BM, SM, SM);

	// First loop
	unsigned long startTime = GetTickCount();
	for (int a = 0; a < BL; a++)
	{
		for (int h = 0; h < BM; h++)
		{
			for (int b = 0; b < BN; b++)
			{
				for (int i = 0; i < SL; i++)
				{
					for (int j = 0; j < SM; j++)
					{
						for (int k = 0; k < SN; k++)
						{
							resMatrix1[a][b][i][k] += firMatrix[a][h][i][j] * secMatrix[h][b][j][k];
						}
					}
				}
			}
		}
	}
	cout << GetTickCount() - startTime << endl;

	// Second loop
	__m256 _rg1, _rg2;

	startTime = GetTickCount();
	for (int a = 0; a < BL; a++)
	{
		for (int h = 0; h < BM; h++)
		{
			for (int b = 0; b < BN; b++)
			{
				for (int i = 0; i < SL; i++)
				{
					for (int j = 0; j < SM; j++)
					{
						_rg1 = _mm256_set1_ps(firMatrix[a][h][i][j]);
						_rg2 = *(__m256*)secMatrix[h][b][j];
						_rg1 = _mm256_mul_ps(_rg1, _rg2);
						_rg2 = *(__m256*)resMatrix2[a][b][i];
						_rg1 = _mm256_add_ps(_rg1, _rg2);
						_mm256_store_ps(resMatrix2[a][b][i], _rg1);
					}
				}
			}
		}
	}
	cout << GetTickCount() - startTime << endl;

	for (int i = 0; i < BM; i++)
	{
		for (int j = 0; j < BM; j++)
		{
			for (int k = 0; k < SM; k++)
			{
				for (int it = 0; it < SM; it++)
				{
					if (resMatrix1[i][j][k][it] != resMatrix2[i][j][k][it])
					{
						cout << resMatrix1[i][j][k][it] << endl << resMatrix2[i][j][k][it] << endl;
						cout << "Matrixes are not equal\n\n\n";
						system("pause");
					}
				}
			}
		}
	}

	FLOAT_FOURTH_PTR_SHOW(resMatrix1, BM, BM, SM, SM);
	cout << "\n\n\n\n\n\n\n\n\n\n";
	FLOAT_FOURTH_PTR_SHOW(resMatrix2, BM, BM, SM, SM);

	dynamicFree<3>((void**)&firMatrix, 3, new int[4]{ NULL, SL, BM, BL });
	dynamicFree<3>((void**)&secMatrix, 3, new int[4]{ NULL, SM, BN, BM });
	dynamicFree<3>((void**)&resMatrix1, 3, new int[4]{ NULL, SM, BM, BM });
	dynamicFree<3>((void**)&resMatrix2, 3, new int[4]{ NULL, SM, BM, BM });

	return 0;
}

#else

float firMatrix[BL][BM][SL][SM];
float secMatrix[BM][BN][SM][SN];
float resMatrix1[BM][BM][SM][SM] = {};
float resMatrix2[BM][BM][SM][SM] = {};

int main()
{
	srand(time(NULL));

	dynamiRandomEachElem<float, 0>(firMatrix, 0, new int[1]{ SM * SL * BM * BL });
	dynamiRandomEachElem<float, 0>(secMatrix, 0, new int[1]{ SM * SL * BM * BL });

	unsigned long startTime = GetTickCount();
	for (int a = 0; a < BL; a++)
	{
		for (int h = 0; h < BM; h++)
		{
			for (int b = 0; b < BN; b++)
			{
				for (int i = 0; i < SL; i++)
				{
					for (int j = 0; j < SM; j++)
					{
						for (int k = 0; k < SN; k++)
						{
							resMatrix1[a][b][i][k] += firMatrix[a][h][i][j] * secMatrix[h][b][j][k];
						}
					}
				}
			}
		}
	}
	cout << GetTickCount() - startTime << endl;

	__m256 _rg1, _rg2;

	startTime = GetTickCount();
	for (int a = 0; a < BL; a++)
	{
		for (int h = 0; h < BM; h++)
		{
			for (int b = 0; b < BN; b++)
			{
				for (int i = 0; i < SL; i++)
				{
					for (int j = 0; j < SM; j++)
					{
						_rg1 = _mm256_set1_ps(firMatrix[a][h][i][j]);
						_rg2 = *(__m256*)((float*)secMatrix + h * (BN * SM * SN) + b * (SM * SN) + j * SM);
						_rg1 = _mm256_mul_ps(_rg1, _rg2);
						_rg2 = *(__m256*)((float*)resMatrix2 + a * (BM * SM * SM) + b * (SM * SM) + i * SM);
						_rg1 = _mm256_add_ps(_rg1, _rg2);
						_mm256_store_ps((float*)resMatrix2 + a * (BM * SM * SM) + b * (SM * SM) + i * SM, _rg1);
					}
				}
			}
		}
	}
	cout << GetTickCount() - startTime << endl;


	for (int i = 0; i < BM; i++)
	{
		for (int j = 0; j < BM; j++)
		{
			for (int k = 0; k < SM; k++)
			{
				for (int it = 0; it < SM; it++)
				{
					if (resMatrix1[i][j][k][it] != resMatrix2[i][j][k][it])
					{
						cout << resMatrix1[i][j][k][it] << endl << resMatrix2[i][j][k][it] << endl;
						cout << "Matrixes are not equal\n\n\n";
						system("pause");
					}
				}
			}
		}
	}

	FLOAT_FOURTH_PTR_SHOW(resMatrix1, BM, BM, SM, SM);
	cout << "\n\n\n\n\n\n\n\n\n\n";
	FLOAT_FOURTH_PTR_SHOW(resMatrix2, BM, BM, SM, SM);

	return 0;
}

#endif
