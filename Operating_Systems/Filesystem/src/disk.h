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
    byte ldisk[NUM_BLOCKS][BLOCK_LENGTH];
    // Private Functions
  public:
    Disk();

    // Public Functions
    byte* getBlkAddr(int blk_num) { return ldisk[blk_num]; };
    void read_block(int blk_num, byte *p);
    void write_block(int blk_num, byte *p);
};


#endif
