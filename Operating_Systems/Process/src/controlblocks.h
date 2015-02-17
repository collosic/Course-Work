#ifndef CONTROLBLOCKS_H
#define CONTROLBLOCKS_H 

#include <string>
#include <vector>
#include <queue>

// Enum classes
enum class STATE {RUNNING, READY, BLOCKED};
enum class RESOURCES {R1, R2, R3, R4};

// Constants
const int NUM_PRIORITIES = 3;

class OtherResources {
  private:
    RESOURCES resource;
    int num_units;

  public:
    void set_resource(RESOURCES res) { resource = res; };
    void set_num_units(int value) { num_units = value; };
};

class PCB {
  private:
    std::string PID;
    std::vector<OtherResources> other_resources;
    STATE type;
    std::vector<std::queue<PCB*>> type_list;
    PCB *parent;
    std::vector<PCB*> child;
    int priority;

  public:
    PCB (std::string name, STATE state, PCB *parent, int p_level);
    ~PCB ();
    
    // funciton for changing the PCB's properties
    void setState(STATE state) { type = state; };
    void insertChild(PCB *kid) { child.push_back(kid); };
    int getPriority() { return priority; };
    std::string getPID() { return PID; };
    std::vector<PCB*>* getChildren() { return &child; };
    void popChild() { child.erase(child.begin()); };
};

class RCB {
  private:
    RESOURCES RID;
    int k;
    int u;
    // need a queue of PCB's waiting for resources
    std::queue<PCB*> waiting_list;

  public:
    RCB ();
    ~RCB ();
};
#endif
