#pragma once

#include<vector>

using namespace std;
class buffer
{
public:
	buffer(int num)
	{
		input1 = new int[num];
		input2 = new int[num];
		output = new int[num];
		for (int i = 0; i < num; i++)
		{
			input1[i] = 0;
			input2[i] = 0;
			output[i] = 0;
		}
	}
	~buffer()
	{
		delete[]input1;
		delete[]input2;
		delete[]output;
	}
	int* input1;
	int* input2;
	int* output;
};