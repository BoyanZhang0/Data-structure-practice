#pragma once
#include <string>
#include<fstream>
#include<sstream>
#include<iostream>
#include "Buffer.h"

using namespace std;

class File
{
public:
    int startPostion; //当前输入文件读到的位置
    int endPosition; //读到这个位置结束
    File() { startPostion = 0; };
    void ReadInput(string filepath, int size, Buffer* inputBuffer)//将输入文件读取到inputBuffer
    {
        inputBuffer->reset(size);
        int currentP = 0;
        int readSize = 0;//读到了几个数
        int bufferSize = inputBuffer->size;
        //buffer不够报错
        if (bufferSize < size)
        {
            cout << "InputBuffer is not enough.";
            return;
        }
        fstream fin(filepath, ios::in);
        string line;
        // 文件无法打开报错
        if (!fin.is_open())
        {
            cout << "Error" << endl;
            return;
        }

        while (getline(fin, line)) {
            istringstream iss(line);
            if (currentP < startPostion) {
                ++currentP;
            }
            else {
                if (readSize >= size)
                    //buffer已经读满，退出
                    break;

                else {
                    int n;
                    iss >> n;
                    inputBuffer->push(n);
                    ++readSize;
                    ++currentP;
                    ++startPostion;
                }
            }
        }
        fin.close();
    }
    void writeOutput(string filepath, int size, Buffer* outputBuffer)//将outputBuffer写到输出文件
    {
        fstream fout;
        fout.open(filepath, ios::out | ios::app);

        if (!fout.is_open())
        {
            cout << "Error" << endl;
            return;
        }

        //写入文件
        ostringstream oss;
        while (!outputBuffer->isEmpty)
            oss << outputBuffer->pop() << "\n";

        string outString = oss.str();
        fout << outString;
        fout.close();
    }
    void createInitialRuns(string filepath, string outputPath, int runSize, Buffer* outputBuffer)//生成初始runs
    {
        int currentP = 0;
        int dataSize = getDataSize(filepath);
        bool isFirst = true;
        while (currentP < dataSize) {
            //从文件读取到outputBuffer
            ReadInput(filepath, runSize, outputBuffer);
            currentP += outputBuffer->size;
            //快速排序
            outputBuffer->sortBuffer();
            //写入文件
            writeOutput(outputPath, outputBuffer->size, outputBuffer);
            outputBuffer->reset(runSize);
        }
        startPostion = 0;
    }
    int getDataSize(string filepath)//返回数据个数
    {
        fstream fin(filepath, ios::in);

        if (!fin.is_open())
        {
            cout << "Error"<<endl;
            return 0;
        }
        string line;
        int size = 0;
        while (getline(fin, line))
            ++size;
        fin.close();
        return size;
    }
};


