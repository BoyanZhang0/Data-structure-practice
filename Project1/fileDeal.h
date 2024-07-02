#pragma once
#ifndef FILEDEAL_H
#define FILEDEAL_H

#include<fileDeal.h>
#include<matrixMul.h>
#include<fstream>
#include<iostream>
#include<string>

void fileWrite()
{
	ofstream outfile;
	outfile.open("matrix.txt", ios::out | ios::binary);
	char s1[3] = { 'A','B','C' };
	char s2[3] = { 'D','E','F' };
	my_Matrix a(s1, 6, 6, 9);
	my_Matrix b(s2, 6, 6, 45);
	int num = 2;
	outfile.write((char*)&num, sizeof(num));//记录矩阵个数
	outfile.write((char*)&a, sizeof(a));//记录矩阵a的具体信息
	outfile.write((char*)&b, sizeof(b));//记录矩阵b的具体信息
	for (int i = 0; i < 2; i++)//假设两个类a和b都为6 * 6的矩阵，且矩阵内元素依次为1-36
	{
		for (int j = 1; j < 37; j++)
		{
			outfile.write((char*)&j, sizeof(j));
		}
	}
	outfile.close();

	return;
}
#endif