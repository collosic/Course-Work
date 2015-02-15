#ifndef DRIVER_H
#define DRIVER_H   
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// Header Files

// Constants
#define NUM_COMMANDS 11

// Typedef
typedef std::vector<std::string> vecstr;


class Driver {
  private:
    //std::string commands[NUM_COMMANDS];
    std::map<std::string, int> map;
    bool quit;

  public:
    // Constructors
    Driver();
    ~Driver();

    // Class Methods
    std::string interface(vecstr *in); 
    bool getQuit() { return quit; };
};








#endif
