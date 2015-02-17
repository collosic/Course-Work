#include "controlblocks.h"

PCB::PCB(std::string name, STATE state, PCB *parent, int p_level) {
    PID = name;
    type = state;
    parent = this->parent;
    priority = p_level;
}
PCB::~PCB() {
    delete this;
}

RCB::RCB() {

}
