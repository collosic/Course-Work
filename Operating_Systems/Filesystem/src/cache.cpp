#include "cache.h"
#include <iostream>

OFT::OFT() {

}
Memory::Memory() {
}


void Memory::initMemory() {
    
    // Set the block 0 bitmap to have a single directory
    clearBlock(0);
    for (int i = 0; i < CACHE_BLK_SIZE; ++i) {
        setBitMapLocation(i);
        setBit(&memory_blks[BITMAP_BLK][slot], offset);
    }
    std::cout << static_cast<unsigned int>(memory_blks[BITMAP_BLK][0]) << std::endl;       
    createDirectory();
}

void Memory::clearBlock(int block_num) {
    // I will all of the bitmap regardless of how much I use
    for (int i = 0; i < NUM_BLOCKS; i++) {
        memory_blks[block_num][i] = 0;
    }
}

void Memory::setBitMapLocation(byte bit_loc) {
    slot = (bit_loc + 1) / SLOT_SIZE;
    offset = SLOT_SIZE - ((bit_loc + 1) % SLOT_SIZE);
    
}

void Memory::createDirectory() {
    setDescriptorLength(1, 0, 0);
    
    // Set the first block of the directory to one after the size of 
    // the bitmap and the file descriptors. This is only done for initialization
    
    setDescriptorBlock(1, 0, CACHE_BLK_SIZE, 0);
}

void Memory::setDescriptorLength(int block, int offset, int length) {
    Pack *pack = new Pack();         
    pack->intToBytes(length);
    pack->extractPackedInteger(&memory_blks[block][offset]);           
    delete pack;
}


void Pack::intToBytes(int val) {
    for (int i = 3; i >= 0; i--) {
        mem[i] = (byte)(val & MASK);
        val = val >> 8;        
    }
}


void Pack::extractPackedInteger(byte *arr) {
     for (int i = 0; i < INT_SIZE; ++i) {
         arr[i] = mem[i];
     } 
}

void Memory::setDescriptorBlock(int desc_blk_num, int desc, int block_num_req, int file_index) {
    int offset = desc * DESCRIPTOR_SIZE;
    int addr_offset = offset + (INT_SIZE + (file_index * INT_SIZE));
    byte *addr = disk.getBlkAddr(block_num_req);
    
    byte *test = addr;
    std::cout << test[0] << std::endl;

    //memory_blks[desc_blk_num][addr_offset] = 

}






