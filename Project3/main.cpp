#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include<time.h>
#include<windows.h>
#include "File.h"
#include "LoserTree.h"
#include "MergeNode.h"

using namespace std;

vector<int>runLength;// 每个 initial runs 的长度
File inputFile;// 用于读取 Input.txt 的对象
File outputFile;// 用于写 0.txt（initial runs）的对象
vector<MergeNode> nodes;// 归并树
int dataSize;
int bufferSize;
int k;

//Thread 1: 读数据
void readFromDisk(Buffer* inputBuffer, string filepath) 
{
    inputBuffer->reset(inputBuffer->size);
    inputFile.ReadInput(filepath, inputBuffer->size, inputBuffer);
}

//Thread 2: 生成runs
void generateRun(LoserTree* loserTree, Buffer* inputBuffer, Buffer* outputBuffer, int* tailLength) {
    *tailLength = -1;
    int bufferSize = inputBuffer->buffer.size();
    int validNum = 0; // 有效的个数
    for (int i = 0; i < bufferSize; ++i) {
        outputBuffer->push(loserTree->getTop().getValue()); //从树中放一个到outputBuffer
        loserTree->adjust(loserTree->getTop().getKey(), inputBuffer->pop()); //读取一个到树中
        ++validNum;
        // 树已满时
        if (loserTree->needToCreateNewRun()) {
            loserTree->setValid();
            *tailLength = validNum; // 记录有几个值有效
        }
        // 没有满，返回值tailLength为-1
    }
}

//Thread 3: 写入磁盘
void writeToDisk(Buffer* outputBuffer, string filepath) {
    if (outputBuffer->isEmpty) {
        cout << "The outputBuffer is empty."<<endl;
        return;
    }
    outputFile.writeOutput(filepath, outputBuffer->buffer.size(), outputBuffer);
}

// 生成initial runs
void ImprovedCreateInitialRuns() {
    // 初始化3个buffer
    cout << "Input buffer size: ";
    cin >> bufferSize;
    Buffer* buffers = new Buffer[3];
    for (int i = 0; i < 3; ++i) {
        buffers[i] = Buffer(bufferSize);
    }

    // 初始化败者树
    cout << "Input loserTree size: ";
    int treeSize = 0;
    cin >> treeSize;
    string filepath = "Input.txt";
    string outputPath = "0.txt";
    int curLocation = 0;
    Buffer treeBuffer = Buffer(treeSize);
    inputFile.ReadInput(filepath, treeSize, &treeBuffer); // 读取treeSize个外部节点
    LoserTree* loserTree = new LoserTree(treeBuffer, treeSize);

    int bufferName = 0; // 当前的inputBuffer_1的编号
    inputFile.ReadInput(filepath, buffers[0].size, &buffers[0]); // 读入buffer[0]
    int currentLength = 0;// 当前的run的长度
    int tailLength = 0;// 一个run结束时，buffer的前tailLength个属于这个run，后面的属于下一个run
    int outputBufferSize;

    // 生成runs
    while (inputFile.startPostion < dataSize) {
        // 读数据到buffer[1]
        thread t1(readFromDisk, &buffers[(bufferName + 1) % 3], filepath);
        // 生成run：从buffer[0]读入树，输出到buffer[2]
        tailLength = 0;
        thread t2(generateRun, loserTree, &buffers[bufferName], &buffers[(bufferName + 2) % 3], &tailLength);
        t2.join();
        // buffer[2]写入磁盘
        thread t3(writeToDisk, &buffers[(bufferName + 2) % 3], outputPath);
        // 如果一个run结束了
        if (tailLength != -1 ) {
            currentLength += tailLength;// 计算run的最终长度
            runLength.emplace_back(currentLength);// 记录run的长度
            currentLength = buffers[(bufferName + 2) % 3].buffer.size() - tailLength; // 初始化下一个run的长度
        }
        else {
            // run没结束，继续增加currentLength
            currentLength += buffers[(bufferName + 2) % 3].buffer.size();
        }
        t1.join();
        t3.join();
        // 更新buffer
        bufferName = (bufferName + 1) % 3;
    }

    // 把最后一个buffer[1]写入败者树
    generateRun(loserTree, &buffers[bufferName], &buffers[(bufferName + 2) % 3], &tailLength);
    if (tailLength != -1) {
        currentLength += tailLength;
        runLength.emplace_back(currentLength);
        currentLength = buffers[(bufferName + 2) % 3].buffer.size() - tailLength;
    }
    else {
        currentLength += buffers[(bufferName + 2) % 3].buffer.size();
    }
    writeToDisk(&buffers[(bufferName + 2) % 3], outputPath);

    // 输出败者树剩下的数据
    int currentIndex = 0;
    Buffer finalBuffer(loserTree->getSize());
    while (currentIndex < loserTree->getSize()) {
        finalBuffer.push(loserTree->getTop().getValue());
        loserTree->adjust(INT_MAX, false); //输入最大值
        ++currentLength;
        if (loserTree->needToCreateNewRun()) {
            loserTree->setValid();
            runLength.emplace_back(currentLength);
            currentLength = 0;
        }
        ++currentIndex;
    }
    if (currentLength > 0)
        runLength.emplace_back(currentLength);// 最后一个run
    writeToDisk(&finalBuffer, outputPath);
}

