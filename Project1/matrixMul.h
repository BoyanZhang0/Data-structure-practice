#pragma once
#ifndef MATRIXMUL_H
#define MATRIXMUL_H

#include<string>
using namespace std;

class my_Matrix
{
public:
	my_Matrix() {};
	my_Matrix(char s[3], int r, int c, int o)//初始化函数
	{
		for (int i = 0; i < 3; i++)
		{
			name[i] = s[i];
		}
		row = r;
		col = c;
		offset = o;
		
	}

	char name[4] = {'M', 'A', 'T', '\0'};
	int row = 0, col = 0, offset = 0;//行数，列数，偏移量
};

#endif