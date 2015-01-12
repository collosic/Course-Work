#include "filesystem.h"
#include <iostream>

FileSystem::FileSystem() {
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
    int dir_blk_loc = mem.getFileIndex(MEM_DIR_BLK, 0, 0);
    oft[OFT_DIR_BLK].clearDirectory(mem.getBlk(dir_blk_loc));

    unpack = new UnPack();
    int test = unpack->bytesToInt(oft[0].getBuf(), 20);
    std::cout << test << std::endl;
    delete unpack;
    setResponse("disk initialized");
}

void FileSystem::loadDisk(std::string file_name) {
    
}

