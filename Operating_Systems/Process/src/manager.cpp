#include "manager.h"


Manager::Manager() {
    // This constructor should initialize a brand new init process
    ready_list = new pcb_q[NUM_PRIORITIES];
    blocked_list = new pcb_q[NUM_PRIORITIES];
    isInit = false;
    init_proc = running = self = nullptr;
    vecstr params = {"init", "0"};
    create(&params);
    isInit = true;
}

Manager::~Manager() {
    quit();
}


/* This function will destroy any previous creation tree, if it exists
 * and then create a brand new init process at level 0. */

std::string Manager::initialize() {
    // first destroy the current tree if it exists
    

    // Now create a new process called init at level 0
    return "*init process created";
}

std::string Manager::create(vecstr *args) {
    // extract and verify the arguments
    std::string name = args->front();
    args->erase(args->begin());
    std::string priority = args->front();
    args->erase(args->begin());
    int p_level;
    
    // run checks for valid process name, valid arguments and duplicate names
    if (!is_printable(name)) {
        return "not a valid name";
    }

    if (processes.find(name) != processes.end()) {
        return "process *" + name + " already exists";  
    }

    if (!is_digits(priority)) {
        return "priotiy paramater is not a digit";
    } else {
        p_level = std::stoi(priority);
        if (p_level == 0 && isInit) {
            return "priority 0 can only be used by init";
        } 
        if (p_level < 0 || p_level > 2) {
            return "invalid priority level";
        }
    }
    
    // create the new PCB and initialize it with the proper parameters
    PCB *new_PCB = new PCB(name, STATE::READY, p_level);
    
    // link the new process to the creation tree and insert into the ready list
    if (init_proc == nullptr) {
       init_proc = new_PCB;
       init_proc->setState(STATE::RUNNING);
       init_proc->setParent(nullptr);
       self = running = init_proc;
    } else {
       running->insertChild(new_PCB); 
       processes.insert(std::pair<std::string, PCB*>(name, new_PCB));
       // link to the parent here
       new_PCB->setParent(running);
    } 
    ready_list[p_level].push_back(new_PCB);
    new_PCB->setTypeList(&ready_list[p_level]);
    self = running;
    // finally run the scheduler to determin which process should be running
    name = scheduler();    
    return running_resp(name); 
}

std::string Manager::destroy(vecstr *arg) {
    // recursively destroy all child PCB's and then kill this current PCB
    std::string name = arg->front();
    if (!is_printable(name)) {
        return "invalid name of a process";  
    }
    
    // if the process does not exist return to the caller
    if (processes.find(name) == processes.end()) {
        return notFound(name) + ", " + running_resp(running->getPID());
    }
    // we know the process exists so we need to find it and extract the PCB
    std::map<std::string, PCB*>::iterator proc = processes.find(name);
    PCB *p = proc->second;
    killSelf(p);
    name = scheduler();
     
    return running_resp(name);
}

std::string Manager::request(vecstr *args) {

    return "blah";
}

std::string Manager::release(vecstr *args) {


    return "blah";
    
}

std::string Manager::timeout() {
    int priority = running->getPriority();
    if (!ready_list[priority].empty() && priority != 0) {
        ready_list[priority].erase(ready_list[priority].begin());
        preempt(ready_list[priority].front());
        return running_resp(running->getPID());
    } else {
       return "Only the init process exists, cannot time-out";
    } 
        
}
std::string Manager::quit() {
    killSelf(init_proc);
    delete[] ready_list;
    delete[] blocked_list;
    return "Good Bye";
}



std::string Manager::scheduler() {
    // check the highest level priority until a process is found
    PCB *p;
    for (int i = NUM_PRIORITIES - 1; i > 0; i--) {
        if(!ready_list[i].empty()) {
            p = ready_list[i].front();
            if (self == nullptr || self->getPriority() < p->getPriority() || self != p) {
                preempt(p);
            }
            break;
        }
    }
    return running->getPID();
}

void Manager::preempt(PCB *new_running) {
    // set the current running state to ready and push to the back of the queue
    if (running != nullptr) {
        pcb_q *list = &ready_list[running->getPriority()];
        list->push_back(running);
        list->erase(list->begin());
        running->setState(STATE::READY);
    }
    // we need to make the new_running process into the running process
    new_running->setState(STATE::RUNNING);
    running = new_running;
}

void Manager::killTree(PCB *p) {
    // for each child in this process run the kill tree function
    std::vector<PCB*> *kids = p->getChildren();
    for (auto &child : *kids) {
        killTree(child); 
        // free all resources
        
        removeFromList(child); 
        processes.erase(child->getPID());
        delete child;

    }
    // remove the child from the parent
    kids->clear(); 
}


void Manager::killSelf(PCB *p) {
    // first let's remove all the children of this process
    killTree(p);
    
    // remove p from its parent child's list and delete the PCB
    PCB* parent = p->getParent();
    if (parent != nullptr) {
        vecpcb *plist = parent->getChildren();
        vecpcb::iterator it;
        it = find(plist->begin(), plist->end(), p);
        auto pos = std::distance(plist->begin(), it);
        parent->removeChildAt(pos);
    }
    // remove from type list and delete
    removeFromList(p);
    if (p == running) {
        running = nullptr;
    }
    if (p == init_proc) {
        init_proc = nullptr;
    }
    processes.erase(p->getPID());
    delete p;
    self = nullptr;
}


void Manager::removeFromList(PCB *p) {
    // using the PCB's own type list remove it from that list
    std::vector<PCB*> *list = p->getTypeList();
    pcb_q::iterator it = find(list->begin(), list->end(), p);
    auto pos = std::distance(list->begin(), it);
    list->erase(list->begin() + pos);
}



bool Manager::is_digits(const std::string &str) {

    return std::all_of(str.begin(), str.end(), ::isdigit);
}


bool Manager::is_printable(const std::string &print) {
    return std::all_of(print.begin(), print.end(), ::isprint);
}


std::string Manager::listProcs() {
    std::string res;
    for (auto const& p : processes) {
        res += p.first + " ";
    }
    return res;
}

std::string Manager::procInfo(vecstr *in) {
    
    return "";
}

