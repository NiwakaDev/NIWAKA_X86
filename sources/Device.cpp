#include "Device.h"

Device::Device(){
    return;
}

bool Device::IsEnable(){
    this->Error("Stopped at Device::IsEnable");
}

void Device::ShowSelf(){
    
}

int Device::IsIrq(){
    return -1;
}