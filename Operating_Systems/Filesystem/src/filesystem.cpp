#include "filesystem.h"
#include <iostream>

void FileSystem::initDisk(vecstr *in) {
    // Before anything can start we need to give the oft access to the disk
    // this only needs to be ran once
    if (!(isInitialized)) {
        for (int i = 0; i < OFT_SIZE; i++) {
            oft[i].diskAccess(&disk);        
        }
    }

    // check and see if we need to load an image 
    if (in->size() == 1) {
       loadDisk(in->front());
       in->erase(in->begin());
    } else {
        createDisk();
    }
}

// TODO: Make sure each command only excepts a certain amount of strings
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
    // call the disk to open the disk file and read it in
    if (!(disk.loadData(file_name))) {
        // initially this was supposed to be an error, but not is will just initialize
        createDisk();
    } else {
        mem.flushDescriptors();
        // load the descriptors into memory
        mem.loadDescriptors();

        // flush out the contents of the buffers and files
        // ** In my design I should have separated the directory from the oft 
        // this is why we see redundancy
        for (int i = 0; i < OFT_SIZE; i++) {
            oft[i].resetFiles();
            oft[i].resetParam();
        }
        // load the first directory into oft 0, setting current position to 0
        int dir_blk =  mem.getBlockLocation(DIR_DESC, 0);
        int dir_length = mem.getFileLength(DIR_DESC);
        oft[OFT_DIR_BLK].read(dir_blk, DIR_DESC, 0, dir_length);
        
        // load file name into object from buffer
        oft[OFT_DIR_BLK].readDirFromBuffer();
        isInitialized = true;

        setResponse("disk restored");
        
    }
}

