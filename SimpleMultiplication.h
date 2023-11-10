#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cstdlib>
#include <immintrin.h>

using namespace std;

//умножение в одном потоке без SIMD инструкций
void MltpMtxNoSIMDNoMulthread(float** a, float** b, int matrixSize, float* resultMatrix[])
{
	double average = 0;
	cout << "Matrix multiplication, No SIMD, No Multithreading\n";
	for (int iterator = 0;iterator < 1;iterator++)
	{
		cout << "Iteration number: " << iterator << endl;
		auto start = chrono::high_resolution_clock::now();
		for (int i = 0;i < matrixSize;i++)
		{
			for (int j = 0;j < matrixSize;j++)
			{
				resultMatrix[i][j] = 0;
				for (int k = 0;k < matrixSize;k++)
				{
					resultMatrix[i][j] += a[i][k] * b[k][j];
				}
				cout.precision(10);
				cout << resultMatrix[i][j]<<" ";
			}
			cout << '\n';
		}
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> dur = (end - start);
		cout << "Single thread, matrix multiplication time " << dur.count() << endl;
		average += dur.count() / 10;
		if (iterator == 9)
			cout << "average: " << average << endl;
	}
	cout << "----------Complete---------\n";
};
//умножение в одном потоке и с использованием SIMD инструкций
void MltpMtxSIMDNoMulthread(float** a, float** b, int matrixSize, float* resultMatrix[])
{
	__m128 aNumbers;
	__m128 bNumbers;
	__m128 sum;
	__m128 zeroVector = _mm_setzero_ps();
	float buffer;
	double average = 0;

	float** bT = new float* [matrixSize];
	for (int i = 0;i < matrixSize;i++)
	{
		bT[i] = new float[matrixSize];
		for (int j = 0;j < matrixSize;j++)
		{
			bT[i][j] = b[j][i];
		}
	}

	cout << "Matrix multiplication, SIMD, No Multithreading\n";
	for (int iterator = 0;iterator < 1;iterator++)
	{
		cout << "Iteration number: " << iterator << endl;
		auto start = chrono::high_resolution_clock::now();
		for (int i = 0;i < matrixSize;i++)
		{
			for (int j = 0;j < matrixSize;j++)
			{
				resultMatrix[i][j] = 0;
				for (int k = 0;k < matrixSize;k += 4)
				{
					aNumbers = _mm_load_ps(&a[i][k]);
					bNumbers = _mm_load_ps(&bT[j][k]);
					sum = _mm_mul_ps(aNumbers, bNumbers);
					sum = _mm_hadd_ps(sum, zeroVector);
					sum = _mm_hadd_ps(sum, zeroVector);
					_MM_EXTRACT_FLOAT(buffer, sum, 0);
					resultMatrix[i][j] += buffer;
				}
				cout.precision(10);
				cout << resultMatrix[i][j] << " ";
			}
			cout << endl;
		}
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> dur = (end - start);
		cout << "Single thread, matrix multiplication time " << dur.count() << endl;
		average += dur.count() / 10;
		if (iterator == 9)
			cout << "average: " << average << endl;
	}
	cout << "----------Complete---------\n";
};