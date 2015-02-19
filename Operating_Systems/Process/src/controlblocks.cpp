#include "controlblocks.h"

PCB::PCB(std::string name, STATE state, int p_level) {
    PID = name;
    type = state;
    priority = p_level;
}

PCB::~PCB() {
    // delete all the resources in the PCB
    for (unsigned long i = 0; i < other_resources.size(); i++) {
        delete other_resources[i]; 
    }
}

RCB::RCB(RESOURCES name, int count) {
    RID = name;
    k = u = count;
}

RCB::~RCB() {
    for (unsigned long i = 0; i < wait_list.size(); i++) {
        delete wait_list.at(i); 
    }
}

OtherResources::OtherResources(RCB *r, int units) {
    res = r;
    num_units = units;
}

OtherResources* PCB::checkResources(RCB *r) {
   for (unsigned long i = 0; i < other_resources.size(); i++) {
        OtherResources *o_r = other_resources[i];
        if (o_r->getResource() == r) 
            return o_r;
   } 
   return nullptr;
}

void PCB::removeResources(RCB *r) {
    for (unsigned long i = 0; i < other_resources.size(); i++) {
        OtherResources *o_r = other_resources[i];
        if (o_r->getResource() == r) { 
            delete o_r;
            other_resources.erase(other_resources.begin() + i); 
        }
    }
}
