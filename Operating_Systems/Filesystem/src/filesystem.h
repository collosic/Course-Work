#ifndef FILESYSTEM_H
#define FILESYSTEM_H 

#include <string>
#include <vector>
#include <iterator>

#include "disk.h"
#include "cache.h"



// Type Definitions
typedef std::vector<std::string> vecstr;



class FileSystem {
  private:
    Disk *disk;
    Memory mem;
    std::string response;

    // Private Functions
    
  public:
    // Constructors        
    FileSystem();

    // Class Methods
    void setResponse(std::string r) { response = r; };
    std::string getResponse() { return response; };
    void initDisk(vecstr *in);    
    void createDisk();
    void loadDisk(std::string file_name);

};


#endif
