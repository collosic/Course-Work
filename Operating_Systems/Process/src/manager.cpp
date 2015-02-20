// manager.cpp

#include "manager.h"

void Manager::initResources() {
    // create each resource
    R1 = new RCB(RESOURCES::R1, 1);
    R2 = new RCB(RESOURCES::R2, 2);
    R3 = new RCB(RESOURCES::R3, 3);
    R4 = new RCB(RESOURCES::R4, 4);

    // place all resources in a hash map for easy lookup
    resources["R1"] = R1;
    resources["R2"] = R2;
    resources["R3"] = R3;
    resources["R4"] = R4;

    res_to_str[RESOURCES::R1] = "R1";
    res_to_str[RESOURCES::R2] = "R2";
    res_to_str[RESOURCES::R3] = "R3";
    res_to_str[RESOURCES::R4] = "R4";

}


/* This function will destroy any previous creation tree, if it exists
 * and then create a brand new init process at level 0. */
std::string Manager::initialize() {
    // first destroy the current init process and its children
    // second recreate the ready and blocked lists
    if (init_proc != nullptr) 
        killAll();
    // init the resources
    initResources();

    // generate a brand new process and resource environment
    init_proc = running = self = nullptr;
    ready_list = new vecproc[NUM_PRIORITIES];
    isInit = false;
    vecstr params = {"init", "0"};
    create(&params);
    isInit = true;       

    // Now create a new process called init at level 0
    return "\n\n" + running->getPID();
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
        return "error(duplicate process name: " + name + ")";
    }

    if (!is_digits(priority)) {
        return "error(priority paramater is not a digiti)";
    } else {
        p_level = std::stoi(priority);
        if (p_level == 0 && isInit) {
            return "error(priority 0 only used by init process)";
        } 
        if (p_level < 0 || p_level > 2) {
            return "error(out of range priority level: " + priority + ")";
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
       // link to the parent here
       new_PCB->setParent(running);
    } 

    processes.insert(std::pair<std::string, PCB*>(name, new_PCB));
    ready_list[p_level].push_back(new Proc(new_PCB, 0));
    new_PCB->setTypeList(&ready_list[p_level]);
    self = running;
    // finally run the scheduler to determin which process should be running
    return  scheduler(); 
}

std::string Manager::destroy(vecstr *arg) {
    // recursively destroy all child PCB's and then kill this current PCB
    std::string name = arg->front();
    if (!is_printable(name)) 
        return "invalid name of a process";  
    
    // deleting the init proc will not be allowed unless init is called
    if (name == "init")
       return "error(cannot delete process: init)"; 

    // if the process does not exist return to the caller
    if (processes.find(name) == processes.end()) 
        return "error(non-existent process: " + name + ")";
    
    // we know the process exists so we need to find it and extract the PCB
    std::map<std::string, PCB*>::iterator proc = processes.find(name);
    PCB *p = proc->second;
    killSelf(p);
    self = running;
    return scheduler();
}

std::string Manager::request(vecstr *args) {
    // extract and anaylze the arguments
    std::string re = args->front();
    args->erase(args->begin());
    std::string units = args->front();
    
    if (init_proc == nullptr)
        return "error(init proc not running)";

    if (!is_printable(re)) 
        return "error(invalid resource name: " + re + ")";

    if (!is_digits(units)) 
        return "error(need number of resources)";
    
    if (running == init_proc) 
       return "error(init cannot request or release resources)";

    int num_req = std::stoi(units);
    // determine what kind of resource is requested 
    if (resources.find(re) == resources.end()) 
        return "error(non-existent resource: " + re + ")";

    std::map<std::string, RCB*>::iterator res = resources.find(re); 
    RCB *r = res->second;

    // now begin analysis of resources
    int available = r->getAvailableUnits();
    int max = r->getMaxUnits();
   
    // check to see if units requested is larger than max available
    if (num_req > max) {
        return "error(request too many units: " + units + "/" + 
                                res_to_str.find(r->getRID())->second + ")";
    }
    
    // need to check if the running process already has the reqeusted resource
    OtherResources* other = running->checkResources(r);
    int num_holding;
    if (other != nullptr) {
        num_holding = other->getUnits();
        if (num_req + num_holding <= max && num_req <= available) {
            other->setNumUnits(num_req + num_holding);
            return running->getPID();
        }
    }

    if (num_req <= available) {
        r->setAvailableUnits(available - num_req);
        running->insertResources(new OtherResources(r, num_req));
    } else {
        self = running;
        running = nullptr;
        removeFromList(self);
        self->setType(STATE::BLOCKED);        
        self->setTypeList(r->getWaitList());
        r->insertWaiting(new Proc(self, num_req));
        scheduler();
    }
    return running->getPID();
}


