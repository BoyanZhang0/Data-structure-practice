#pragma once
class buffer
{
public:
	buffer(int num)
	{
		buffer::num = num;
		buffer::temp = new int[num];
		buffer::col = new int[num];
		for (int i = 0; i < num; i++)
		{
			temp[i] = 0;
			col[i] = 0;
		}
	}
	~buffer()
	{
		delete[]temp;
		delete[]col;
	}
	int num = 0;
	int* temp;
	int* col;
};