// runs的排序规则（升序）
bool sortStruct(MergeNode& node1, MergeNode& node2)
{
    return node1.weight < node2.weight;
}

// 把第i个run复制到输出文件
void Copy(int round, int i) {
    string inputFilePath = to_string(round) + ".txt";//输入文件的名字
    string outputFilePath = to_string(round + 1) + ".txt";//输出文件的名字
    fstream fin(inputFilePath, ios::in);
    string line;
    int currentP = 0, startPosition = nodes[i].startPosition, endPosition = nodes[i].endPosition;

    fstream fout;
    fout.open(outputFilePath, ios::out | ios::app);
    ostringstream oss;

    while (getline(fin, line)) {
        istringstream iss(line);
        if (currentP < startPosition) {
            ++currentP;
        }
        else {
            if (startPosition >= endPosition)
                break;
            else {
                int n;
                iss >> n;
                oss << n << "\n";
                ++currentP;
                ++startPosition;
            }
        }
    }
    string outString = oss.str();
    fout << outString;
    fout.close();
}

// 归并k个runs
int Merge(Buffer* inputBuffer, Buffer* outputBuffer, int round, int currentK) {

    string inputFilePath = to_string(round) + ".txt";//输入文件的名字
    string outputFilePath = to_string(round + 1) + ".txt";//输出文件的名字

    bool isAllEmpty = false;// inputBuffer是否全为空
    int newRunSize = 0;

    // 记录每个run在文件的起始位置
    File* file = new File[currentK];
    for (int i = 0; i < currentK; ++i) {
        file[i].startPostion = nodes[i].startPosition;
        file[i].endPosition = nodes[i].endPosition;
        newRunSize += nodes[i].endPosition - nodes[i].startPosition;
    }

    //把runs读入inputBuffer
    for (int i = 0; i < currentK; i++) {
        if (file[i].startPostion < file[i].endPosition)
            file[i].ReadInput(inputFilePath, inputBuffer[i].size, &inputBuffer[i]);
    }

    while (!isAllEmpty) {
        isAllEmpty = true;
        int index = 0;
        int EmptyBufferIndex = -1;
        int min = INT_MAX;

        // 找到inputBuffer中最小的值，放进outputBuffer
        for (int i = 0; i < currentK; ++i) {
            if (inputBuffer[i].isEmpty) {
                // 内存已空，尝试读入数据
                if (file[i].startPostion < file[i].endPosition)
                    file[i].ReadInput(inputFilePath, inputBuffer[i].size, &inputBuffer[i]);
            }
            if (!inputBuffer[i].isEmpty) {
                if (inputBuffer[i].front() < min) {
                    min = inputBuffer[i].front();
                    index = i;
                }
            }
            isAllEmpty = isAllEmpty && inputBuffer[i].isEmpty;
        }
        if (!isAllEmpty) {
            outputBuffer->push(inputBuffer[index].pop());// 放进outputBuffer
            if (outputBuffer->isFull)
                file[0].writeOutput(outputFilePath, outputBuffer->size, outputBuffer); // outputBuffer满了，输出一次
        }
    };

    // 输出outputBuffer中遗留的数
    if (!outputBuffer->isEmpty) {
        file[0].writeOutput(outputFilePath, outputBuffer->size, outputBuffer);
        outputBuffer->reset(outputBuffer->size);
    }

    return newRunSize;
}

