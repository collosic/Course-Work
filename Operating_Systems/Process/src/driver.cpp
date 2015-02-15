#include "driver.h" 

// Constructors initialization

Driver::Driver() {
    // Initialize the accepted commands
    quit = false;
    std::string tmp[] = {"init", "quit", "cr", "de", "req", "rel", "to", "lp", "lr", "pp", "pr"};
    for (int i = 0; i < NUM_COMMANDS; i++) {
        map[tmp[i]] = i + 1;            
        //commands[i] = tmp[i];
    }
}

Driver::~Driver() {

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
    switch (value->second) {
        case 1:     return "init works"; 
                    break;
        case 2:     quit = true;
                    return "quiting program"; 
                    break;
        case 3:     
                    break;
        case 4:     
                    break;
        case 5:     
                    break;
        case 6:     
                    break;
        case 7:     
                    break;
        case 8:     
                    break;
        case 9:     
                    break;
        case 10:    
                    break;
        default:    return "no such command";
                    break;
    }
    in->clear();
    return "all is well";
}    

