#include "driver.h" 

// Constructors initialization

Driver::Driver() {
    // Initialize a FileSystem
    fs = new FileSystem();
    fs->setResponse("Invalid command entered");

    // Initialize the accepted commands
    std::string tmp[] = {"cr", "de", "op", "cl", "rd", "wr", "sk", "dr", "in", "sv"};
    for (int i = 0; i < NUM_COMMANDS; i++) {
        //map[tmp[i]] = i + 1;            
        commands[i] = tmp[i];
    }
}

Driver::~Driver() {
}


std::string Driver::interface(vecstr *in) {
    fs->setResponse("");
    std::string command = in->front();
    int value = 0;    
    // check and see if the FS has been initialized if the command is not "in"
    if (command.compare("in") != 0) {
        if (fs->checkInitialization() == false) {
            in->clear();
            return "you must first initialize a disk";
        }
    }

    // remove the first element that was extracted into command
    in->erase(in->begin());
    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (commands[i].compare(command) == 0) {
            value = i + 1;
            break;
    }
    } 

    
    // we need to evaluate the command and then determine if the user has typed
    // in too many arguments for each command
    switch (value) {
        case 1:     in->size() != 1 ? fs->wrongArg(command) : fs->createFile(in); 
                    break;
        case 2:     in->size() != 1 ? fs->wrongArg(command) : fs->deleteFile(in); 
                    break;
        case 3:     in->size() != 1 ? fs->wrongArg(command) : fs->openFile(in); 
                    break;
        case 4:     in->size() != 1 ? fs->wrongArg(command) : fs->closeFile(in); 
                    break;
        case 5:     in->size() != 2 ? fs->wrongArg(command) : fs->read(in); 
                    break;
        case 6:     in->size() != 3 ? fs->wrongArg(command) : fs->write(in); 
                    break;
        case 7:     in->size() != 2 ? fs->wrongArg(command) : fs->seek(in); 
                    break;
        case 8:     in->size() != 0 ? fs->wrongArg(command) : fs->listDirectory();
                    break;
        case 9:     in->size() > 1 ? fs->wrongArg(command) : fs->initDisk(in);
                    break;
        case 10:    in->size() != 1 ? fs->wrongArg(command) : fs->save(in);
                    break;
        default:    fs->setResponse("Invalid command entered");
                    break;
    }
    in->clear();
    return fs->getResponse();
}    

