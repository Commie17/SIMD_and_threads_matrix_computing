#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

class Comparator
{
public:
	void Compare(float** firstMatrix,float** secondMatrix,int matrixSize)
	{
		bool isEqual = true;
		for (int i=0;i<matrixSize;i++)
		{
			for (int j=0;j<matrixSize;j++)
			{
				if (round(firstMatrix[i][j] * 10) / 10 != round(secondMatrix[i][j] * 10) / 10)
				{
					cout << firstMatrix[i][j] << "  " << secondMatrix[i][j]<<endl;
					isEqual = false;
					break;
				}
			}
			if (!isEqual)break;
		}
		if (isEqual)
			cout << "All right\n";
		else
			cout << "Something wrong\n";
	}
};