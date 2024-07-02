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

	int num_seg = 0, num_vol = 2, count = 1;//num_seg用于记录当前所需排序的段数，num_vol为buffer的容量，count用于确定打开文件为1还是2
	buffer my_buffer(num_vol);
	file.read((char*)&num_seg, sizeof(int));
	file.close();

	//记录时间
	LARGE_INTEGER t1, t2, tc;
	QueryPerformanceFrequency(&tc);
	QueryPerformanceCounter(&t1);

	int test = 4;
	while (num_seg > 1 && test > 0)
	{
		int time = num_seg;//记录一共有几个段
		int my_location = 1, locationOfValue1 = 0, locationOfValue2 = 0;//location记录读取段的起始和结束值的所在位置，locationOfValue1和locationOfValue2记录所需读取段中数值的位置
		num_seg = ceil(num_seg * 1.0 / 2);//确定排序后的段数
		fstream file1, file2;
		//确定哪个文件为读文件，哪个为写文件
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
		//确定段数为奇数还是偶数
		if (time % 2 == 0)
		{
			//写入每个段的起止值
			int beg1, end1, beg2, end2;//记录两个需要排序段的起止值
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
				int check1 = 1, check2 = 1;//检测input中第一段或者第二段是否比较完成
				int num_out = 0;//记录output中有多少数据
				int num_read1 = 0, num_read2 = 0;//确定input中已经比较了几个数据
				int break1 = num_vol, break2 = num_vol;//防止无法整数次读取所有数据，从而记录input中储存到第几位
				while ((numOfValue1 > 0 || check1 == 0) && (numOfValue2 > 0 || check2 == 0))
				{
					//读取第一段数据到input中
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
					//读取第二段数据到input中
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

					//开始比较
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
						//如果一个input已被全部读取，则停止
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

				//将input中剩余的值写入文件
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
				//将剩余的数值直接写入文件
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
			//写入每个段的起止值
			int beg1, end1, beg2, end2;//记录两个需要排序段的起止值
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
			//最后一段的起止值单独写入
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
				int check1 = 1, check2 = 1;//检测input中第一段或者第二段是否比较完成
				int num_out = 0;//记录output中有多少数据
				int num_read1 = 0, num_read2 = 0;//确定input中已经比较了几个数据
				int break1 = num_vol, break2 = num_vol;//防止无法整数次读取所有数据，从而记录input中储存到第几位
				while (numOfValue1 > 0 || numOfValue2 > 0)
				{
					//读取第一段数据到input中
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
					//读取第二段数据到input中
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

					//开始比较
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
						//如果一个input已被全部读取，则停止
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

				//将input中剩余的值写入文件
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
				//将剩余的数值直接写入文件
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
			//最后一段只需直接写入

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
	cout << "time = " << time << endl;  //输出时间（单位：ｓ）

	//验证排序结果
	ifstream my_file;
	my_file.open("merge1.txt", ios::in | ios::binary);

	int a = 0;
	int beg = 0, end = 0;
	int* aff = new int[4];
	cout << " 排序结果如下:" << endl;
	for (int i = 0; i < 13; i++)
	{
		my_file.read((char*)&a, sizeof(int));
		if (i > 2)//直接读取排序的结果，将前面的段数、起始值、终止值忽略
		{
			cout << a << " ";
		}
	}
	return 0;
}