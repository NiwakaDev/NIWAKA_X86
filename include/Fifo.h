#pragma once
#include <queue>
#include <mutex>
#include<map>
using namespace std;

class Fifo{
    private:
        queue<pair<unsigned int, unsigned char>> q;
    public:
        Fifo();
        pair<unsigned int, unsigned char> Pop();
        void Push(unsigned int irq_num, unsigned char data);
        bool IsEmpty();
        mutex fifo_mtx;
};