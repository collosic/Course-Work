#ifndef DISK_H
#define DISK_H 

#include <string>
#include <fstream>

// Constants
#define NUM_BLOCKS 64
#define BLOCK_LENGTH 64

// Type Definitions
typedef unsigned char byte;

// Function Declarations
void clearBitmap();

// Classes
class Disk {
  private:
    char ldisk[NUM_BLOCKS][BLOCK_LENGTH];
    // Private Functions
  public:
    Disk();

    // Public Functions
    char* getBlkAddr(int blk_num) { return ldisk[blk_num]; };
    void read_block(int blk_num, char *p);
    void write_block(int blk_num, char *p);
    bool saveData(std::string data_name);
    bool loadData(std::string data_name);
};


#endif
