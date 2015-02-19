#ifndef CONTROLBLOCKS_H
#define CONTROLBLOCKS_H 

#include <string>
#include <vector>

// Call definitions
class Proc;
class OtherResources;
class PCB;
class RCB;

// Enum classes
enum class STATE {RUNNING, READY, BLOCKED};
enum class RESOURCES {R1, R2, R3, R4};

// Constants
const int NUM_PRIORITIES = 3;

// Type definitions
typedef std::vector<PCB*> vecpcb;
typedef std::vector<Proc*> vecproc;
typedef decltype(nullptr) nullptr_t;

class Proc {
  private:
    PCB *p;
    int units;

  public:
    Proc (PCB *s, int u) : p(s), units(u) {};
    ~Proc () {};

    PCB* getPCB() { return p; };
    int getUnits() { return units; };
};

class OtherResources {
  private:
    RCB *res;
    int num_units;

  public:
    OtherResources(RCB *r, int units);
    ~OtherResources() {};

    void setResource(RCB *r) { res = r; };
    void setNumUnits(int value) { num_units = value; };
    int getUnits() { return num_units; };
    RCB* getResource() { return res; };
    int releaseResources(int r) { return num_units -= r; };

};

class PCB {
  private:
    std::string PID;
    std::vector<OtherResources*> other_resources;
    STATE type;
    vecproc *type_list;
    PCB *parent;
    vecpcb child;
    int priority;

  public:
    PCB (std::string name, STATE state, int p_level);
    ~PCB ();
    
    // funciton for changing the PCB's properties
    void setState(STATE state) { type = state; };
    void setType(STATE s) { type = s; };
    void setTypeList(vecproc *l) { type_list = l; };
    STATE getType() { return type; };
    vecproc* getTypeList() { return type_list; };
    void insertChild(PCB *kid) { child.push_back(kid); };
    void setParent(PCB *p) { parent = p; };
    int getPriority() { return priority; };
    STATE getState() { return type; };
    std::string getPID() { return PID; };
    vecpcb* getChildren() { return &child; };
    PCB* getParent() { return parent; };
    void removeChildAt(int p) { child.erase(child.begin() + p); };
    std::vector<OtherResources*>* getResources() { return &other_resources; };
    OtherResources* checkResources(RCB *r);
    void insertResources(OtherResources *o) { other_resources.push_back(o); };
    void removeResources(RCB *r);

};

class RCB {
  private:
    RESOURCES RID;
    int k;
    int u;
    // need a queue of PCB's waiting for resources
    vecproc wait_list;

  public:
    RCB (RESOURCES name, int count);
    ~RCB ();

    // functions to access the properties of an RCB
    RESOURCES getRID() { return RID; };
    int getMaxUnits() { return k; };
    int getAvailableUnits() { return u; };
    vecproc* getWaitList() { return &wait_list; };
    void setAvailableUnits(int r) { u = r; };
    void insertWaiting(Proc *w) { wait_list.push_back(w); };
    void release(int i) { u += i; };
};
#endif
