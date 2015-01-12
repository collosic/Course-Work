#ifndef DRIVER_H
#define DRIVER_H   
#include <iostream>
#include <sstream>

// Header Files
#include "filesystem.h"

// Constants
#define NUM_COMMANDS 10

class Driver {
  private:
    FileSystem fs;
    std::string commands[NUM_COMMANDS];

  public:
    // Constructors
    Driver();

    // Class Methods
    std::string interface(std::vector<std::string> *in); 

};








#endif
