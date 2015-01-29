#ifndef DRIVER_H
#define DRIVER_H   
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

// Header Files
#include "filesystem.h"

// Constants
#define NUM_COMMANDS 10



class Driver {
  private:
    FileSystem fs;
    std::string commands[NUM_COMMANDS];
    std::unordered_map<std::string, int> map;

  public:
    // Constructors
    Driver();
    ~Driver();

    // Class Methods
    std::string interface(vecstr *in); 

};








#endif
