#include "driver.h" 

// Constructors initialization

Driver::Driver() {
    // Initialize a FileSystem
    FileSystem fs;
    fs.setResponse("Invalid command entered");

    // Initialize the accepted commands
    std::string tmp[] = {"cr", "de", "op", "cl", "rd", "wr", "sk", "dr", "in", "sv"};
    for (int i = 0; i < NUM_COMMANDS; ++i) {
        commands[i] = tmp[i];        
    }
}

std::string Driver::interface(vecstr *in) {
    std::string command = in->front();
    int value = -1;
    
    // remove the first element that was extracted into command
    in->erase(in->begin());
     
    for (int i = 0; i < NUM_COMMANDS; ++i) {
        if(command.compare(commands[i]) == 0) {
            value = i + 1;
        }
    }
    std::string response;
    switch (value) {
        case 1:     response = "Good work"; 
                    break;
        case 2:     response = "Good";
                    break;
        case 3:     response = "op";
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