// create a file by calling the various parts of cache and disk
void FileSystem::createFile(vecstr *in) {
    std::string file_name = in->front();
    in->erase(in->begin());
    int off;
    // TODO: make sure you check if files can digits for names
    if (file_name.length() <= 3) {
        // Search the directory for an empty slot
        int new_desc_index = mem.findAvailableDescriptorSlot();
        if (new_desc_index == -1) {
            setResponse("disk is full");
        } else {
            // this should always return an empty directory location because 
            // there exists an available descriptor slot 
            int curr_pos = oft[OFT_DIR_BLK].getCurrentPos();
            oft[OFT_DIR_BLK].writeDirToBuffer();
            oft[OFT_DIR_BLK].seek(0, curr_pos, mem.getDescriptor(DIR_DESC)); 
            oft[OFT_DIR_BLK].resetFiles();
            oft[OFT_DIR_BLK].readDirFromBuffer();
            int ptr_to_filename = -1;
            for (int i = 1; i < 3; i++) {
                int offset = i * BLOCK_LENGTH;
                ptr_to_filename = oft[OFT_DIR_BLK].findEmptyDirLoc();
                if (ptr_to_filename == -1) {
                    // Now we need to go to the next block or create a new one
                    off = offset;
                    int next_blk = mem.getBlockLocation(OFT_DIR_BLK, i);
                    int curr_blk = mem.getBlockLocation(OFT_DIR_BLK, i - 1);
                    if (next_blk == -1) {
                        next_blk = mem.findAvailableBlock();
                        mem.setDiskMap(DIR_DESC, i, next_blk);
                        oft[OFT_DIR_BLK].writeDirToBuffer();
                        disk.write_block(curr_blk, oft[OFT_DIR_BLK].getBuf());
                        oft[OFT_DIR_BLK].resetFiles();
                        ptr_to_filename = 0;
                        break;
                    } else {
                        // seek to the new block, for directory we need to write to the buffer first
                        oft[OFT_DIR_BLK].writeDirToBuffer();
                        disk.write_block(curr_blk, oft[OFT_DIR_BLK].getBuf());
                        oft[OFT_DIR_BLK].seek(offset, offset - 1, mem.getDescriptor(0));
                        oft[OFT_DIR_BLK].resetFiles();
                        oft[OFT_DIR_BLK].readDirFromBuffer();                
                    }
                } else {
                    break;
                } // end of if prt to file
            } // end of for loop
            oft[OFT_DIR_BLK].setCurrentPos(off);
            // find an open block to link the newly created file
            int new_block_ptr = mem.findAvailableBlock(); 

            // First set the new block to the new descriptor, Second set the file name 
            // in the correct direcotry block and have its index point to the file descriptor
            mem.createNewFileDescriptor(new_desc_index, new_block_ptr);
            oft[OFT_DIR_BLK].setFileInDirBlk(ptr_to_filename, new_desc_index, file_name);
            // increment the length in oft 0 using the descriptor length
            oft[OFT_DIR_BLK].setLength(mem.getFileLength(DIR_DESC));
            // TODO: increment the length
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
        int ptr_to_filename = mem.findFileName(file_name, &oft[OFT_DIR_BLK]);
        if (ptr_to_filename != -1) {
            mem.deleteFile(ptr_to_filename, &oft[OFT_DIR_BLK]);
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
    // TODO: make sure you check and see if the string is a Digit first
    int oft_index = stoi(in->front());
    if (oft_index >= 1 && oft_index <= 3) {
         // get the current block that is open and save it to disk
        int curr_pos = oft[oft_index].getCurrentPos();
        int disk_map = curr_pos / BLOCK_LENGTH;
        int blk_num = mem.getBlockLocation(oft[oft_index].getIndex(), disk_map);  
        char *buf = oft[oft_index].getBuf();
        disk.write_block(blk_num, buf);
        // reset the oft parameters
        oft[oft_index].resetParam();
    } else {
        setResponse("not a valid use of cl");
    }
}

// TODO: block against reading from oft 0
void FileSystem::read(vecstr *in) {
    std::string oft_i = in->front();
    in->erase(in->begin());   
    std::string end_p = in->front();
    in->erase(in->begin());
    // check and see if the string extracted is in fact an integer if not error
    if (isInteger(oft_i) && isInteger(end_p)) {
        std::string str;
        int oft_index = stoi(oft_i);
        int end_pos = stoi(end_p);
        int curr_pos = oft[oft_index].getCurrentPos();
        int length = oft[oft_index].getLength();

        // read the bytes in sequential order
        for (int i = curr_pos; i < curr_pos + end_pos; ++i) {
           if (i >= FILE_LIMIT || i >= length) {
                break;
            }
            // if the bytes being read goes beyond the block, seek to that new block
            if (i % BLOCK_LENGTH == 0) {
                int desc_index = oft[oft_index].getIndex();   
                oft[oft_index].seek(i, i - 1, mem.getDescriptor(desc_index));
            } 
            str += oft[oft_index].read_byte(i % BLOCK_LENGTH);
            oft[oft_index].setCurrentPos(i); 
        }
        setResponse(str);
    } else {
        setResponse("invalid use of rd");
    }
}


void FileSystem::write(vecstr *in) {
    std::string oft_i = in->front();
    in->erase(in->begin());
    std::string characters = in->front();
    in->erase(in->begin());
    std::string end_p;
    end_p = in->front();
    in->erase(in->begin());
    
    int size = characters.size();
    // this flag is used for when a user attempts to write beyond the length or size
    // of the file.
    response = "";

    // check if file is open first
    if (oft[stoi(oft_i)].getIsEmpty()) {
        setResponse("file is not open");
    }
    
    // check and see if the strings are formated correctly and have the correct input
    if(isInteger(oft_i) && isLetter(characters) && isInteger(end_p)) {
        int oft_index = stoi(oft_i);
        int end_pos = stoi(end_p);
        int start_pos = oft[oft_index].getCurrentPos();
        int length = oft[oft_index].getLength();
        int bytes_written = 0;
        int desc_index = oft[oft_index].getIndex();   
        const char *chars = characters.c_str(); 

        for (int i = start_pos; i < start_pos + end_pos; i++) {
            if (i >= FILE_LIMIT) {
                response += "file limit reached, only ";
                break;
            }
            // if the bytes being written goes beyond the block, create that new block
            if (i % BLOCK_LENGTH == 0) {
                // check and see if the next block exists
                int next_blk = mem.getBlockLocation(desc_index, i / BLOCK_LENGTH);
                // check and see if we need to allocate a new block
                if (next_blk == -1) {
                    int disk_map_index = i / BLOCK_LENGTH; 
                    int new_block_ptr = mem.findAvailableBlock(); 
                    mem.setDiskMap(desc_index, disk_map_index, new_block_ptr);
                }
                oft[oft_index].seek(i, i - 1, mem.getDescriptor(desc_index));
            } 
            // depending on the character/s we need to insert that char into the block 
            oft[oft_index].write_byte(chars[(i - start_pos) % size], i % BLOCK_LENGTH);
            oft[oft_index].setCurrentPos(i + 1); 
            if (i >= length) {
                mem.setDescLength(desc_index, length + 1);
                oft[oft_index].setLength(length + 1);
            }
            length++;
            bytes_written++;
        }
        response += std::to_string(bytes_written) + " bytes written";

    } else {
        setResponse("invalid use of wr");
    }
}


void FileSystem::seek(vecstr *in) {
    std::string oft_i = in->front();
    in->erase(in->begin());   
    std::string end_p = in->front();
    in->erase(in->begin());
    if (isInteger(oft_i) && isInteger(end_p)) {
        int oft_index = stoi(oft_i);
        int end_pos = stoi(end_p);
        int curr_pos = oft[oft_index].getCurrentPos();
        // if the bytes being read goes beyond the block, seek to that new block
        int desc_index = oft[oft_index].getIndex();   
        oft[oft_index].seek(end_pos, curr_pos, mem.getDescriptor(desc_index));
        oft[oft_index].setCurrentPos(end_pos); 
        
        // this command is redundant but it outputs the current pos in the oft
        curr_pos = oft[oft_index].getCurrentPos(); 
        setResponse("position is " + std::to_string(curr_pos));
    } else {
        setResponse("invalid use of sk");
    }
}


void FileSystem::listDirectory() {
    std::string all_files;
    int desc_index = 0; // we know that the descriptor index is 0
    int num_desc_per_blk = 16;

    // need to first seek to the first block of the directory
    int curr_pos = oft[OFT_DIR_BLK].getCurrentPos();
    desc_index = oft[OFT_DIR_BLK].getDescIndex(DIR_DESC);
    oft[OFT_DIR_BLK].seek(0, curr_pos, mem.getDescriptor(0));
    oft[OFT_DIR_BLK].resetFiles();
    oft[OFT_DIR_BLK].readDirFromBuffer();

    for (int i = 0; i < 23; i++) {
        // first seek the directory 
        int j = i * SLOT_SIZE;
        if (i % 8 == 0 && i != 0) {
            int next_blk = mem.getBlockLocation(DIR_DESC, j / BLOCK_LENGTH);
            // check and see if we need to allocate a new block
            if (next_blk == -1) {
                break;
            }
            oft[OFT_DIR_BLK].seek(j, j - 1, mem.getDescriptor(0));
            oft[OFT_DIR_BLK].resetFiles();
            oft[OFT_DIR_BLK].readDirFromBuffer();
            // Now we need to place the new block into 
        }
        std::string s = oft[OFT_DIR_BLK].getFileName(i % 8);
        desc_index = oft[OFT_DIR_BLK].getDescIndex(i % 8);
        if (desc_index > -1)
            all_files += s + " ";

        oft[OFT_DIR_BLK].setCurrentPos(j + 4); 
    }
    setResponse(all_files);
}

void FileSystem::save(vecstr *in) {
    std::string disk_name = in->front();
    in->erase(in->begin());
    
    // save the directory block to oft buffer first
    oft[OFT_DIR_BLK].writeDirToBuffer();
    
    // now save the entire oft to the ldisk
    for (int i = 0; i < OFT_SIZE; i++) {
        if (oft[i].getIsEmpty()) 
            continue;
        int curr_pos = oft[i].getCurrentPos();
        int desc_index = oft[i].getIndex(); 
        int dir_blk = mem.getBlockLocation(desc_index, curr_pos / BLOCK_LENGTH); 
        disk.write_block(dir_blk, oft[i].getBuf());
    }

    // save the descriptor to disk
    mem.saveDescriptors();

    // now write the ldisk to a file
    if (disk.saveData(disk_name)) {
        setResponse("disk saved");
    } else {
        setResponse("could not save disk");
    }
}

bool FileSystem::isInteger(std::string s) {
    const char *test = s.c_str();
    char *end;    
    errno = 0;
    long str = strtol(test, &end, 10);

    if (errno == ERANGE || str > std::numeric_limits<int>::max() ||
        str < std::numeric_limits<int>::min() || static_cast<unsigned>(end - test) != s.length()) {
        return false;
    }
    return true;
}

bool FileSystem::isLetter(std::string s) {
    return std::regex_match(s, std::regex("^[A-Za-z]+$"));
}


