#include "filesystem.h"
#include <iostream>

FileSystem::FileSystem() {
    isInitialized = false;
}

void FileSystem::initDisk(vecstr *in) {
    // Check and see if user provided a disk image
    if (in->size() >= 1) {
       //loadDisk(in->front());
    } else {
        createDisk();
    }
}

void FileSystem::createDisk() {
    // Initialize memery with bitmap and directory
    mem.initMemory();

    // Copy the directory into index 0 of the OFT
    oft[OFT_DIR_BLK].initDirectory();

    unpack = new UnPack();
    //int test1 = unpack->bytesToInt(oft[0].getBuf(), 20);
    //int desc_test = unpack->bytesToInt(mem.getBlk(1), 16);
    delete unpack;
    isInitialized = true;
    setResponse("disk initialized");
}

void FileSystem::loadDisk(std::string file_name) {
    
}

// create a file by calling the various parts of cache and disk
void FileSystem::createFile(vecstr *in) {
    std::string file_name = in->front();
    in->erase(in->begin());
    if (file_name.length() <= 3) {
        // Search the directory for an empty slot
        int new_desc_index = mem.findAvailableDescriptorSlot();
        if (new_desc_index == -1) {
            setResponse("disk is full");
        } else {
            // TODO: make sure you if this returns -1 to move over to index j and k 
            int loc_in_dir_blk = oft[OFT_DIR_BLK].findEmptyDirLoc();
            
            // find an open block to link the newly created file
            int new_block = mem.findAvailableBlock(); 

            // First set the new block to the new descriptor, Second set the file name 
            // in the correct direcotry block and have its index point to the file descriptor
            mem.createNewFileDescriptor(new_desc_index, new_block);
            oft[OFT_DIR_BLK].setFileInDirBlk(loc_in_dir_blk, new_desc_index, file_name);
            setResponse(file_name + " created");
        }
    } else {
        setResponse("file name too long");
    }

}

void FileSystem::deleteFile(vecstr *in) {
    std::string file_name = in->front();
    in->erase(in->begin());
    if (file_name.length() <= 3) {
        // first find the name in the directory if it does not exist return error
        int ptr_to_desc = mem.findFileName(file_name, &oft[OFT_DIR_BLK]);
        if (ptr_to_desc != -1) {
            mem.deleteFile(ptr_to_desc, &oft[OFT_DIR_BLK]);            
            setResponse(file_name + " was deleted"); 
        } else {
            setResponse("file name does not exist");
        }

    } else {
        setResponse("file name too long");
    }
}

void FileSystem::openFile(vecstr *in) {
    std::string file_name = in->front();
    int new_oft_index = -1;
    int length;
    // got through the OFT and check for an empty spot
    for (int i = 1; i < OFT_SIZE; i++) {
        if(oft[i].getIsEmpty()) {
            new_oft_index = i;
            break;
        } 
    }
    
    // check and ses if the new oft index has a positive value, if not return an error
    if (new_oft_index != -1) {
        // first get the block that you will be copying over from the disk
        byte *block;
        int block_index = mem.findFileName(file_name, &oft[OFT_DIR_BLK]);
        if (block_index == -1) {
            setResponse("file does not exist");
        } else {
            int blk_num = mem.getBlockLocation(oft->getDescIndex(block_index), 0);
            disk.read_block(blk_num, block);
            length = mem.getFileLength(oft->getDescIndex(block_index));
            oft[new_oft_index].read(block, new_oft_index, 0, length);
            setResponse(file_name + " opened " + std::to_string(new_oft_index));
        }

    } else {
        setResponse("OFT is full");
    }




}


