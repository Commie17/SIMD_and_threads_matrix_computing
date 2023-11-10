#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <ctime>
#include <cstdlib>
#include "SimpleMultiplication.h"
#include "Comparator.h"
#include "MultithreadingMultiplication.h"

using namespace std;

void ComputeInThread(uint64_t startNumber, uint64_t endPoint, uint64_t& finalSum);

int main()
{
	int coreNumber = _Thrd_hardware_concurrency();
	int matrixSize = 4;
	uint64_t n = 1e3;
	uint64_t finalSum = 0;

	vector<std::thread> threads;
	vector<uint64_t> endPoints;

	double average=0;
	float** aMatrix=new float* [matrixSize];
	float** bMatrix = new float* [matrixSize];
	float** RsltNoSIMDNoMlthrd = new float* [matrixSize];
	float** RsltNoSIMDMlthrd = new float* [matrixSize];
	float** RsltSIMDNoMlthrd = new float* [matrixSize];
	float** RsltSIMDMlthrd = new float* [matrixSize];

	//SimpleMultiplicator simpleMultiplicator;
	Comparator comparator;

	//разбиваем на равные промежутки и фиксируем конец каждого промежутка
	uint64_t range = n / coreNumber;
	for (int k = 0;k < coreNumber;k++)
	{
		endPoints.push_back(range * (k + 1));
	}

	//вносим коррективы если есть остаток от деления
	int remainder = n % coreNumber;
	endPoints[endPoints.size() - 1] += remainder;

	//рассчитываем без мультипоточности
	std::cout << "Single thread: "<<endl;
	average = 0;
	for (int i=0;i<10;i++)
	{
		finalSum = 0;

		auto start = chrono::high_resolution_clock::now();
		ComputeInThread(0,n,finalSum);
		auto end = chrono::high_resolution_clock::now();

		chrono::duration<double, std::milli> dur = (end - start);
		average += dur.count();
		std::cout << finalSum << " time: " << dur.count() << endl;
	}

	//выводим среднее время работы и обнуляем переменные для рассчётов с мультипоточностью
	std::cout << "Average: " << average / 10<<endl;
	finalSum = 0;
	average = 0;
	std::cout << "Multithreading: "<<endl;

	//считаем с использованием мультипоточности
	for (int m = 0;m < 10;m++)
	{
		//вренм считать отсюда
		for (int k = 0;k < coreNumber;k++)
		{
			if (k == 0)
				threads.push_back(std::thread(ComputeInThread, 1, endPoints[k], ref(finalSum)));

			else
				threads.push_back(std::thread(ComputeInThread, endPoints[k - 1] + 1, endPoints[k], ref(finalSum)));
		}

		auto start = chrono::high_resolution_clock::now();
		for (int i = 0;i < coreNumber;i++)
		{
			threads[i].join();
		}
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double, std::milli> dur = (end - start);

		std::cout << finalSum << " time: " << dur.count()<<endl;
		average += dur.count();
		finalSum = 0;
		threads.clear();
	}

	std::cout <<"Average: "<< average / 10<<endl;
	
	srand(time(NULL));
	for (int k=0;k< matrixSize;k++)
	{
		//выделяем память под матрицы для результатов и под изначальные матрицы
		aMatrix[k] = new float[matrixSize];
		bMatrix[k] = new float[matrixSize];
		RsltNoSIMDMlthrd[k]= new float[matrixSize];
		RsltSIMDMlthrd[k] = new float[matrixSize];
		RsltSIMDNoMlthrd[k]= new float[matrixSize];
		RsltNoSIMDNoMlthrd[k]= new float[matrixSize];

		for (int i = 0;i < matrixSize;i++)
		{
			RsltNoSIMDMlthrd[k][i] = 0;
			RsltSIMDMlthrd[k][i] = 0;
			RsltSIMDNoMlthrd[k][i] = 0;
			RsltNoSIMDNoMlthrd[k][i] = 0;
			aMatrix[k][i]= 1.0f+rand()/100000.0f;
			bMatrix[k][i] = 1.0f + rand() / 100000.0f;
		}
	}

	//здесь выполняется вторая часть лабораторной по расчёту произведения матриц
	MltpMtxNoSIMDNoMulthread(aMatrix,bMatrix,matrixSize,RsltNoSIMDNoMlthrd);
	MltpMtxSIMDNoMulthread(aMatrix, bMatrix, matrixSize, RsltSIMDNoMlthrd);
	comparator.Compare(RsltNoSIMDNoMlthrd, RsltSIMDNoMlthrd,matrixSize);
	MltpMtxNoSIMDMulthread(aMatrix, bMatrix, RsltNoSIMDMlthrd, matrixSize, coreNumber);
	comparator.Compare(RsltNoSIMDNoMlthrd,RsltNoSIMDMlthrd,matrixSize);
	MltpMtxSIMDMulthread(aMatrix, bMatrix, RsltSIMDMlthrd, matrixSize, coreNumber);
	comparator.Compare(RsltSIMDMlthrd, RsltNoSIMDNoMlthrd, matrixSize);

	for (int i = 0;i < matrixSize;i++)
	{
		delete[] aMatrix[i];
		delete[] bMatrix[i];
		delete[] RsltNoSIMDMlthrd[i];
		delete[] RsltSIMDMlthrd[i];
		delete[] RsltSIMDNoMlthrd[i];
		delete[] RsltNoSIMDNoMlthrd[i];
	}
	delete[] aMatrix;
	delete[] bMatrix;
	delete[] RsltNoSIMDMlthrd;
	delete[] RsltSIMDMlthrd;
	delete[] RsltSIMDNoMlthrd;
	delete[] RsltNoSIMDNoMlthrd;
}

//функция для вычисления суммы ряда
void ComputeInThread(uint64_t startNumber, uint64_t endPoint, uint64_t& finalSum)
{
	uint64_t sum = startNumber;

	for (uint64_t i = startNumber + 1;i <= endPoint;i++)
	{
		sum += i;
	}

	finalSum += sum;
}