#pragma once
#include<queue>
#include <algorithm>

using namespace std;

class Buffer {
public:
    int size; //大小
    bool isFull; //buffer是否已满
    bool isEmpty; //buffer是否为空
    queue<int>buffer;

    Buffer(int s = 8)
    {
        size = s;
        isEmpty = true;
        isFull = false;
    }

    void reset(int s)//重设大小并清空
    {
        size = s;
        isEmpty = true;
        isFull = false;
        while (!buffer.empty()) buffer.pop();
    }
    bool push(int value)//插入一个元素
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
    int front()//返回第一个元素
    {
        if (buffer.empty())
            return 0;
        else {
            int front = buffer.front();
            return front;
        }
    }
    int pop()//弹出第一个元素
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

    void sortBuffer()//将此buffer排序(升序)
    {
        //使用vector和sort实现排序
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


