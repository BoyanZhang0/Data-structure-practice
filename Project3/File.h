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
    int startPostion; //��ǰ�����ļ�������λ��
    int endPosition; //�������λ�ý���
    File() { startPostion = 0; };
    void ReadInput(string filepath, int size, Buffer* inputBuffer)//�������ļ���ȡ��inputBuffer
    {
        inputBuffer->reset(size);
        int currentP = 0;
        int readSize = 0;//�����˼�����
        int bufferSize = inputBuffer->size;
        //buffer��������
        if (bufferSize < size)
        {
            cout << "InputBuffer is not enough.";
            return;
        }
        fstream fin(filepath, ios::in);
        string line;
        // �ļ��޷��򿪱���
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
                    //buffer�Ѿ��������˳�
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
    void writeOutput(string filepath, int size, Buffer* outputBuffer)//��outputBufferд������ļ�
    {
        fstream fout;
        fout.open(filepath, ios::out | ios::app);

        if (!fout.is_open())
        {
            cout << "Error" << endl;
            return;
        }

        //д���ļ�
        ostringstream oss;
        while (!outputBuffer->isEmpty)
            oss << outputBuffer->pop() << "\n";

        string outString = oss.str();
        fout << outString;
        fout.close();
    }
    void createInitialRuns(string filepath, string outputPath, int runSize, Buffer* outputBuffer)//���ɳ�ʼruns
    {
        int currentP = 0;
        int dataSize = getDataSize(filepath);
        bool isFirst = true;
        while (currentP < dataSize) {
            //���ļ���ȡ��outputBuffer
            ReadInput(filepath, runSize, outputBuffer);
            currentP += outputBuffer->size;
            //��������
            outputBuffer->sortBuffer();
            //д���ļ�
            writeOutput(outputPath, outputBuffer->size, outputBuffer);
            outputBuffer->reset(runSize);
        }
        startPostion = 0;
    }
    int getDataSize(string filepath)//�������ݸ���
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


