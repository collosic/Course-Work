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
#define MAX_NUM_DESC 24
#define DIR_INDEX 0
#define NUM_FILE_PER_BLK 8
 


// Type Definitions
typedef std::vector<std::string> vecstr;
typedef unsigned char byte;

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


class File {
  private:
    std::string name;
    int index;  

  public:
    File() : name(""), index(-1) {};
    inline void setName(std::string s) { name = s; };
    inline void setIndex(int i) { index = i; };
    inline std::string getName() { return name; };
    inline int getIndex() { return index; };
};


class Descriptor {
  private:
    int length;
    int disk_map[MAX_NUM_BLKS];

  public:
    Descriptor() : length(EMPTY_LOC), disk_map{ -1, -1, -1 } {};
    inline void setLength(int l) { length = l; };
    inline int getLength() { return length; };
    inline void setDiskMap(int i, int blk_num) { disk_map[i] = blk_num; };
    inline int getDiskMapLoc(int i) { return disk_map[i]; };
    inline void incLength(int i) { length += i; };
    inline void decLength(int i) { length -= i; };
};


class OFT {
  private:
    File dir_block[NUM_FILE_PER_BLK];
    Disk *disk;
    byte buffer[BLOCK_LENGTH];
    int current_pos;
    int index;
    int length;
    bool isEmpty;

  public:
    OFT();
    
    //void initDiskAccess(Disk *d) { disk = d; };
    byte* getBuf() { return buffer; };
    int findEmptyDirLoc();
    void setFileInDirBlk(int file_loc, int desc_index, std::string name);
    
    void seek(int new_pos, int old_pos, Descriptor *desc);
    int read(int blk_num, int i, int c_pos, int l);
    void resetParam();
    byte read_byte(int i) { return buffer[i]; };
    void write_byte(byte ch, int i) { buffer[i] = ch; };
    void readDirFromBuffer();
    void writeDirToBuffer();
    void resetFiles();
    

    inline void setCurrentPos(int pos) { current_pos = pos; };
    inline void setIndex(int i) { index = i; };
    inline void setLength(int l)  { length = l; };
    inline void setEmpty(bool flag) { isEmpty = flag; };
    inline int getCurrentPos() { return current_pos; };
    inline int getIndex() { return index; };
    inline int getLength() { return length; };
    inline bool getIsEmpty() { return isEmpty; };
    inline std::string getFileName(int i) { return dir_block[i].getName(); };
    inline int getDescIndex(int i) { return dir_block[i].getIndex(); };
    inline void clearFileName(int i) { dir_block[i].setName(""); };
    inline void clearDescIndex(int i) { dir_block[i].setIndex(-1); };
    inline void initDiskAccess(Disk *d) { disk = d; };
};

class Memory {
  private:
    Disk *disk;
    Descriptor desc[MAX_NUM_DESC];
    byte memory_blks[NUM_BLOCKS][BLOCK_LENGTH];
    byte slot;
    byte offset;
    int bitMask[BIT_MASK_SIZE];
    Pack pack;
    UnPack unpack;

    // Private Functions
    void setBit(int *num, int x) { *num |= x; };
    int clearBit(int *num, int x) { return *num &= ~(x); };
    void toggleBit(int *num, int x) { *num ^= 1 << x; };
    void checkBit(int *num, int x);   

  public:
    Memory();
    
    void initMemory(Disk *ldisk);  
    void clearBlock(int block_num);
    void setBitMapLocation(byte bit_loc);
    void generateBitMask();
    void createDirectory();
    void setDescriptorEntry(int block, int offset, int length);
    void setDescriptorBlock(int desc_blk_num, int desc, int block_num_req, int file_index);
    void createNewFileDescriptor(int file_loc, int blk_num);

    inline byte* getBlk(int blk_num) { return memory_blks[blk_num]; };
    int getDescriptorIndexLocation(int desc_index, int file_index);
    int getFileIndex(int blk_num, int desc_index, int file_index);
    int getDirectoryLength();
    int findAvailableDescriptorSlot();
    int findAvailableBlock();
    int searchBitMap(int bits);
    int findFileName(std::string file_name, OFT *oft);
    int deleteFile(int i, OFT *oft);
    int generateBitMap(int block_loc);
    void writeToBitMap(int blk_loc, int bitmap);
    inline int getFileLength(int i) { return desc[i].getLength(); };
    inline int getBlockLocation(int i, int j) { return desc[i].getDiskMapLoc(j); };
    inline Descriptor* getDescriptor(int i) { return &desc[i]; };
    inline void setDiskMap(int i, int disk_i, int blk) { desc[i].setDiskMap(disk_i, blk); };
    inline void setDescLength(int i, int l) { desc[i].setLength(l); };
    void saveDescriptors();
};


#endif
