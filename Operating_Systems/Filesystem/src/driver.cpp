#include "driver.h" 

// Constructors initialization

Driver::Driver() {
    // Initialize a FileSystem
    FileSystem fs;
    fs.setResponse("Invalid command entered");

    // Initialize the accepted commands
    std::string tmp[] = {"cr", "de", "op", "cl", "rd", "wr", "sk", "dr", "in", "sv"};
    for (int i = 0; i < NUM_COMMANDS; i++) {
        map[tmp[i]] = i + 1;            
    }
}

std::string Driver::interface(vecstr *in) {
    std::string command = in->front();
    
    // check and see if the FS has been initialized if the command is not "in"
    if (command.compare("in") != 0) {
        if (fs.checkInitialization() == false) return "you must first initialize a disk";
    }

    // remove the first element that was extracted into command
    in->erase(in->begin());
     
    std::unordered_map<std::string, int>::const_iterator value = map.find(command);
    std::string response;
 
    switch (value->second) {
        case 1:     fs.createFile(in); 
                    break;
        case 2:     fs.deleteFile(in); 
                    break;
        case 3:     fs.openFile(in); 
                    break;
        case 4:     response = "";
                    break;
        case 5:     response = "";
                    break;
        case 6:     response = "";
                    break;
        case 7:     response = "";
                    break;
        case 8:     response = "";
                    break;
        case 9:     fs.initDisk(in);
                    break;
        case 10:    response = "";
                    break;
        default:    response = "Invalid command entered";
                    break;
    }
    in->clear();
    return fs.getResponse();
}    

