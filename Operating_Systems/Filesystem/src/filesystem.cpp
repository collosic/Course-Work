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
    oft[OFT_DIR_BLK].clearDirectory();

    unpack = new UnPack();
    //int test1 = unpack->bytesToInt(oft[0].getBuf(), 20);
    //int desc_test = unpack->bytesToInt(mem.getBlk(1), 16);
    delete unpack;
    isInitialized = true;
    setResponse("disk initialized");
}

void FileSystem::loadDisk(std::string file_name) {
    
}


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
            int file_loc = oft[OFT_DIR_BLK].findAvailableSlot();
            
            int new_block = mem.findAvailableBlock(); 
            mem.setFileDescriptor(new_desc_index, new_block);
            oft[OFT_DIR_BLK].setFile(file_loc, new_desc_index, file_name);
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
        // first find the name in the directory if does not existe return error
        int ptr_to_desc = mem.findFileName(file_name, &oft[OFT_DIR_BLK]);
    } else {
        setResponse("file name too long");
    }
}

