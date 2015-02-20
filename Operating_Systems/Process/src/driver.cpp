#include "driver.h" 

// Constructors initialization

Driver::Driver() {
    // Initialize the accepted commands
    quit = false;
    std::string tmp[] = {"init", "cr", "de", "req", "rel", "to", "quit", "lp", "lr", "pp", "pr"};
    for (int i = 0; i < NUM_COMMANDS; i++) {
        map[tmp[i]] = i + 1;            
        //commands[i] = tmp[i];
    }

    // Create a manager
    manager = new Manager();
}

Driver::~Driver() {
    if (manager != nullptr)
        delete manager;
    manager = nullptr;
}


std::string Driver::interface(vecstr *in) {
    std::string command = in->front();
    // check and see if the system has been initialized if the command is not "init"
    if (command.compare("init") != 0) {
        
    }
    // remove the first element that was extracted into command
    in->erase(in->begin());
    std::map<std::string, int>::iterator value = map.find(command);
        
    // we need to evaluate the command and then determine if the user has typed
    // in too many arguments for each command
    std::string response;
    switch (value->second) {
        case 1:     response = in->size() > 0 ? "init takes no arguments" : manager->initialize();
                    break;
        case 2:     response = in->size() == 2 ? manager->create(in) : "invalid use of cr";
                    break;
        case 3:     response = in->size() == 1 ? manager->destroy(in) : "invalid use of de";
                    break;
        case 4:     response = in->size() == 2 ? manager->request(in) : "invalid use of req";
                    break;
        case 5:     response = in->size() == 2 ? manager->release(in) : "invalid use of rel";
                    break;
        case 6:     response = in->size() == 0 ? manager->timeout() : "invalid use of to"; 
                    break;
        case 7:     if (manager != nullptr)
                        delete manager;
                    manager = nullptr;
                    quit = true;
                    response ="Good bye";
                    break;
        case 8:     response = manager->listProcs();
                    break;
        case 9:     
                    break;
        case 10:    response = manager->procInfo(in); 
                    break;
        case 11:    
                    break;
        default:    response = "error(invalid command: " + command + ")"; 
                    break;
    }
    in->clear();
    return response;
}    

