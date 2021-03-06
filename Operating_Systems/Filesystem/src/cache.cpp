#include "cache.h"
#include <iostream>

int OFT::findEmptyDirLoc() {
    // look for an empty file location in the directory
    for (int i = 0; i < NUM_FILE_PER_BLK; ++i) {
        // update the current pos
        //this->current_pos += SLOT_SIZE;
        if (dir_block[i].getIndex() == EMPTY_LOC) {
            return i;
        }
    }
    return -1;
}

void OFT::makeNull() {
    for (int i = 0; i < BLOCK_LENGTH; i++) {
        buffer[i] = '\0';
    }
}


void OFT::setFileInDirBlk(int file_loc, int desc_index, std::string name) {
    // prep the file for a write
    dir_block[file_loc].size = name.size();
    dir_block[file_loc].setName(name);
    dir_block[file_loc].setIndex(desc_index); 
    // need to update the current pos by 8 bytes. 4 for the name and 4 for the index
    //current_pos += SLOT_SIZE;
}

// 

void OFT::seek(int new_pos, int old_pos, Descriptor *desc) {
    int new_disk_map_index = new_pos / BLOCK_LENGTH;
    int old_disk_map_index = old_pos / BLOCK_LENGTH;
    // see if the we are currently in the correct block 
    if(old_disk_map_index != new_disk_map_index) {
        // get the block number that we need to read into the buffer
        int new_blk_num = desc->getDiskMapLoc(new_disk_map_index);
        int old_blk_num = desc->getDiskMapLoc(old_disk_map_index);
        // write the current block back to disk and then read in the new one
        if (new_blk_num > 0 && old_blk_num > 0) {
            disk->write_block(old_blk_num, buffer);
            read(new_blk_num, this->index, new_pos, this->length);
        }
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

void OFT::writeDirToBuffer() {
    for (int i = 0; i < NUM_FILE_PER_BLK; i++) {
        // for each file write the name and the index to the buffer
        const char *chars = dir_block[i].getName().c_str();
        int size = strlen(chars);
        int offset = i * NUM_FILE_PER_BLK;
        for (int j = offset; j < offset + size; j++) {
            write_byte(chars[j - offset], j);   
        }
        int file_index = dir_block[i].getIndex();
        pack.intToBytes(buffer, offset + INT_SIZE, file_index);  
    }
}


void OFT::readDirFromBuffer() {
    for (int i = 0; i < NUM_FILE_PER_BLK * SLOT_SIZE; i += SLOT_SIZE) {
        std::string name;
        int offset = i;
        int signed desc_index = unpack.bytesToInt(buffer, offset + INT_SIZE); 
        if (desc_index > -1) {
            for (int j = offset; j < offset + INT_SIZE; j++) {
                char temp = read_byte(j);
                if (temp == '\0') break;
                if (isalnum(temp)) {
                    name += temp;
                }
            }
            dir_block[i / SLOT_SIZE].setName(name);
            dir_block[i / SLOT_SIZE].setIndex(desc_index); 
        }
    } 
}

void OFT::resetFiles() {
    for (int i = 0; i < NUM_FILE_PER_BLK; i++) {
        dir_block[i].setName("");
        dir_block[i].setIndex(-1);     
    }
}
void OFT::resetParam() {
    // reset all the oft parameters
    current_pos = 0;
    index = 0; 
    length = 0;
    isEmpty = true;
    file_name = "";
}





void Memory::initMemory() {
    int bitMap = 0;
    
    // Set the block 0 bitmap to have a single directory the bitmap should have 0 - 7 blocks 
    // as taken.  0 through 6 for bitmap and file descriptors and 7 for the first directory index
    generateBitMask();
    clearBlock(BITMAP_BLK);
    
    for (int i = 0; i < CACHE_BLK_SIZE; ++i) {
        setBit(&bitMap, bitMask[i]);       
    }
    // write the new bitMap to block 0
    pack.intToBytes(memory_blks[0], 0, bitMap);
    //std::cout << static_cast<unsigned int>(memory_blks[BITMAP_BLK][0]) << std::endl;  
    desc[0].setLength(0);
    
    // In this instance we set the first block available, which is 8 and set it as 
    // our first directory block.
    desc[0].setDiskMap(0, 7);
    //createDirectory();
    //disk = ldisk;
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
    pack.intToBytes(memory_blks[block], offset, integer);
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
    int index_loc = getDescriptorIndexLocation(desc_index, file_index); 
    int index = unpack.bytesToInt(memory_blks[blk_num], index_loc);
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
    int bits;
    int location = -1;
    // we place a 8 here because our bitmap is 64 bits in length, but out
    // bitMask is only 32 bits.  Therefore, we must search twice if needed
    for (int i = 0; i < 8; i += 4) {
        // grab an int from the bitmap and search for an empty block
        bits = unpack.bytesToInt(memory_blks[BITMAP_BLK], i);
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
}



int Memory::findFileName(std::string file_name, OFT *oft) {
    int curr = oft->getCurrentPos();
    if (curr != 0) curr -= 1;
    int curr_blk = desc[DIR_INDEX].getDiskMapLoc(curr / BLOCK_LENGTH);
    if (curr_blk != -1)  {
        oft->writeDirToBuffer();
        disk->write_block(curr_blk, oft->getBuf());
        oft->seek(0, curr, &desc[DIR_INDEX]);
        oft->readDirFromBuffer();
        curr_blk = desc[DIR_INDEX].getDiskMapLoc(0);
    }
    


    for (int k = 64; k <= 192; k += 64) {

        // this loop will find the name of the file and returns a ptr index to that file
        for (int i = 0; i < NUM_FILE_PER_BLK; i++) {
            oft->setCurrentPos((i * SLOT_SIZE) + (k - 64)) ;
            if (file_name.compare(oft->getFileName(i)) == 0) {
                return i;
            }          
        } 
        if (k < 192) {

            int next_blk = desc[DIR_INDEX].getDiskMapLoc(k / BLOCK_LENGTH);
            // if next blk is -1 then we've reached the end of our search
            if (next_blk == -1) return -1;
            oft->writeDirToBuffer();
            disk->write_block(curr_blk, oft->getBuf());
            oft->seek(k, k - 1, &desc[DIR_INDEX]);
            oft->resetFiles();
            oft->readDirFromBuffer();
            curr_blk = next_blk;
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
    oft->makeNull();
    return 0;
}


int Memory::generateBitMap(int block_loc) {
    int start_pos = 0;
    int bitmap;
    if (block_loc >= BIT_MASK_SIZE) {
        start_pos = INT_SIZE;
    }
    bitmap = unpack.bytesToInt(memory_blks[BITMAP_BLK], start_pos);
    return bitmap;

}


void Memory::writeToBitMap(int blk_loc, int bitmap) {
    int start_pos = 0;
    if (blk_loc >= BIT_MASK_SIZE) {
        start_pos = INT_SIZE;
    }
    pack.intToBytes(memory_blks[0], 0 + start_pos, bitmap); 
}


void Memory::saveDescriptors() {
    for (int i = 1; i < CACHE_BLK_SIZE; i++) {
        for (int j = 0; j < 4; j++) {
            int offset = j * DESCRIPTOR_SIZE;
            int desc_offset = 4 * (i - 1);
            // place the length of the file into 4 bytes
            int signed length = desc[j + desc_offset].getLength();
            pack.intToBytes(memory_blks[i], offset, length);
            // save the disk map to the memory block
            for (int l = 0; l < MAX_NUM_BLKS; l++) {
                offset += INT_SIZE;
                int signed blk_num = desc[j + desc_offset].getDiskMapLoc(l); 
                pack.intToBytes(memory_blks[i], offset, blk_num); 
            }
        }
    }
    // For simplicity I saved it first to a local disk and now we 
    // can save it to the ldisk.  The ldisk may be used above instead
    // of the local memory block.
    for (int i = 0; i < CACHE_BLK_SIZE; i++) {
        disk->write_block(i, memory_blks[i]);
    }
}


void Memory::flushDescriptors() {
   for (int i = 0; i < CACHE_BLK_SIZE; i++) {
       for (int j = 0; j < BLOCK_LENGTH; j++) {
           memory_blks[i][j] = 0;
       }
   } 
   
   for (int i = 0; i < MAX_NUM_DESC; i++) {
        desc[i].setLength(-1);
        for (int j = 0; j < MAX_NUM_BLKS; j++) {
            desc[i].setDiskMap(j, -1);
        }   
   }


}

void Memory::loadDescriptors() {
    // insert the bitmap first
    disk->read_block(0, memory_blks[0]);
    for (int i = 1; i < CACHE_BLK_SIZE; i++) {
        disk->read_block(i, memory_blks[i]);
        // insert the lengths and diskmaps of each descriptor
        loadDescriptorsFromBlk(i);
    }

}

void Memory::loadDescriptorsFromBlk(int blk_num) {
    int num_desc_per_blk = 4;
    int offset = 0;
    int diff = (blk_num - 1) * num_desc_per_blk;
    // traverse the given block number populating the descriptor object
    for (int i = 0; i < num_desc_per_blk; ++i) {
        int signed length = unpack.bytesToInt(memory_blks[blk_num], offset);
        //if (length == 0) length = -1;
        desc[i + diff].setLength(length);
        offset += INT_SIZE;
        for (int j = 0; j < MAX_NUM_BLKS; j++) {
            int signed disk_map_blk = unpack.bytesToInt(memory_blks[blk_num], offset);
            desc[i + diff].setDiskMap(j, disk_map_blk);
            offset += INT_SIZE;
        }
    }
}

void Pack::intToBytes(char *block, int offset, int signed val) {
    for (int i = 3; i >= 0; i--) {
        block[offset + i] = (byte)(val & MASK);
        val = val >> 8;        
    }
} 


int UnPack::bytesToInt(const char *mem_loc, int start_loc) {
    num = (signed int)mem_loc[start_loc] & MASK;
    int num_ret;
    for (int i = 0; i < INT_SIZE; ++i) {
        num = num << 8;
        num = num | ((signed int)mem_loc[start_loc + i] & MASK);        
    }
    num_ret = num;
    num = 0;
    return num_ret;
}








