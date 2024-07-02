#include<iostream>
#include<fstream>
#include<string>
#include<time.h>
#include<windows.h>
#include<buffer.h>

using namespace std;

void init()
{
	ofstream ofs;
	ofs.open("merge1.txt", ios::out | ios::binary);

	int num = 10;
	ofs.write((char*)&num, sizeof(int));
	int pos = 21;
	for (int i = 0; i < 10; i++)
	{
		ofs.write((char*)&pos, sizeof(int));
		ofs.write((char*)&pos, sizeof(int));
		pos++;
	}
	int a[10] = { 2, 3, 10, 4, 8, 6, 2, 55, 1, 7 };
	for (int i = 0; i < 10; i++)
	{
		ofs.write((char*)&a[i], sizeof(int));
	}

	ofs.close();
}

int changeFile(int figure)
{
	if (figure == 1)
	{
		return 2;
	}
	else
	{
		return 1;
	}
}
int main()
{
	init();
	fstream file;
	file.open("merge1.txt", ios::in | ios::binary);
	if (!file.is_open())
	{
		cout << "failed" << endl;
		return 0;
	}

	int num_seg = 0, num_vol = 2, count = 1;//num_seg���ڼ�¼��ǰ��������Ķ�����num_volΪbuffer��������count����ȷ�����ļ�Ϊ1����2
	buffer my_buffer(num_vol);
	file.read((char*)&num_seg, sizeof(int));
	file.close();

	//��¼ʱ��
	LARGE_INTEGER t1, t2, tc;
	QueryPerformanceFrequency(&tc);
	QueryPerformanceCounter(&t1);

	int test = 4;
	while (num_seg > 1 && test > 0)
	{
		int time = num_seg;//��¼һ���м�����
		int my_location = 1, locationOfValue1 = 0, locationOfValue2 = 0;//location��¼��ȡ�ε���ʼ�ͽ���ֵ������λ�ã�locationOfValue1��locationOfValue2��¼�����ȡ������ֵ��λ��
		num_seg = ceil(num_seg * 1.0 / 2);//ȷ�������Ķ���
		fstream file1, file2;
		//ȷ���ĸ��ļ�Ϊ���ļ����ĸ�Ϊд�ļ�
		if (count == 1)
		{
			file1.open("merge1.txt", ios::in | ios::binary);
			file2.open("merge2.txt", ios::in | ios::out | ios::binary | ios::trunc);
		}
		else
		{
			file1.open("merge2.txt", ios::in | ios::binary);
			file2.open("merge1.txt", ios::in | ios::out | ios::binary | ios::trunc);
		}

		if (!file1.is_open() || !file2.is_open())
		{
			cout << "failed" << endl;
			return 0;
		}
		file2.write((char*)&num_seg, sizeof(int));
		//ȷ������Ϊ��������ż��
		if (time % 2 == 0)
		{
			//д��ÿ���ε���ֵֹ
			int beg1, end1, beg2, end2;//��¼������Ҫ����ε���ֵֹ
			int writeNum = 0;
			int temp = writeNum + num_seg * 2 + 1;
			for (int n = 0; n < num_seg; n++)
			{
				file1.seekg(my_location * sizeof(int), ios::beg);
				file1.read((char*)&beg1, sizeof(int));
				file1.read((char*)&end1, sizeof(int));
				file1.read((char*)&beg2, sizeof(int));
				file1.read((char*)&end2, sizeof(int));


				file2.write((char*)&temp, sizeof(int));

				writeNum = end1 - beg1 + end2 - beg2 + 1;
				temp += writeNum;
				file2.write((char*)&temp, sizeof(int));
				my_location += 4;
				temp++;
			}

			my_location = 1;
			for (int i = time / 2; i > 0; i--)
			{
				file1.seekg(my_location * sizeof(int), ios::beg);
				file1.read((char*)&beg1, sizeof(int));
				file1.read((char*)&end1, sizeof(int));
				file1.read((char*)&beg2, sizeof(int));
				file1.read((char*)&end2, sizeof(int));
				my_location += 4;
				locationOfValue1 = beg1;
				locationOfValue2 = beg2;

				int numOfValue1 = end1 - beg1 + 1;
				int	numOfValue2 = end2 - beg2 + 1;
				int check1 = 1, check2 = 1;//���input�е�һ�λ��ߵڶ����Ƿ�Ƚ����
				int num_out = 0;//��¼output���ж�������
				int num_read1 = 0, num_read2 = 0;//ȷ��input���Ѿ��Ƚ��˼�������
				int break1 = num_vol, break2 = num_vol;//��ֹ�޷������ζ�ȡ�������ݣ��Ӷ���¼input�д��浽�ڼ�λ
				while ((numOfValue1 > 0 || check1 == 0) && (numOfValue2 > 0 || check2 == 0))
				{
					//��ȡ��һ�����ݵ�input��
					if (check1 && numOfValue1 > 0)
					{
						for (int j = 0; j < num_vol; j++)
						{
							file1.seekg(locationOfValue1 * sizeof(int), ios::beg);
							file1.read((char*)&my_buffer.input1[j], sizeof(int));
							locationOfValue1++;
							numOfValue1--;
							if (numOfValue1 <= 0)
							{
								break1 = j + 1;
								break;
							}
						}
						check1 = 0;
					}
					//��ȡ�ڶ������ݵ�input��
					if (check2 && numOfValue2 > 0)
					{
						for (int j = 0; j < num_vol; j++)
						{
							file1.seekg(locationOfValue2 * sizeof(int), ios::beg);
							file1.read((char*)&my_buffer.input2[j], sizeof(int));
							locationOfValue2++;
							numOfValue2--;

							if (numOfValue2 <= 0)
							{
								break2 = j + 1;
								break;
							}
						}
						check2 = 0;
					}

					//��ʼ�Ƚ�
					for (int n = 0; n < num_vol; n++)
					{
						if (my_buffer.input1[num_read1] <= my_buffer.input2[num_read2])
						{
							my_buffer.output[n] = my_buffer.input1[num_read1];
							num_read1++;
							num_out++;
						}
						else
						{
							my_buffer.output[n] = my_buffer.input2[num_read2];
							num_read2++;
							num_out++;
						}
						//���һ��input�ѱ�ȫ����ȡ����ֹͣ
						if (num_read1 >= num_vol)
						{
							check1 = 1;
							num_read1 = 0;
							break;
						}
						if (num_read2 >= num_vol)
						{
							check2 = 1;
							num_read2 = 0;
							break;
						}
						if (num_read1 >= break1)
						{
							check1 = 1;
							break;
						}
						if (num_read2 >= break2)
						{
							check2 = 1;
							break;
						}
					}

					for (int m = 0; m < num_out; m++)
					{
						file2.write((char*)&my_buffer.output[m], sizeof(int));
					}
					num_out = 0;
				}

				//��input��ʣ���ֵд���ļ�
				while (num_read1 < break1 && check1 == 0)
				{
					file2.write((char*)&my_buffer.input1[num_read1], sizeof(int));
					num_read1++;
				}
				while (num_read2 < break2 && check2 == 0)
				{
					file2.write((char*)&my_buffer.input2[num_read2], sizeof(int));
					num_read2++;
				}
				//��ʣ�����ֱֵ��д���ļ�
				if (numOfValue1 > 0)
				{
					num_out = num_vol;
					for (int j = 0; j < num_vol; j++)
					{
						file1.seekg(locationOfValue1 * sizeof(int), ios::beg);
						file1.read((char*)&my_buffer.output[j], sizeof(int));
						locationOfValue1++;
						numOfValue1--;
						if (numOfValue1 <= 0)
						{
							num_out = j + 1;
							break;
						}
					}
					for (int m = 0; m < num_out; m++)
					{
						file2.write((char*)&my_buffer.output[m], sizeof(int));
					}
				}
				if (numOfValue2 > 0)
				{
					num_out = num_vol;
					for (int j = 0; j < num_vol; j++)
					{
						file1.seekg(locationOfValue2 * sizeof(int), ios::beg);
						file1.read((char*)&my_buffer.output[j], sizeof(int));
						locationOfValue2++;
						numOfValue2--;
						if (numOfValue1 <= 0)
						{
							num_out = j + 1;
							break;
						}
					}
					for (int m = 0; m < num_out; m++)
					{
						file2.write((char*)&my_buffer.output[m], sizeof(int));
					}
				}
			}
		}
		else
		{
			//д��ÿ���ε���ֵֹ
			int beg1, end1, beg2, end2;//��¼������Ҫ����ε���ֵֹ
			int writeNum = 0;
			int temp = writeNum + num_seg * 2 + 1;

			for (int n = 0; n < num_seg - 1; n++)
			{
				file1.seekg(my_location * sizeof(int), ios::beg);
				file1.read((char*)&beg1, sizeof(int));
				file1.read((char*)&end1, sizeof(int));
				file1.read((char*)&beg2, sizeof(int));
				file1.read((char*)&end2, sizeof(int));


				file2.write((char*)&temp, sizeof(int));

				writeNum = end1 - beg1 + end2 - beg2 + 1;
				temp += writeNum;
				file2.write((char*)&temp, sizeof(int));
				my_location += 4;
				temp++;
			}
			//���һ�ε���ֵֹ����д��
			file1.seekg(my_location * sizeof(int), ios::beg);
			file1.read((char*)&beg1, sizeof(int));
			file1.read((char*)&end1, sizeof(int));
			file2.write((char*)&temp, sizeof(int));
			writeNum = end1 - beg1;
			temp += writeNum;
			file2.write((char*)&temp, sizeof(int));

			my_location = 1;
			for (int i = time / 2; i > 0; i--)
			{
				file1.seekg(my_location * sizeof(int), ios::beg);
				file1.read((char*)&beg1, sizeof(int));
				file1.read((char*)&end1, sizeof(int));
				file1.read((char*)&beg2, sizeof(int));
				file1.read((char*)&end2, sizeof(int));
				my_location += 4;
				locationOfValue1 = beg1;
				locationOfValue2 = beg2;

				int numOfValue1 = end1 - beg1 + 1;
				int	numOfValue2 = end2 - beg2 + 1;
				int check1 = 1, check2 = 1;//���input�е�һ�λ��ߵڶ����Ƿ�Ƚ����
				int num_out = 0;//��¼output���ж�������
				int num_read1 = 0, num_read2 = 0;//ȷ��input���Ѿ��Ƚ��˼�������
				int break1 = num_vol, break2 = num_vol;//��ֹ�޷������ζ�ȡ�������ݣ��Ӷ���¼input�д��浽�ڼ�λ
				while (numOfValue1 > 0 || numOfValue2 > 0)
				{
					//��ȡ��һ�����ݵ�input��
					if (check1)
					{
						for (int j = 0; j < num_vol; j++)
						{
							file1.seekg(locationOfValue1 * sizeof(int), ios::beg);
							file1.read((char*)&my_buffer.input1[j], sizeof(int));
							locationOfValue1++;
							numOfValue1--;
							if (numOfValue1 <= 0)
							{
								break1 = j + 1;
								break;
							}
						}
						check1 = 0;
					}
					//��ȡ�ڶ������ݵ�input��
					if (check2)
					{
						for (int j = 0; j < num_vol; j++)
						{
							file1.seekg(locationOfValue2 * sizeof(int), ios::beg);
							file1.read((char*)&my_buffer.input2[j], sizeof(int));
							locationOfValue2++;
							numOfValue2--;
							if (numOfValue2 <= 0)
							{
								break2 = j + 1;
								break;
							}
						}
						check2 = 0;
					}

					//��ʼ�Ƚ�
					for (int n = 0; n < num_vol; n++)
					{
						if (my_buffer.input1[num_read1] <= my_buffer.input2[num_read2])
						{
							my_buffer.output[n] = my_buffer.input1[num_read1];
							num_read1++;
							num_out++;
						}
						else
						{
							my_buffer.output[n] = my_buffer.input2[num_read2];
							num_read2++;
							num_out++;
						}
						//���һ��input�ѱ�ȫ����ȡ����ֹͣ
						if (num_read1 >= num_vol)
						{
							check1 = 1;
							num_read1 = 0;
							break;
						}
						if (num_read2 >= num_vol)
						{
							check2 = 1;
							num_read2 = 0;
							break;
						}
						if (num_read1 >= break1)
						{
							break;
						}
						if (num_read2 >= break2)
						{
							break;
						}
					}
					for (int m = 0; m < num_out; m++)
					{
						file2.write((char*)&my_buffer.output[m], sizeof(int));
					}
					num_out = 0;
				}

				//��input��ʣ���ֵд���ļ�
				while (num_read1 < break1 && check1 == 0)
				{
					file2.write((char*)&my_buffer.input1[num_read1], sizeof(int));
					num_read1++;
				}
				while (num_read2 < break2 && check2 == 0)
				{
					file2.write((char*)&my_buffer.input2[num_read2], sizeof(int));
					num_read2++;
				}
				//��ʣ�����ֱֵ��д���ļ�
				if (numOfValue1 > 0)
				{
					for (int j = 0; j < num_vol; j++)
					{
						file1.seekg(locationOfValue1 * sizeof(int), ios::beg);
						file1.read((char*)&my_buffer.output[j], sizeof(int));
						locationOfValue1++;
						numOfValue1--;
						if (numOfValue1 <= 0)
						{
							num_out = j + 1;
							break;
						}
					}
					for (int m = 0; m < num_out; m++)
					{
						file2.write((char*)&my_buffer.output[m], sizeof(int));
					}
				}
				if (numOfValue1 > 0)
				{
					for (int j = 0; j < num_vol; j++)
					{
						file1.seekg(locationOfValue2 * sizeof(int), ios::beg);
						file1.read((char*)&my_buffer.output[j], sizeof(int));
						locationOfValue1++;
						numOfValue1--;
						if (numOfValue1 <= 0)
						{
							num_out = j + 1;
							break;
						}
					}
					for (int m = 0; m < num_out; m++)
					{
						file2.write((char*)&my_buffer.output[m], sizeof(int));
					}
				}
			}
			//���һ��ֻ��ֱ��д��

			file1.seekg(my_location * sizeof(int), ios::beg);
			file1.read((char*)&beg1, sizeof(int));
			file1.read((char*)&end1, sizeof(int));

			int numOfValue1 = end1 - beg1 + 1;
			locationOfValue1 = beg1;

			int num_out = 0;
			if (numOfValue1 > 0)
			{
				for (int j = 0; j < num_vol; j++)
				{
					file1.seekg(locationOfValue1 * sizeof(int), ios::beg);
					file1.read((char*)&my_buffer.output[j], sizeof(int));
					locationOfValue1++;
					numOfValue1--;
					if (numOfValue1 <= 0)
					{
						num_out = j + 1;
						break;
					}
				}
				for (int m = 0; m < num_out; m++)
				{
					file2.write((char*)&my_buffer.output[m], sizeof(int));
				}
			}
		}

		count = changeFile(count);
		file1.close();
		file2.close();
		test--;
	}


	QueryPerformanceCounter(&t2);
	double time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
	cout << "time = " << time << endl;  //���ʱ�䣨��λ����

	//��֤������
	ifstream my_file;
	my_file.open("merge1.txt", ios::in | ios::binary);

	int a = 0;
	int beg = 0, end = 0;
	int* aff = new int[4];
	cout << " ����������:" << endl;
	for (int i = 0; i < 13; i++)
	{
		my_file.read((char*)&a, sizeof(int));
		if (i > 2)//ֱ�Ӷ�ȡ����Ľ������ǰ��Ķ�������ʼֵ����ֵֹ����
		{
			cout << a << " ";
		}
	}
	return 0;
}