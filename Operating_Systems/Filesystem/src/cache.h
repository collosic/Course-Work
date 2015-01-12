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
#define EMPTY_LOC -1

// Type Definitions
typedef std::vector<std::string> vecstr;
typedef unsigned char byte;


class OFT {
  private:
    byte buffer[BLOCK_LENGTH];
    int current_pos;
    int index;
    int length;
    bool isEmpty;

  public:
    OFT();
    
    void clearDirectory(const byte *mem);
    byte* getBuf() { return buffer; };
};

class Memory {
  private:
    byte memory_blks[NUM_BLOCKS][BLOCK_LENGTH];
    byte slot;
    byte offset;
    
    // Private Functions
    void setBit(byte *num, byte x) { *num |= 1 << x; }
  
  public:
    Memory();
    
    void initMemory();  
    void clearBlock(int block_num);
    void setBitMapLocation(byte bit_loc);
    void createDirectory();
    void setDescriptorEntry(int block, int offset, int length);
    void setDescriptorBlock(int desc_blk_num, int desc, int block_num_req, int file_index);

    inline byte* getBlk(int blk_num) { return memory_blks[blk_num]; };
    int getDescriptorIndexLocation(int desc_index, int file_index);
    int getFileIndex(int blk_num, int desc_index, int file_index);
};

class Pack {
  private:
    byte pack_mem[INT_SIZE];
    const int MASK;

  public:
    Pack() : MASK(0xFF) {} ;

    void intToBytes(byte *block, int offset, int val);  
};

class UnPack {
  private:
    int num;
    const int MASK;

  public:
    UnPack() : MASK(0xFF) {};

    int bytesToInt(const byte *mem_loc, int start_loc);
};

#endif
