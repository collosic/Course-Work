#ifndef CACHE_H
#define CACHE_H 

#include <string>
#include <cstring>
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
#define BIT_MASK_SIZE 32
#define MAX_NUM_BLKS 3

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
    
    void clearDirectory();
    byte* getBuf() { return buffer; };
    int findAvailableSlot();
    void setFile(int file_loc, int desc_index, std::string name);
    void seek(int index, int new_pos);

    inline void setCurrentPos(int pos) { current_pos = pos; };
    inline void setIndex(int i) { index = i; };
    inline void setLength(int l)  { length = l; };
    inline void setEmpty(bool flag) { isEmpty = flag; };
    inline int getCurrentPos() { return current_pos; };
    inline int getindex() { return index; };
    inline int getlength() { return length; };
    inline bool getIsEmpty() { return isEmpty; };

};

class Memory {
  private:
    byte memory_blks[NUM_BLOCKS][BLOCK_LENGTH];
    int dir_slots[MAX_NUM_BLKS];
    byte slot;
    byte offset;
    int bitMask[BIT_MASK_SIZE];

    // Private Functions
    void setBit(int *num, int x) { *num |= x; };
    void clearBit(int *num, int x) { *num &= ~(x); };
    void toggleBit(int *num, int x) { *num ^= 1 << x; };
    void checkBit(int *num, int x);   

  public:
    Memory();
    
    void initMemory();  
    void clearBlock(int block_num);
    void setBitMapLocation(byte bit_loc);
    void generateBitMask();
    void createDirectory();
    void setDescriptorEntry(int block, int offset, int length);
    void setDescriptorBlock(int desc_blk_num, int desc, int block_num_req, int file_index);
    void setFileDescriptor(int file_loc, int blk_num);

    inline byte* getBlk(int blk_num) { return memory_blks[blk_num]; };
    int getDescriptorIndexLocation(int desc_index, int file_index);
    int getFileIndex(int blk_num, int desc_index, int file_index);
    int getDirectoryLength();
    int findAvailableDescriptorSlot();
    int findAvailableBlock();
    int searchBitMap(int bits);
    int findFileName(std::string file_name, OFT *oft);
};

class Pack {
  private:
    const int MASK;

  public:
    Pack() : MASK(0xFF) {} ;

    void intToBytes(byte *block, int offset, int val);  
};

class UnPack {
  private:
    const int MASK;
    int num;

  public:
    UnPack() : MASK(0xFF) {};

    int bytesToInt(const byte *mem_loc, int start_loc);
};

#endif
