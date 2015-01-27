#include "disk.h"
#include <iostream>



Disk::Disk() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        for (int j = 0; j < BLOCK_LENGTH; j++) {
            ldisk[i][j] = 0;
        }
    }
}

void Disk::read_block(int blk_num, byte *b) {
    for (int i = 0; i < BLOCK_LENGTH; i++) {
        b[i] = ldisk[blk_num][i];
    }
}
   
void Disk::write_block(int blk_num, byte *p) {
    // write the incoming block back to the ldisk
    for (int j = 0; j < BLOCK_LENGTH; j++) {
        ldisk[blk_num][j] = p[j];
    }
}

void Disk::saveData(std::string disk_name) {
    // write the 2d array to a file
    std::ofstream out_file(disk_name);
    if (!out_file) {
        // this is a major error hopefully this never happens
        std::cerr << "There was an error opening " << disk_name << std::endl;
    } else {
        for (int i = 0; i < NUM_BLOCKS; i++) {
            for (int j = 0; j < BLOCK_LENGTH; j++) {
                std::cout << ldisk[i][j] << std::endl;
                out_file << ldisk[i][j];
                ldisk[i][j] = 0;
            }
        } 
        out_file.close();
    }
    
}

int Disk::loadData(std::string disk_name) {
   // open and read in the file 
    std::ifstream in_file(disk_name); 

    // check and see if we opened the file correctly or wrong name
    if (!in_file) 
        return -1;

    for (int i = 0; i < NUM_BLOCKS; i++) {
        for (int j = 0; j < BLOCK_LENGTH; j++) {
            in_file >> ldisk[i][j]; 
        }
    }
    return 0;
}
