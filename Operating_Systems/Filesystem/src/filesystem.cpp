#include "filesystem.h"
#include <iostream>

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
    
    // initialize the oft to have disk access
    for (int i = 0; i < OFT_SIZE; i++) {
        oft[i].initDiskAccess(&disk);
    }
    // set the oft to non empty
    oft[OFT_DIR_BLK].setEmpty(false);
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
            int ptr_to_filename = oft[OFT_DIR_BLK].findEmptyDirLoc();
            
            // find an open block to link the newly created file
            int new_block_ptr = mem.findAvailableBlock(); 

            // First set the new block to the new descriptor, Second set the file name 
            // in the correct direcotry block and have its index point to the file descriptor
            mem.createNewFileDescriptor(new_desc_index, new_block_ptr);
            oft[OFT_DIR_BLK].setFileInDirBlk(ptr_to_filename, new_desc_index, file_name);
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
            // TODO: you can modularize the function before to delete different sections            
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
    // go through the OFT and check for an empty spot, starting at 1 because
    // 0 is always taken by the directory
    for (int i = 1; i < OFT_SIZE; i++) {
        if(oft[i].getIsEmpty()) {
            new_oft_index = i;
            break;
        } 
    }
    
    // check and see if the new oft index has a positive value, if not return an error
    if (new_oft_index != -1) {
        // first get the block that you will be copying over from the disk
        int ptr_to_filename = mem.findFileName(file_name, &oft[OFT_DIR_BLK]);
        if (ptr_to_filename == -1) {
            setResponse("file does not exist");
        } else {
            int desc_index = oft->getDescIndex(ptr_to_filename);
            int blk_num = mem.getBlockLocation(desc_index, 0);
            length = mem.getFileLength(desc_index);
            
            // read disk block into buffer
            oft[new_oft_index].read(blk_num, desc_index, 0, length);
            setResponse(file_name + " opened " + std::to_string(new_oft_index));
        }

    } else {
        setResponse("OFT is full");
    }
}

void FileSystem::closeFile(vecstr *in) {
    // test to see if the second input is a digit between 1 and 3
    int oft_index = stoi(in->front());
    if (oft_index >= 1 && oft_index <= 3) {
         // get the current block that is open and save it to disk
        int curr_pos = oft[oft_index].getCurrentPos();
        int disk_map = (curr_pos / BLOCK_LENGTH) + 1;
        int blk_num = mem.getBlockLocation(oft[oft_index].getIndex(), disk_map);  
        byte *buf = oft[oft_index].getBuf();
        disk.write_block(blk_num, buf);
        // reset the oft parameters
        oft[oft_index].resetParam();
    } else {
        setResponse("not a valid use of cl");
    }

   

}
