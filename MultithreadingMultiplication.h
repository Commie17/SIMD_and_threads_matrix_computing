#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <cstdlib>
#include <immintrin.h>
#include <thread>
#include <vector>

using namespace std;

void ComputeMatrixNoSIMD(int begining, int endPoint, float** a, float** bT, float** resultMatrix, int matrixSize);
void ComputeMatrixSIMD(int begining, int endPoint, float** a, float** bT, float* resultMatrix[], int matrixSize);

inline void MltpMtxNoSIMDMulthread(float** a, float** b, float** resultMatrix, int matrixSize,int threadsNumber)
{
	vector<int> borders;
	vector<thread> threads;
	float** bT = new float* [matrixSize];
	double average = 0;

	//транспонируем матрицу
	for (int i = 0;i < matrixSize;i++)
	{
		bT[i] = new float[matrixSize];
		for (int j = 0;j < matrixSize;j++)
		{
			bT[i][j] = b[j][i];
		}
	}

	//разделяем строки по потокам
	for (int i = 0;i < threadsNumber;i++)
	{
		borders.push_back((i+1) * (matrixSize / threadsNumber));
	}

	std::cout << "Matrix multiplication, No SIMD, Multithreading\n";
	//раскидываем функцию без SIMD по потокам
	for (int iterator = 0;iterator < 1;iterator++)
	{
		for (int k = 0;k < threadsNumber;k++)
		{
			if (k == 0)
				threads.push_back(std::thread(ComputeMatrixNoSIMD, 0,borders[k], a, bT, resultMatrix, matrixSize));

			else if(k==threadsNumber-1)
				threads.push_back(std::thread(ComputeMatrixNoSIMD, borders[k-1]+1, borders[k]+ matrixSize % threadsNumber, a, bT, resultMatrix, matrixSize));
			else
				threads.push_back(std::thread(ComputeMatrixNoSIMD, borders[k - 1] + 1, borders[k], a, bT, resultMatrix, matrixSize));
		}

		std::cout << "Iteration number: " << iterator << endl;
		auto start = chrono::high_resolution_clock::now();
		for (int i = 0;i < threadsNumber;i++)
		{
			threads[i].join();
		}
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> dur = (end - start);

		std::cout << "Iteration: " << iterator << " time: " << dur.count()<<endl;
		average += dur.count() / 10;
		threads.clear();
	}
	std::cout << "Average: " << average << endl;
	std::cout << "------------Complete------------\n";
	average = 0;
};

//функция для умножения матриц на множестве потоков и с использованием SIMD инструкций
void MltpMtxSIMDMulthread(float** a, float** b, float* resultMatrix[], int matrixSize, int threadsNumber)
{
	vector<int> borders;
	vector<thread> threads;
	float** bT = new float* [matrixSize];
	double average = 0;

	//транспонируем матрицу
	for (int i = 0;i < matrixSize;i++)
	{
		bT[i] = new float[matrixSize];
		for (int j = 0;j < matrixSize;j++)
		{
			bT[i][j] = b[j][i];
		}
	}

	//разделяем строки по потокам
	for (int i = 0;i < threadsNumber;i++)
	{
		borders.push_back((i + 1) * (matrixSize / threadsNumber));
	}

	std::cout << "Matrix multiplication, SIMD, Multithreading\n";
	//раскидываем функцию с SIMD по потокам
	for (int iterator = 0;iterator < 1;iterator++)
	{
		for (int k = 0;k < threadsNumber;k++)
		{
			if (k == 0)
				threads.push_back(std::thread(ComputeMatrixSIMD, 0, borders[k], a, bT, resultMatrix, matrixSize));

			else if (k == threadsNumber - 1)
				threads.push_back(std::thread(ComputeMatrixSIMD, borders[k - 1] + 1, borders[k] + matrixSize % threadsNumber, a, bT, resultMatrix, matrixSize));
			else
				threads.push_back(std::thread(ComputeMatrixSIMD, borders[k - 1] + 1, borders[k], a, bT, resultMatrix, matrixSize));
		}

		std::cout << "Iteration number: " << iterator << endl;
		auto start = chrono::high_resolution_clock::now();
		for (int i = 0;i < threadsNumber;i++)
		{
			threads[i].join();
		}
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> dur = (end - start);

		std::cout << "Iteration: " << iterator << " time: " << dur.count() << endl;
		average += dur.count() / 10;
		threads.clear();
	}
	std::cout << "Average: " << average << endl;
	std::cout << "------------Complete------------\n";
	average = 0;
};

void ComputeMatrixNoSIMD(int begining, int endPoint, float** a, float** bT, float** resultMatrix, int matrixSize)
{
	for (int i = begining;i < endPoint;i++)
	{
		for (int j = 0;j < matrixSize;j++)
		{
			resultMatrix[i][j] = 0;
			for (int k = 0;k < matrixSize;k++)
			{
				resultMatrix[i][j] += a[i][k] * bT[j][k];
			}
		}
	}
};

void ComputeMatrixSIMD(int begining, int endPoint, float** a, float** bT, float* resultMatrix[], int matrixSize)
{
	__m128 aVector;
	__m128 bVector;
	__m128 sumVector;
	__m128 zeroVector=_mm_setzero_ps();
	float buffer=0;
	for (int i = begining;i < endPoint;i++)
	{
		for (int j = 0;j < matrixSize;j++)
		{
			resultMatrix[i][j] = 0;
			for (int k = 0;k < matrixSize;k+=4)
			{
				aVector = _mm_load_ps(&a[i][k]);
				bVector = _mm_load_ps(&bT[j][k]);
				sumVector = _mm_mul_ps(aVector, bVector);
				sumVector = _mm_hadd_ps(sumVector, zeroVector);
				sumVector = _mm_hadd_ps(sumVector, zeroVector);
				_MM_EXTRACT_FLOAT(buffer, sumVector, 0);
				resultMatrix[i][j] += buffer;
			}
		}
	}
};