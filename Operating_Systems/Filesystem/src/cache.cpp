#include "cache.h"

OFT::OFT() {
    current_pos = 0;
    index = 0;
    isEmpty = true;
}


Memory::Memory() {
}

void OFT::clearDirectory(const byte *mem) {
    // copy the contects of the directory block into the buffer
    Pack *pack = new Pack();
    pack->intToBytes(buffer, INT_SIZE, EMPTY_LOC);
    for(int i = (INT_SIZE + SLOT_SIZE); i < BLOCK_LENGTH; i += SLOT_SIZE) {
        // place a -1 in the index of empty file
        pack->intToBytes(buffer, i, EMPTY_LOC);
    }
}
void Memory::initMemory() {
    // Set the block 0 bitmap to have a single directory out bitmap should have 0 - 7 blocks 
    // as taken.  0 through 6 for bitmap and file descriptors and 7 for the first directory index
    clearBlock(0);
    for (int i = 0; i < CACHE_BLK_SIZE + 1; ++i) {
        setBitMapLocation(i);
        setBit(&memory_blks[BITMAP_BLK][slot], offset);
    }
    //std::cout << static_cast<unsigned int>(memory_blks[BITMAP_BLK][0]) << std::endl;       
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
    // Set the Directory to have zero files 
    setDescriptorEntry(1, 0, 0);
    
    // Set the first block of the directory to one after the size of 
    // the bitmap and the file descriptors. This is only done for initialization
    setDescriptorBlock(1, 0, CACHE_BLK_SIZE, 0);
}

void Memory::setDescriptorEntry(int block, int offset, int integer) {
    Pack *pack = new Pack();         
    pack->intToBytes(memory_blks[block], offset, integer);
    delete pack;
}

void Memory::setDescriptorBlock(int desc_blk_num, int desc, int block_num_req, int file_index) {
    // determine how many bytes to move into a block to add the file index i, j or k
    int addr_offset = getDescriptorIndexLocation(desc, file_index); 
    // Enter the index into the file descriptor 
    setDescriptorEntry(desc_blk_num, addr_offset, block_num_req);
}


int Memory::getFileIndex(int blk_num, int desc_index, int file_index) {
    int index_loc = getDescriptorIndexLocation(desc_index, file_index); 
    UnPack *unpack = new UnPack();
    int index = unpack->bytesToInt(memory_blks[blk_num], index_loc);
    delete unpack;
    return index;
}


/* This function takes in a block number, the descriptor index, and the file index
 * It will then go to the location in that block, find the descriptor and then determine
 * the location of the index byte relative to the descriptor
 */

int Memory::getDescriptorIndexLocation(int desc_index, int file_index) {
    int offset = desc_index * DESCRIPTOR_SIZE;
    return offset + (INT_SIZE + (file_index * INT_SIZE));
}



void Pack::intToBytes(byte *block, int offset, int val) {
    for (int i = 3; i >= 0; i--) {
        block[offset + i] = (byte)(val & MASK);
        val = val >> 8;        
    }
} 


int UnPack::bytesToInt(const byte *mem_loc, int start_loc) {
    num = (int)mem_loc[start_loc] & MASK;
    for (int i = 0; i < INT_SIZE; ++i) {
        num = num << 8;
        num = num | ((int)mem_loc[start_loc + i] & MASK);        
    }
    return this->num;
}








