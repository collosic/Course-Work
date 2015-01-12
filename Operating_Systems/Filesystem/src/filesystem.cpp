#include "filesystem.h"
#include <iostream>

FileSystem::FileSystem() {
    Memory mem(&disk);
}

void FileSystem::initDisk(vecstr *in) {
    // Check and see if user provided a disk image
    std::cout << in->size() << std::endl;
    if (in->size() >= 1) {
       //loadDisk(in->front());
    } else {
        createDisk();
    }
}

void FileSystem::createDisk() {
    mem.initMemory();
    setResponse("Created Disk");     
}

void FileSystem::loadDisk(std::string file_name) {
    
}

