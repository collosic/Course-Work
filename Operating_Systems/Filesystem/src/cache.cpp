#include "cache.h"
#include <iostream>

OFT::OFT() {
    current_pos = 0;
    index = 0;
    isEmpty = true;
}


Memory::Memory() {
    int dir = INT_SIZE;
    for (int i = 0; i < MAX_NUM_BLKS; i++) {
        dir_slots[i] = dir;
        dir += INT_SIZE;
    }
}

void OFT::clearDirectory() {
    // copy the contects of the directory block into the buffer
    Pack *pack = new Pack();
    pack->intToBytes(buffer, INT_SIZE, EMPTY_LOC);
    for(int i = (INT_SIZE + SLOT_SIZE); i < BLOCK_LENGTH; i += SLOT_SIZE) {
        // place a -1 in the index of empty file
        pack->intToBytes(buffer, i, EMPTY_LOC);
    }
    isEmpty = false;
    delete pack;
}


int OFT::findAvailableSlot() {
    UnPack *unpack = new UnPack();
    for (int i = INT_SIZE; i < BLOCK_LENGTH; i += SLOT_SIZE) {
        if (unpack->bytesToInt(buffer, i) == EMPTY_LOC) {
           // return the begining of the slot found
           delete unpack;
           return i - INT_SIZE;           
        }
    }
    delete unpack;
    return -1;
}


void OFT::setFile(int file_loc, int desc_index, std::string name) {
    // prep the file for a write
    Pack *pack = new Pack();
    char *char_name = (char*)name.c_str();
    int length = name.size();

    // write char to the directory
    for (int i = 0; i < length; ++i) {
        buffer[file_loc++] = char_name[i];
    } 
    // now write the index of the file descriptot
    pack->intToBytes(buffer, file_loc, desc_index);  
}

void OFT::seek(int index, int new_pos) {
    // and see if the we are currently in the correct block 
    if(current_pos % BLOCK_LENGTH != new_pos % BLOCK_LENGTH) {
        // now move in the block from the disk into the current OFT index
        // extract the directory block that will first be searched
        //int dir_blk = unpack->bytesToInt(memory_blks[1], dir_slots[i]];
        

    }
    current_pos = new_pos;
}


void Memory::initMemory() {
    Pack *pack = new Pack();
    int bitMap = 0;
    
    // Set the block 0 bitmap to have a single directory the bitmap should have 0 - 7 blocks 
    // as taken.  0 through 6 for bitmap and file descriptors and 7 for the first directory index
    generateBitMask();
    clearBlock(0);
    
    for (int i = 0; i < CACHE_BLK_SIZE; ++i) {
        setBit(&bitMap, bitMask[i]);       
    }
    // write the new bitMap to block 0
    pack->intToBytes(memory_blks[0], 0, bitMap);
    //std::cout << static_cast<unsigned int>(memory_blks[BITMAP_BLK][0]) << std::endl;       
    createDirectory();

    // Place a -1 in all descriptors length slot to make them empty, except directory
    for (int i = 1; i < CACHE_BLK_SIZE; ++i) {
        for (int j = 0; j < BLOCK_LENGTH; j += DESCRIPTOR_SIZE) {
           pack->intToBytes(memory_blks[i], j, EMPTY_LOC);
        }
    }  
    pack->intToBytes(memory_blks[1], 0, 0);
    delete pack;
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


void Memory::generateBitMask() {
    bitMask[BIT_MASK_SIZE - 1] = 1;
    for (int i = BIT_MASK_SIZE - 2; i >= 0; i--) {
        bitMask[i] = bitMask[i + 1] << 1; 
    }
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

// TODO: 
int Memory::getFileIndex(int blk_num, int desc_index, int file_index) {
    UnPack *unpack = new UnPack();
    int index_loc = getDescriptorIndexLocation(desc_index, file_index); 
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


int Memory::findAvailableDescriptorSlot() {
    UnPack *unpack = new UnPack();
    for (int i = 1; i < CACHE_BLK_SIZE; i++) {
        for (int j = 0; j < BLOCK_LENGTH; j += SLOT_SIZE) {
            if (unpack->bytesToInt(memory_blks[i], j) == EMPTY_LOC) {
                return j;
            }
        }
    }
    // No empty space available, directory is full
    return -1;
}


int Memory::getDirectoryLength() {
    UnPack *un = new UnPack();
    int length = un->bytesToInt(memory_blks[1],0);
    delete un;
    return length;
}


int Memory::findAvailableBlock() {
    UnPack *unpack = new UnPack();
    int bits;
    int location = -1;
    // we place a 8 here because our bitmap is 64 bits in length, but out
    // bitMask is only 32 bits.  Therefore, we must search twice if needed
    for (int i = 0; i < 8; i += 4) {
        // grab an int from the bitmap and search for an empty block
        bits = unpack->bytesToInt(memory_blks[BITMAP_BLK], i);
        location = searchBitMap(bits);
        if (location != -1 && i == 0) {
            break;
        }
        if (location != -1 && i > 0) {
            location += BIT_MASK_SIZE;
        }
    }
    delete unpack;
    return location;
}


int Memory::searchBitMap(int bits) {
    int test = 1;
    for (int i = 0; i < BIT_MASK_SIZE; ++i) {
        test = bits & bitMask[i];
        if (test == 0) {
            return i + 1;
        }        
    }
    return -1;    
}


void Memory::setFileDescriptor(int file_loc, int blk_num) {
    Pack *pack = new Pack();
    // determine the descriptor block and offset
    int desc_blk = file_loc / BLOCK_LENGTH;
    int slot_offset = file_loc % BLOCK_LENGTH;

    // write the length (zero) of the file followed by the index to its first block
    pack->intToBytes(memory_blks[desc_blk], slot_offset, 0);
    pack->intToBytes(memory_blks[desc_blk], slot_offset + INT_SIZE, blk_num); 
    delete pack;
}



int Memory::findFileName(std::string file_name, OFT *oft) {
    UnPack *unpack = new UnPack();
    // We should seek here to the beginning of the directory
    std::string extracted_name;
    int dir_length = getDirectoryLength();
    int index_to_desc = -1;

    for (int i = 0; i < (dir_length % BLOCK_LENGTH); i++) {
        // we have been given index 0 of the OFT and we can just seek to the beginning 
        // of the directory.  The position will increment if needed by placing new blocks
        // into the OFT in each iteration.
        oft->seek(0, BLOCK_LENGTH * i);

        for (int j = 0; j < BLOCK_LENGTH; j += SLOT_SIZE) {
            extracted_name = oft->read[j + 4];
            oft->setCurrentPos(j);    
            // if name found then return the index 
            if (extracted_name.compare(file_name) == 0) {
                // extract index of descriptor and set the index to -1
                index_to_desc = unpack->bytesToInt(oft->getBuf(), j + 4);
                pack->intToBytes(oft->getBuf(), j + 4, EMPTY_LOC);
                return index_to_desc;
            }      
        }
    }    
    return index_to_desc;

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
    return num;
}








