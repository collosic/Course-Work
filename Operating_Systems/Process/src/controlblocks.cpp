#include "controlblocks.h"

PCB::PCB(std::string name, STATE state, int p_level) {
    PID = name;
    type = state;
    priority = p_level;
}

PCB::~PCB() {
    // delete all the resources in the PCB
    int size = other_resources.size();
    for (int i = 0; i < size; i++) {
        delete other_resources[i]; 
    }
}

RCB::RCB(RESOURCES name, int count) {
    RID = name;
    k = u = count;
}

RCB::~RCB() {
    int size = wait_list.size();
    for (int i = 0; i < size; i++) {
        delete wait_list.at(i); 
    }
}

OtherResources::OtherResources(RCB *r, int units) {
    res = r;
    num_units = units;
}

OtherResources* PCB::checkResources(RCB *r) {
   int size = other_resources.size();
   for (int i = 0; i < size; i++) {
        OtherResources *o_r = other_resources[i];
        if (o_r->getResource() == r) 
            return o_r;
   } 
   return nullptr;
}

void PCB::removeResources(RCB *r) {
    int size = other_resources.size();
    for (int i = 0; i < size; i++) {
        OtherResources *o_r = other_resources[i];
        if (o_r->getResource() == r) { 
            delete o_r;
            other_resources.erase(other_resources.begin() + i); 
            break;
        }
    }
}
