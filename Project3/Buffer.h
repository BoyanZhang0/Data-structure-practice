#pragma once
#include<queue>
#include <algorithm>

using namespace std;

class Buffer {
public:
    int size; //��С
    bool isFull; //buffer�Ƿ�����
    bool isEmpty; //buffer�Ƿ�Ϊ��
    queue<int>buffer;

    Buffer(int s = 8)
    {
        size = s;
        isEmpty = true;
        isFull = false;
    }

    void reset(int s)//�����С�����
    {
        size = s;
        isEmpty = true;
        isFull = false;
        while (!buffer.empty()) buffer.pop();
    }
    bool push(int value)//����һ��Ԫ��
    {
        if (isFull)
            return false;
        else {
            isEmpty = false;
            buffer.push(value);
            if (buffer.size() == size) isFull = true;
            return true;
        }
    }
    int front()//���ص�һ��Ԫ��
    {
        if (buffer.empty())
            return 0;
        else {
            int front = buffer.front();
            return front;
        }
    }
    int pop()//������һ��Ԫ��
    {
        if (buffer.empty())
            return 0;
        else {
            int front = buffer.front();
            buffer.pop();
            isFull = false;
            if (buffer.empty()) isEmpty = true;
            return front;
        }
    }

    void sortBuffer()//����buffer����(����)
    {
        //ʹ��vector��sortʵ������
        vector<int>temp;
        while (!isEmpty) 
            temp.push_back(this->pop());
        
        sort(temp.begin(), temp.end());
        reverse(temp.begin(), temp.end());
        
        while (!temp.empty()) {
            this->push(temp.back());
            temp.pop_back();
        }
    }
};


