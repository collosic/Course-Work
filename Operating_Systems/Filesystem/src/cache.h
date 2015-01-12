#ifndef CACHE_H
#define CACHE_H 

#include <string>
#include <vector>
#include <iterator>

#include "disk.h"

// Constants
#define CACHE_BLK_SIZE 7
#define NUM_BLOCKS 64
#define BLOCK_LENGTH 64
#define BITMAP_BLK 0
#define SLOT_SIZE 8
#define INT_SIZE 4
#define DESCRIPTOR_SIZE 16

// Type Definitions
typedef std::vector<std::string> vecstr;
typedef unsigned char byte;

// Function declarations

class OFT {
  private:

  public:
    OFT();
};

class Memory {
  private:
    Disk disk;
    byte memory_blks[NUM_BLOCKS][BLOCK_LENGTH];
    byte slot;
    byte offset;

    void setBit(byte *num, byte x) { *num |= 1 << x; }
  
  public:
    Memory();
    
    void initMemory();  
    void clearBlock(int block_num);
    void setBitMapLocation(byte bit_loc);
    void createDirectory();
    void setDescriptorLength(int block, int offset, int length);
    void setDescriptorBlock(int desc_blk_num, int desc, int block_num_req, int file_index);
};

class Pack {
  private:
    byte mem[INT_SIZE];
    const int MASK;

  public:
    Pack() : MASK(0xFF) {}

    void intToBytes(int val);  
    void extractPackedInteger(byte *arr);
};

#endif
