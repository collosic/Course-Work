#ifndef FILESYSTEM_H
#define FILESYSTEM_H 

#include <cstdlib>
#include <string>
#include <vector>
#include <iterator>
#include <unordered_map>

#include "disk.h"
#include "cache.h"

// Constants
#define OFT_SIZE 4
#define MEM_DIR_BLK 1
#define OFT_DIR_BLK 0

// Type Definitions
typedef std::vector<std::string> vecstr;



class FileSystem {
  private:
    Disk disk;
    Memory mem;
    OFT oft[OFT_SIZE];
    UnPack *unpack;
    std::string response;
    bool isInitialized;

    // Private Functions
    
  public:
    // Constructors        
    FileSystem() : isInitialized(false) {}; 

    // Class Methods
    void setResponse(std::string r) { response = r; };
    std::string getResponse() { return response; };
    void initDisk(vecstr *in);    
    void createDisk();
    void loadDisk(std::string file_name);
    void createFile(vecstr *in);
    void deleteFile(vecstr *in);
    void openFile(vecstr *in);
    int findAvailableDescriptorSlot();
    inline bool checkInitialization() { return isInitialized; };
    void closeFile(vecstr *in);
};


#endif
