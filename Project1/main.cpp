/*
�ļ���ʽ�������´洢��ʽ��
ǰ4���ֽڴ洢һ��int���ʹ������洢�ľ������n
֮��n�����󶼷ֱ��ԣ����ƣ�������������ƫ����������ʽ������Ϣ�洢������������char[4]���ͽ��д洢��������������ƫ������int���ͽ��д洢��
���������δ洢
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
	
	ifstream ifs;//��ȡ��������ļ�
	ifs.open("matrix.txt", ios::in | ios::binary);
	if (!ifs.is_open())
	{
		cout << "reading file failed" << endl;
		return 0;
	}

	//��ȡ��������ļ�
	fstream ofs;
	ofs.open("result.txt", ios::binary | ios::out | ios::in);

	int num = 0, w = 0;//�������,cache�Ĵ�С

	ifs.read((char*)&num, sizeof(int));
	vector<my_Matrix> sum(num);//����ÿ���������Ϣ�����ƣ��У��У�ƫ������
	vector<int> pos(num);//�����ļ��ж�ȡ�ĵ�ǰ����Ԫ�ص�λ��


	//��ʼ��sum��pos
	for (int i = 0; i < num; i++)
	{
		ifs.read((char*)&sum[i], sizeof(sum[i]));
		pos[i] = sum[i].offset;
	}

	w = 5;
	buffer matrixA(w);
	buffer matrixB(w);
	buffer matrixC(w);

	//��ʼ����������ļ�
	int result_num = 1, value = 0;//����������ĸ����ͳ�ʼ����Ԫ��ֵ
	char name[3] = { 'R','e','s' };//���������������
	my_Matrix result(name, sum[0].row, sum[1].col, 5);//��ʼ����������࣬ƫ����Ϊ��ֵ5
	ofs.write((char*)&result_num, sizeof(int));
	ofs.write((char*)&result, sizeof(result));
	for (int i = 0; i < sum[0].row; i++)//��ʼ�������е�Ԫ��
	{
		for (int j = 0; j < sum[1].col; j++)
		{
			ofs.write((char*)&value, sizeof(int));
		}
	}

	int count = 0;

	for (int i = 0; i < sum[0].row; i++)//���ζ�ȡA�������
	{
		for (int j = 0; j < ceil(1.0 * sum[0].col / w); j++)//����cache��С���ζ�ȡA�������
		{
			//���ļ��е�A����Ԫ�ض�ȡ��A��cache��
			ifs.seekg(pos[0] * sizeof(int), ios::beg);//����ʱʹ���ֽ�Ϊ��λ���˴�ת��Ϊ����Ϊ��λ
			for (int k = 0; k < w; k++)//��txt�ļ��е�Ԫ�ط���A�����cache��
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

			for (int u = 0; u < w; u++)//����A����cache�е�Ԫ�ؽ��м��㣬��ʱA�����е�u��Ԫ�ض�Ӧ��B�����j * w + u��
			{
				if (matrixA.col[u] < 0)
				{
					break;
				}
				pos[1] = sum[1].offset + matrixA.col[u] * sum[1].row;//ÿ�ζ�ȡB������У������pos��ֵ
				
				//��ȡB����ʱ��Ҫ����A������cache��Ԫ�ص�����λ�ý��ж�ȡ
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
						//��֮ǰ�ļ��������ļ��ж���
						ofs.seekg((5 + i * sum[0].row + matrixB.col[v]) * sizeof(int), ios::beg);
						ofs.read((char*)&matrixC.temp[v], sizeof(int));
						matrixC.temp[v] += matrixA.temp[u] * matrixB.temp[v];

						//�����д���ļ�
						ofs.seekp((5 + i * sum[0].row + matrixB.col[v]) * sizeof(int), ios::beg);
						ofs.write((char*)&matrixC.temp[v], sizeof(int));
					}
					count++;
				}
			}
		}
	}
	ifs.close();//�رռ�������ļ�
	ofs.close();//�رս�������ļ�
	
	
	QueryPerformanceCounter(&t2);
	double time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
	cout << "time = " << time << endl;  //���ʱ�䣨��λ����
	
	//����Ϊ��֤��������ȷ�����Ǳ���
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
	vector<vector<int>> matrixResult(sum[0].row, vector<int>(sum[1].col));//�����������
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