#ifndef CONTROLBLOCKS_H
#define CONTROLBLOCKS_H 

#include <string>
#include <vector>
#include <queue>

// Call definitions
class PCB;

// Enum classes
enum class STATE {RUNNING, READY, BLOCKED};
enum class RESOURCES {R1, R2, R3, R4};

// Constants
const int NUM_PRIORITIES = 3;

// Type definitions
typedef std::vector<PCB*> vecpcb;

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
    vecpcb *type_list;
    PCB *parent;
    std::vector<PCB*> child;
    int priority;

  public:
    PCB (std::string name, STATE state, int p_level);
    ~PCB ();
    
    // funciton for changing the PCB's properties
    void setState(STATE state) { type = state; };
    void setTypeList(vecpcb *l) { type_list = l; };
    vecpcb* getTypeList() { return type_list; };
    void insertChild(PCB *kid) { child.push_back(kid); };
    void setParent(PCB *p) { parent = p; };
    int getPriority() { return priority; };
    STATE getState() { return type; };
    std::string getPID() { return PID; };
    std::vector<PCB*>* getChildren() { return &child; };
    PCB* getParent() { return parent; };
    void removeChildAt(int p) { child.erase(child.begin() + p); };
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
