#include "cache.h"
#include <iostream>

OFT::OFT() {
    current_pos = 0;
    index = 0;
    isEmpty = true;
}


Memory::Memory() {
    // 
    int dir = INT_SIZE;
    for (int i = 0; i < MAX_NUM_BLKS; i++) {
        dir_slots[i] = dir;
        dir += INT_SIZE;
    }
}

int OFT::findEmptyDirLoc() {
    // look for an empty file location in the directory
    for (int i = 0; i < NUM_FILE_PER_BLK; ++i) {
        if (dir_block[i].getIndex() == EMPTY_LOC) {
            return i;
        }
    }
    return -1;
}


void OFT::setFileInDirBlk(int file_loc, int desc_index, std::string name) {
    // prep the file for a write
    dir_block[file_loc].setName((char*) name.c_str());
    dir_block[file_loc].setIndex(desc_index); 
    //std::memcpy((buffer + file_loc), (char*) name.c_str(), length + 1);
    //pack->intToBytes(buffer, file_loc + 4, desc_index);  

}

// 

void OFT::seek(int new_pos, Descriptor *desc) {
    int new_disk_map_index = (new_pos / BLOCK_LENGTH) + 1;
    int old_disk_map_index = (current_pos / BLOCK_LENGTH) + 1;
    // see if the we are currently in the correct block 
    if(old_disk_map_index != new_disk_map_index) {
        // get the block number that we need to read into the buffer
        int new_blk_num = desc->getDiskMapLoc(new_disk_map_index);
        int old_blk_num = desc->getDiskMapLoc(old_disk_map_index);
        // write the current block back to disk and then read in the new one
        disk->write_block(old_blk_num, buffer);
        read(new_blk_num, index, new_pos, length);
    }
    current_pos = new_pos;
}

// read will read a block from the disk and place it in the OFT table with
// the specified index

int OFT::read(int blk_num, int i, int c_pos, int l) {
    // read in the block from disk to buffer
    disk->read_block(blk_num, buffer);
    current_pos = c_pos;
    index = i;
    length = l; 
    isEmpty = false;
    return 0;
}


void OFT::resetParam() {
    // reset all the oft parameters
    current_pos = -1;
    index = -1;
    length = -1;
    isEmpty = true;
}





void Memory::initMemory() {
    Pack *pack = new Pack();
    int bitMap = 0;
    
    // Set the block 0 bitmap to have a single directory the bitmap should have 0 - 7 blocks 
    // as taken.  0 through 6 for bitmap and file descriptors and 7 for the first directory index
    generateBitMask();
    clearBlock(BITMAP_BLK);
    
    for (int i = 0; i < CACHE_BLK_SIZE; ++i) {
        setBit(&bitMap, bitMask[i]);       
    }
    // write the new bitMap to block 0
    pack->intToBytes(memory_blks[0], 0, bitMap);
    //std::cout << static_cast<unsigned int>(memory_blks[BITMAP_BLK][0]) << std::endl;  
    desc[0].setLength(0);
    
    // In this instance we set the first block available, which is 8 and set it as 
    // our first directory block.
    desc[0].setDiskMap(0, 7);
    //createDirectory();
    delete pack;
}


void Memory::clearBlock(int block_num) {
    // I will all of the bitmap regardless of how much I use
    for (int i = 0; i < NUM_BLOCKS; i++) {
        memory_blks[block_num][i] = 0;
    }
}


void Memory::generateBitMask() {
    bitMask[BIT_MASK_SIZE - 1] = 1;
    for (int i = BIT_MASK_SIZE - 2; i >= 0; i--) {
        bitMask[i] = bitMask[i + 1] << 1; 
    }
}


// Descriptor Functions
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


// This funcion will write the block number into 1 of the 3 block indices that point to 
// the files data
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
    // search for an empty descriptor by looking at its index and searching for -1.
    for (int i = 1; i < MAX_NUM_DESC; ++i) {
        if (desc[i].getLength() == EMPTY_LOC) {
            return i;
        }
    }    
    // No empty space available, directory is full
    return -1;
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
    // remove the block from the bitmap
    setBit(&bits, bitMask[location - 1]);
    writeToBitMap(location, bits);
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


void Memory::createNewFileDescriptor(int desc_index, int blk_num) {
    desc[desc_index].setLength(0);
    desc[desc_index].setDiskMap(0, blk_num);
    // increment the file count
    desc[DIR_INDEX].incLength(1);

    /*
    Pack *pack = new Pack();
    // determine the descriptor block and offset
    int desc_blk = file_loc / BLOCK_LENGTH;
    int slot_offset = file_loc % BLOCK_LENGTH;

    // write the length (zero) of the file followed by the index to its first block
    pack->intToBytes(memory_blks[desc_blk], slot_offset, 0);
    pack->intToBytes(memory_blks[desc_blk], slot_offset + INT_SIZE, blk_num); 
    delete pack;
    */
}



int Memory::findFileName(std::string file_name, OFT *oft) {
    int dir_length = desc[DIR_INDEX].getLength();
    
    // this loop will find the name of the file and returns a ptr index to that file
    for (int i = 0; i < dir_length; i++) {
        if (i % NUM_FILE_PER_BLK == 0) {
            oft->seek(i * NUM_FILE_PER_BLK, &desc[DIR_INDEX]);
        }
        if (file_name.compare(oft->getFileName(i)) == 0) {
            // clear the block
            return i;
        }          
    } 
    return -1;   
}


int Memory::deleteFile(int ptr_to_file, OFT *oft) {
    // get the the index number of the descriptor that will be deleted
    int desc_index = oft->getDescIndex(ptr_to_file);
    
    // set the file descriptor length to -1 and its disk map to -1
    // get the length of the file to iterate through its disk map and remove all blocks
    int num_block = (desc[desc_index].getLength() / BLOCK_LENGTH) + 1;
    for (int j = 0; j < num_block; j++) {
        // clear block from bitmap and remove index from descriptor
        UnPack *unpack = new UnPack();
        // clear the block using the bitmap
        int bit_loc = desc[desc_index].getDiskMapLoc(j);
        int bitmap = generateBitMap(bit_loc);
        bitmap = clearBit(&bitmap, bitMask[bit_loc - 1]);
        writeToBitMap(bit_loc, bitmap);
        // now remove the index from the disk map
        desc[desc_index].setDiskMap(j, -1);
        delete unpack;
    }
    // set the file descriptor length to -1 and remove a file from the directory length
    desc[desc_index].setLength(-1);
    desc[DIR_INDEX].decLength(1);
    // clear the file name and index pointing to the descriptor
    oft->clearFileName(ptr_to_file);
    oft->clearDescIndex(ptr_to_file);
    return 0;
}


int Memory::generateBitMap(int block_loc) {
    UnPack *unpack = new UnPack();
    int start_pos = 0;
    int bitmap;
    if (block_loc >= BIT_MASK_SIZE) {
        start_pos = INT_SIZE;
    }
    bitmap = unpack->bytesToInt(memory_blks[BITMAP_BLK], start_pos);
    delete unpack;
    return bitmap;

}


void Memory::writeToBitMap(int blk_loc, int bitmap) {
    Pack *pack = new Pack();
    int start_pos = 0;
    if (blk_loc >= BIT_MASK_SIZE) {
        start_pos = INT_SIZE;
    }
    pack->intToBytes(memory_blks[0], 0 + start_pos, bitmap); 
    delete pack;
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








