/*
文件格式采用以下存储方式：
前4个字节存储一个int类型代表所存储的矩阵个数n
之后n个矩阵都分别以（名称，行数，列数，偏移量）的形式进行信息存储，其中名称以char[4]类型进行存储，行数、列数、偏移量以int类型进行存储。
最后矩阵依次存储
*/
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<matrixMul.h>
#include<fileDeal.h>
#include<buffer.h>
#include<time.h>
#include<windows.h>

using namespace std;

int main()
{
	
	LARGE_INTEGER t1, t2, tc;
	QueryPerformanceFrequency(&tc);
	QueryPerformanceCounter(&t1);
	
	ifstream ifs;//读取计算矩阵文件
	ifs.open("matrix.txt", ios::in | ios::binary);
	if (!ifs.is_open())
	{
		cout << "reading file failed" << endl;
		return 0;
	}

	//读取结果矩阵文件
	fstream ofs;
	ofs.open("result.txt", ios::binary | ios::out | ios::in);

	int num = 0, w = 0;//矩阵个数,cache的大小

	ifs.read((char*)&num, sizeof(int));
	vector<my_Matrix> sum(num);//储存每个矩阵的信息（名称，行，列，偏移量）
	vector<int> pos(num);//储存文件中读取的当前矩阵元素的位置


	//初始化sum和pos
	for (int i = 0; i < num; i++)
	{
		ifs.read((char*)&sum[i], sizeof(sum[i]));
		pos[i] = sum[i].offset;
	}

	w = 5;
	buffer matrixA(w);
	buffer matrixB(w);
	buffer matrixC(w);

	//初始化结果矩阵文件
	int result_num = 1, value = 0;//定义结果矩阵的个数和初始化的元素值
	char name[3] = { 'R','e','s' };//定义结果矩阵的名称
	my_Matrix result(name, sum[0].row, sum[1].col, 5);//初始化结果矩阵类，偏移量为定值5
	ofs.write((char*)&result_num, sizeof(int));
	ofs.write((char*)&result, sizeof(result));
	for (int i = 0; i < sum[0].row; i++)//初始化矩阵中的元素
	{
		for (int j = 0; j < sum[1].col; j++)
		{
			ofs.write((char*)&value, sizeof(int));
		}
	}

	int count = 0;

	for (int i = 0; i < sum[0].row; i++)//依次读取A矩阵的行
	{
		for (int j = 0; j < ceil(1.0 * sum[0].col / w); j++)//根据cache大小依次读取A矩阵的列
		{
			//将文件中的A矩阵元素读取到A的cache中
			ifs.seekg(pos[0] * sizeof(int), ios::beg);//储存时使用字节为单位，此处转换为以字为单位
			for (int k = 0; k < w; k++)//将txt文件中的元素放入A矩阵的cache中
			{
				if (j * w + k >= sum[0].col)
				{
					matrixA.col[k] = -1;
					break;
				}
				ifs.read((char*)&matrixA.temp[k], sizeof(int));
				matrixA.col[k] = (pos[0] - sum[0].offset) % sum[0].col;
				pos[0]++;
			}
			count++;

			for (int u = 0; u < w; u++)//遍历A矩阵cache中的元素进行计算，此时A矩阵中第u个元素对应于B矩阵第j * w + u行
			{
				if (matrixA.col[u] < 0)
				{
					break;
				}
				pos[1] = sum[1].offset + matrixA.col[u] * sum[1].row;//每次读取B矩阵的行，需更新pos的值
				
				//读取B矩阵时需要根据A矩阵中cache中元素的行列位置进行读取
				for (int n = 0; n < ceil(1.0 * sum[1].col / w); n++)
				{
					ifs.seekg(pos[1] * sizeof(int), ios::beg);
					for (int m = 0; m < w; m++)
					{
						if (n * w + m >= sum[1].col)
						{
							matrixB.col[m] = -1;
							break;
						}
						ifs.read((char*)&matrixB.temp[m], sizeof(int));
						matrixB.col[m] = n * w + m;
						pos[1]++;
					}
					count++;

					for (int v = 0; v < w; v++)
					{
						if (matrixB.col[v] < 0)
						{
							break;
						}
						//将之前的计算结果从文件中读出
						ofs.seekg((5 + i * sum[0].row + matrixB.col[v]) * sizeof(int), ios::beg);
						ofs.read((char*)&matrixC.temp[v], sizeof(int));
						matrixC.temp[v] += matrixA.temp[u] * matrixB.temp[v];

						//将结果写入文件
						ofs.seekp((5 + i * sum[0].row + matrixB.col[v]) * sizeof(int), ios::beg);
						ofs.write((char*)&matrixC.temp[v], sizeof(int));
					}
					count++;
				}
			}
		}
	}
	ifs.close();//关闭计算矩阵文件
	ofs.close();//关闭结果矩阵文件
	
	
	QueryPerformanceCounter(&t2);
	double time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
	cout << "time = " << time << endl;  //输出时间（单位：ｓ）
	
	//以下为验证计算结果正确，并非必须
	cout << "The result is followed" << endl;
	ifstream infile;
	infile.open("result.txt", ios::in | ios::binary);
	if (!infile.is_open())
	{
		cout << "reading file failed" << endl;
		return 0;
	}
	int number = 0;
	my_Matrix a;
	vector<vector<int>> matrixResult(sum[0].row, vector<int>(sum[1].col));//创建结果矩阵
	infile.read((char*)&number, sizeof(int));
	infile.read((char*)&a, sizeof(a));
	infile.seekg(5 * sizeof(int), ios::beg);
	for (int i = 0; i < sum[0].row; i++)
	{
		for (int j = 0; j < sum[1].col; j++)
		{
			infile.read((char*)&matrixResult[i][j], sizeof(int));
		}
	}

	for (int i = 0; i < sum[0].row; i++)
	{
		for (int j = 0; j < sum[1].col; j++)
		{
			cout << matrixResult[i][j] << '\t';
		}
		cout << endl;
	}
	infile.close();

	return 0;
}