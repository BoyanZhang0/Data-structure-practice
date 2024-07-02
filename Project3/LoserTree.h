#pragma once
#include "Buffer.h"
#include "TreeNode.h"

using namespace std;

class LoserTree
{
public:
    TreeNode* winnerArray;// ʤ����
    TreeNode* loserArray;// ������

    LoserTree(int* data, int size)
    {
        this->size = size;
        this->invalidNum = 0;
        this->loserArray = new TreeNode[2 * this->size];// ����size�����ⲿ�ڵ�
        this->winnerArray = new TreeNode[this->size];
        // ��ʼ�����������ⲿ�ڵ�
        for (int i = 0; i < size; ++i)
            loserArray[i + size] = TreeNode(data[i], i, true);
        // ���ɰ����������ⲿ��������
        int currentIndex = this->size - 1;
        while (currentIndex != 0) {
            modify(currentIndex);
            --currentIndex;
        }
        // loserArray[0]����ʤ�ߣ���Сֵ��
        this->loserArray[0] = this->winnerArray[1];
    }
    LoserTree(Buffer data, int size)
    {
        this->size = size;
        this->invalidNum = 0;
        this->loserArray = new TreeNode[2 * this->size];// ����size�����ⲿ�ڵ�
        this->winnerArray = new TreeNode[this->size];
        // ��ʼ�����������ⲿ�ڵ�
        for (int i = 0; i < size; ++i)
            loserArray[i + size] = TreeNode(data.pop(), i, true);
        // ���ɰ����������ⲿ��������
        int currentIndex = this->size - 1;
        while (currentIndex != 0) {
            modify(currentIndex);
            --currentIndex;
        }
        // loserArray[0]����ʤ�ߣ���Сֵ��
        this->loserArray[0] = this->winnerArray[1];
    }
    TreeNode getTop()// ����ʤ�ߣ���Сֵ��
    {
        return loserArray[0];
    }
    void modify(int index)// ����һ���ڵ�
    {

        // �ҵ������ӽڵ�
        TreeNode leftChild;
        TreeNode rightChild;
        if (index < this->size / 2) {
            leftChild = winnerArray[index * 2];
            rightChild = winnerArray[index * 2 + 1];
        }
        else {
            leftChild = loserArray[index * 2];
            rightChild = loserArray[index * 2 + 1];
        }
        // ����
        TreeNode loser, winner;
        if ((leftChild.getIsValid()) && (rightChild.getIsValid())) {
            // ���Ҷ���Ч
            loser = (leftChild.getValue() > rightChild.getValue()) ? leftChild : rightChild;
            winner = ((loser.getValue() == leftChild.getValue()) && (loser.getKey() == leftChild.getKey())) ? rightChild : leftChild;
        }
        else if ((!leftChild.getIsValid()) && (rightChild.getIsValid())) {
            // ����Ч
            loser = leftChild;
            winner = rightChild;
        }
        else if ((leftChild.getIsValid()) && (!rightChild.getIsValid())) {
            // ����Ч
            loser = rightChild;
            winner = leftChild;
        }
        else {
            // ���Ҷ���Ч
            loser = (leftChild.getValue() > rightChild.getValue()) ? leftChild : rightChild;
            winner = ((loser.getValue() == leftChild.getValue()) && (loser.getKey() == leftChild.getKey())) ? rightChild : leftChild;
        }
        // �ӽڵ�����
        loserArray[index] = loser;
        winnerArray[index] = winner;
    }
    void adjust(int index, int newValue)// ��ȡһ���ⲿ�ڵ㣬��������
    {
        int newIndex = index + this->size;
        if (newValue < this->loserArray[0].getValue()) {
            // �µ�ֵ��winner��С->���Ϊ��Ч�ڵ㣬������һ��run
            this->loserArray[newIndex] = TreeNode(newValue, index, false);
            ++this->invalidNum;
        }
        else
            // �µ�ֵ��Ч
            this->loserArray[newIndex] = TreeNode(newValue, index, true);
        // �͸��ڵ�Ƚϣ�����
        int currentIndex = newIndex / 2;
        while (currentIndex != 0) {
            this->modify(currentIndex);
            currentIndex /= 2;
        }
        //����ʤ��
        this->loserArray[0] = this->winnerArray[1];
    }
    void adjust(int newValue, bool isValid)// ֻ���������
    {
        TreeNode node(newValue, this->getTop().getKey(), isValid);
        int currentIndex = this->size + this->getTop().getKey();
        this->loserArray[currentIndex] = node;
        int curIndex = currentIndex / 2;

        if (!isValid)
            ++this->invalidNum;
        while (curIndex != 0) {
            this->modify(curIndex);
            curIndex /= 2;
        }
        this->loserArray[0] = this->winnerArray[1];
    }
    bool needToCreateNewRun()// ȫ������Ч�ڵ�ʱ����Ҫ������ǰrun
    {
        return (this->invalidNum == this->size);
    }
    void setValid()// ��������isValid
    {
        for (int i = 0; i < this->size; ++i) {
            this->winnerArray[i].setValid();
            this->loserArray[i].setValid();
            this->loserArray[i + this->size].setValid();
        }
        this->invalidNum = 0;
    }
    int getSize()
    {
        return this->size;
    }
    int getInvalidNum()// ������Ч�ڵ����
    {
        return this->invalidNum;
    }

private:
    int size;       // �ⲿ�ڵ�����
    int invalidNum; // ��Ч��������������Ž���һ��run��
};