// merge过程
void BestMergeSequence() {
    int cost = 0;
    int size = runLength.size();
    
    // 初始化nodes
    for (int i = 0; i < size; ++i) {
        MergeNode node;
        node.weight = runLength[i];
        node.isMerged = false;
        node.startPosition = 0;
        for (int j = 0; j < i; ++j) {
            node.startPosition += runLength[j];
        }
        node.endPosition = node.startPosition + runLength[i];
        nodes.emplace_back(node);
    }

    // 设置inputBuffer, outputBuffer
    Buffer* inputBuffer = new Buffer[k];
    for (int i = 0; i < k; ++i)
        inputBuffer[i].reset(bufferSize);
    Buffer outputBuffer(bufferSize);

    int round = 0;

    while(nodes.size()>1) {
        
        sort(nodes.begin(), nodes.end(), sortStruct);// 按weight升序排列

        // merge前k个
        int currentK = (k > nodes.size()) ? nodes.size() : k; // 要归并的段数

        MergeNode newNode;
        newNode.startPosition = 0;
        newNode.endPosition = Merge(inputBuffer, &outputBuffer, round, currentK);
        newNode.weight = 0;
        
        // 把未merge的部分抄下来
        nodes[currentK - 1].endPosition = newNode.endPosition;
        for (int i = currentK; i < nodes.size(); ++i) {
            Copy(round, i);
            //更新run在文件中的位置
            int runSize = nodes[i].endPosition - nodes[i].startPosition;
            nodes[i].startPosition = nodes[i - 1].endPosition;
            nodes[i].endPosition = nodes[i].startPosition + runSize;
        }

        // newNode加入nodes列表
        for (int i = 0; i < currentK; ++i) {
            newNode.weight += nodes[0].weight;
            cost += nodes[0].weight;
            nodes.erase(nodes.begin());
        }

        nodes.emplace_back(newNode);

        round++;
    }
}

// 生成Input.txt
void GenerateData(int dataSize, string filepath ) {
    fstream fout;
    fout.open(filepath, ios::out | ios::app);
    // 文件无法打开报错
    if (!fout.is_open())
    {
        cout << "Error" << endl;
        return;
    }
    // 1-1000的随机数
    ostringstream oss;
    for (int i = 0; i < dataSize; ++i) {
        oss << rand() % 1000 << "\n";
    }   
    string outString = oss.str();
    fout << outString;
    fout.close();
}


int main()
{
    File file1;
    cout << "Input data size: ";
    cin >> dataSize;
    
    cout << "Input k (ways to merge) : ";
    cin >> k;

    GenerateData(dataSize, "Input.txt");

    int round = 0;

    //记录时间
    LARGE_INTEGER t1, t2, tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);

    // 生成initialRuns
    ImprovedCreateInitialRuns();

    // merge
    BestMergeSequence();

    QueryPerformanceCounter(&t2);
    double time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
    cout << "time = " << time << endl;  //输出时间（单位：ｓ）
    
}

