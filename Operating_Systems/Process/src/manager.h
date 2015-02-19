#ifndef MANAGER_H
#define MANAGER_H

#include <map>
#include <algorithm>

#include "controlblocks.h"

typedef std::vector<std::string> vecstr;
typedef std::vector<PCB*> pcb_q;
typedef decltype(nullptr) nullptr_t;


class Manager {
  private:
    vecpcb *ready_list;
    vecpcb *blocked_list;
    PCB *init_proc;
    PCB *running;
    PCB *self;

    // Resources: each Ri holds i resources 
    RCB *R1;
    RCB *R2;
    RCB *R3;
    RCB *R4;

    // hashmaps to store names of processes and resources
    std::map<std::string, PCB*> processes;
    std::map<std::string, int> resources;
    
    // this variable is used to test if the init process is has already 
    // been created.  Priority level 0 can only be used once.
    bool isInit;

  public:
    Manager() : init_proc(nullptr), running(nullptr), self(nullptr) { initialize(); };
    ~Manager() { killAll(); };
    
   std::string initialize();
   std::string create(vecstr *args);
   std::string destroy(vecstr *arg);
   std::string request(vecstr *args);
   std::string release(vecstr *args);
   std::string timeout();
   std::string killAll();
   std::string scheduler();
   
   // These functions verify if a string is a valid digit or a printable ascii char
   bool is_digits(const std::string &str);
   bool is_printable(const std::string &print);

   // These functions return string responses to the caller
   std::string running_resp(std::string name) { return "*" + name + " is running"; };
   std::string notFound(std::string name) { return "process " + name + " does not exist"; };

   // Helper functions to make things easier to read and follow
   void preempt(PCB *p);
   void killTree(PCB *p);
   void killSelf(PCB *p);
   void removeFromList(PCB *p);
   void remove(pcb_q *list, PCB *p);
   std::string listProcs();
   std::string procInfo(vecstr *in);

};


#endif