std::string Manager::release(vecstr *args) {
    // extract and anaylze the arguments
    std::string rel = args->front();
    args->erase(args->begin());
    std::string units = args->front();

    if (running == init_proc)
        return "error(init cannot request or release resources)";
    
    if (init_proc == nullptr)
        return "error(no process running)"; 

    if (!is_printable(rel)) 
        return "error(not a valid resource)";

    if (!is_digits(units)) 
        return "error(need number of resources)";

    int num_rel = std::stoi(units);
    if (num_rel < 1) 
        return "error(invalid number of resources: " + units + ")";
    
    // determine what kind of resource is requested 
    if (resources.find(rel) == resources.end()) 
        return "error(non-existent resource: " + rel + ")";
    
    std::map<std::string, RCB*>::iterator release = resources.find(rel); 
    RCB *r = release->second;

    self = running;
    std::string s = releaseRes(self, r, num_rel);
    if (s != "") return s;
    return scheduler();
}


std::string Manager::timeout() {
    if (running != nullptr) {
        vecproc *list = &ready_list[running->getPriority()];
        PCB *p;
        if (list->size() > 1 ) {
            p = list->at(1)->getPCB();
            preempt(p);
            return running->getPID();
        }
        return running->getPID(); 
    }
    // this should never happen since there should always be a process running
    return "no process is running";
}


std::string Manager::killAll() {
    if (init_proc != nullptr) 
        killSelf(init_proc);
    
    delete[] ready_list;
    delete R1;
    delete R2;
    delete R3;
    delete R4;
    return "Good Bye";
}


std::string Manager::scheduler() {
    // check the highest level priority until a process is found
    for (int i = NUM_PRIORITIES - 1; i >= 0; i--) {
        if(!ready_list[i].empty()) {
            PCB *p = ready_list[i].front()->getPCB();
            if (self == nullptr || self->getPriority() < p->getPriority() || 
                    self->getType() != STATE::RUNNING) {
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
        if (running->getPriority() == new_running->getPriority()) {
            vecproc *list = &ready_list[running->getPriority()];
            list->push_back(list->front());
            list->erase(list->begin());
            running->setState(STATE::READY);
        }
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
        freeResources(child);

        // remove from type list and lookup and delete
        removeFromList(child); 
        processes.erase(child->getPID());
        if (child == running) 
            running = nullptr;
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
        vecpcb::iterator it = find(plist->begin(), plist->end(), p);
        auto pos = std::distance(plist->begin(), it);
        parent->removeChildAt(pos);
    }
    // free resources
    freeResources(p);

    // remove from Type List and delete
    processes.erase(p->getPID());
    removeFromList(p);
    
    // properly determine which pointers require null
    if (p == running) 
        running = nullptr;
    if (p == init_proc) 
        init_proc = nullptr;
    self = nullptr;
    delete p;
}


void Manager::removeFromList(PCB *p) {
    // using the PCB's own type list remove it from that list
    vecproc *list = p->getTypeList();
    for (unsigned long i = 0; i < list->size(); i++) {
        PCB *s = list->at(i)->getPCB();
        if (p == s) {
            list->erase(list->begin() + i);
            break;
        }
    }
}


void Manager::freeResources(PCB *p) {
    // find all resources and release them back to the pool
    std::vector<OtherResources*> *resources = p->getResources();
    int size = resources->size();
    for (int i = 0; i < size; i++) {
        OtherResources *o = resources->front();   
        RCB *r = o->getResource();
        int units = o->getUnits();
        releaseRes(p, r, units);
    }
}

std::string Manager::releaseRes(PCB *p, RCB* r, int num_rel) {
    OtherResources* other = p->checkResources(r);
    if (other == nullptr) 
        return "error(not holding resource: " + res_to_str.find(r->getRID())->second + ")";
    
    // verify the number of units being released is no more than the number of units
    // this process is holding.    
    int units_holding = other->getUnits();
    if (num_rel > units_holding) {
        return "error(release too many units: " + std::to_string(num_rel) + "/" +
                res_to_str.find(r->getRID())->second + ":" + std::to_string(units_holding) + ")";
    }

    int units_left = other->releaseResources(num_rel);
    if (units_left == 0) 
        p->removeResources(r);

    // release the units from the resource and check the waiting liiist
    r->release(num_rel); 
    vecproc *wait_list = r->getWaitList(); 
    while (!wait_list->empty()) {
        // check and see if we can satisfy the processes in the queue
        Proc *next = wait_list->front();
        int available = r->getAvailableUnits();
        int needed = next->getUnits();
        if (needed > available) 
            break;
        
        r->setAvailableUnits(available - needed);
        
        // remove the process from the waiting list
        wait_list->erase(wait_list->begin());
        PCB *q = next->getPCB();
        q->setType(STATE::READY);
        q->setTypeList(&ready_list[q->getPriority()]);
        q->insertResources(new OtherResources(r, needed));
        ready_list[q->getPriority()].push_back(new Proc(q, needed));
    }
    return "";
}

bool Manager::is_digits(const std::string &str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
}


bool Manager::is_printable(const std::string &print) {
    return std::all_of(print.begin(), print.end(), ::isprint);
}

