#pragma once
class TreeNode
{
private:
    int value;
    int key;
    bool isValid;

public:
    TreeNode(int value = 0, int key = -1, bool isValid = false)
    {
        this->value = value;
        this->key = key;
        this->isValid = isValid;
    }
    int getValue()
    {
        return this->value;
    }
    int getKey()
    {
        return this->key;
    }
    bool getIsValid()
    {
        return this->isValid;
    }
    void setValid()
    {
        this->isValid = true;
    }
};

