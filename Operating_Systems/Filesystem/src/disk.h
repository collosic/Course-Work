#ifndef DISK_H
#define DISK_H 

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
    byte **ldisk;
    // Private Functions
  public:
    Disk();

    // Public Functions
    void readBlock(int block, byte *loc);
    void writeBlock(int block, byte *loc);
    byte* getBlkAddr(int blk_num) { return ldisk[blk_num]; };
};


#endif
