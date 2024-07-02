#pragma once
#include "Buffer.h"
#include "TreeNode.h"

using namespace std;

class LoserTree
{
public:
    TreeNode* winnerArray;// 胜者树
    TreeNode* loserArray;// 败者树

    LoserTree(int* data, int size)
    {
        this->size = size;
        this->invalidNum = 0;
        this->loserArray = new TreeNode[2 * this->size];// 后面size个是外部节点
        this->winnerArray = new TreeNode[this->size];
        // 初始化败者树的外部节点
        for (int i = 0; i < size; ++i)
            loserArray[i + size] = TreeNode(data[i], i, true);
        // 生成败者树（从外部结点比赛）
        int currentIndex = this->size - 1;
        while (currentIndex != 0) {
            modify(currentIndex);
            --currentIndex;
        }
        // loserArray[0]储存胜者（最小值）
        this->loserArray[0] = this->winnerArray[1];
    }
    LoserTree(Buffer data, int size)
    {
        this->size = size;
        this->invalidNum = 0;
        this->loserArray = new TreeNode[2 * this->size];// 后面size个是外部节点
        this->winnerArray = new TreeNode[this->size];
        // 初始化败者树的外部节点
        for (int i = 0; i < size; ++i)
            loserArray[i + size] = TreeNode(data.pop(), i, true);
        // 生成败者树（从外部结点比赛）
        int currentIndex = this->size - 1;
        while (currentIndex != 0) {
            modify(currentIndex);
            --currentIndex;
        }
        // loserArray[0]储存胜者（最小值）
        this->loserArray[0] = this->winnerArray[1];
    }
    TreeNode getTop()// 返回胜者（最小值）
    {
        return loserArray[0];
    }
    void modify(int index)// 调整一个节点
    {

        // 找到左右子节点
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
        // 比赛
        TreeNode loser, winner;
        if ((leftChild.getIsValid()) && (rightChild.getIsValid())) {
            // 左右都有效
            loser = (leftChild.getValue() > rightChild.getValue()) ? leftChild : rightChild;
            winner = ((loser.getValue() == leftChild.getValue()) && (loser.getKey() == leftChild.getKey())) ? rightChild : leftChild;
        }
        else if ((!leftChild.getIsValid()) && (rightChild.getIsValid())) {
            // 左无效
            loser = leftChild;
            winner = rightChild;
        }
        else if ((leftChild.getIsValid()) && (!rightChild.getIsValid())) {
            // 右无效
            loser = rightChild;
            winner = leftChild;
        }
        else {
            // 左右都无效
            loser = (leftChild.getValue() > rightChild.getValue()) ? leftChild : rightChild;
            winner = ((loser.getValue() == leftChild.getValue()) && (loser.getKey() == leftChild.getKey())) ? rightChild : leftChild;
        }
        // 子节点上升
        loserArray[index] = loser;
        winnerArray[index] = winner;
    }
    void adjust(int index, int newValue)// 读取一个外部节点，并更新树
    {
        int newIndex = index + this->size;
        if (newValue < this->loserArray[0].getValue()) {
            // 新的值比winner更小->标记为无效节点，放入下一个run
            this->loserArray[newIndex] = TreeNode(newValue, index, false);
            ++this->invalidNum;
        }
        else
            // 新的值有效
            this->loserArray[newIndex] = TreeNode(newValue, index, true);
        // 和父节点比较，上升
        int currentIndex = newIndex / 2;
        while (currentIndex != 0) {
            this->modify(currentIndex);
            currentIndex /= 2;
        }
        //更新胜者
        this->loserArray[0] = this->winnerArray[1];
    }
    void adjust(int newValue, bool isValid)// 只用于清空树
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
    bool needToCreateNewRun()// 全部是无效节点时，需要结束当前run
    {
        return (this->invalidNum == this->size);
    }
    void setValid()// 重置树的isValid
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
    int getInvalidNum()// 返回无效节点个数
    {
        return this->invalidNum;
    }

private:
    int size;       // 外部节点数量
    int invalidNum; // 无效结点的数量（必须放进下一个run）
};
