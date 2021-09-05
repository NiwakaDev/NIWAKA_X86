#include "Fifo.h"

Fifo::Fifo(){

}

pair<unsigned int, unsigned char> Fifo::Pop(){
    pair<unsigned int, unsigned char> element;
    element.first = 0xFFFFFFFF;
    element.second = 0xFF;
    if(this->IsEmpty()){
        return element;
    }
    lock_guard<std::mutex> lock(this->fifo_mtx);
    element = this->q.front();
    this->q.pop();
    return element;
}

void Fifo::Push(unsigned int irq_num, unsigned char data){
    pair<unsigned int, unsigned char> element;
    element.first = irq_num;
    element.second = data;
    lock_guard<std::mutex> lock(this->fifo_mtx);
    this->q.push(element);
    return;
}

bool Fifo::IsEmpty(){
    lock_guard<std::mutex> lock(this->fifo_mtx);
    return this->q.empty();
}