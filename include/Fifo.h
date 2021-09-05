#pragma once
#include <queue>
#include <mutex>
#include<map>
using namespace std;

template<typename TYPE1, typename TYPE2>class Fifo{
    private:
        queue<pair<TYPE1, TYPE2>> q;
    public:
        bool IsEmpty(){
            lock_guard<std::mutex> lock(this->fifo_mtx);
            return this->q.empty();
        }
        pair<TYPE1, TYPE2> Pop(){
            pair<TYPE1, TYPE2> element;
            element.first = 0xFFFFFFFF;
            element.second = 0xFF;
            if(this->IsEmpty()){
                return element;
            }
            lock_guard<mutex> lock(this->fifo_mtx);
            element = this->q.front();
            this->q.pop();
            return element;
        }
        void Push(TYPE1 irq_num, TYPE2 data){
            pair<TYPE1, TYPE2> element;
            element.first = irq_num;
            element.second = data;
            lock_guard<mutex> lock(this->fifo_mtx);
            this->q.push(element);
            return;
        }
        mutex fifo_mtx;
};