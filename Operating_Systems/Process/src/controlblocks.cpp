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
}

OtherResources::OtherResources(RCB *r, int units) {
    res = r;
    num_units = units;
}

OtherResources* PCB::checkResources(RCB *r) {
   for (unsigned long i = 0; i < other_resources.size(); i++) {
        OtherResources *o_r = other_resources[i];
        if (o_r->getResource() == r) 
            return other_resources[i];         
   } 
   return nullptr;
}
