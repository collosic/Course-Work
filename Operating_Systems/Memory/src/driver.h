#ifndef DRIVER_H
#define DRIVER_H   
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// Header Files
#include "manager.h"

// Constants
#define NUM_COMMANDS 11

// Typedef
typedef std::vector<std::string> vecstr;

class Driver {
  private:
    std::map<std::string, int> map;
    bool quit;

    // Data Structures required for the driver
    Manager *manager;

  public:
    // Constructors
    Driver();
    ~Driver();

    // Class Methods
    std::string interface(vecstr *in); 
    bool getQuit() { return quit; };
};
#endif
