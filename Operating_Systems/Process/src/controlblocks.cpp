#include "controlblocks.h"

PCB::PCB(std::string name, STATE state, int p_level) {
    PID = name;
    type = state;
    priority = p_level;
}
PCB::~PCB() {
    //delete this;
}

RCB::RCB() {

}
