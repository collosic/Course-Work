#include "disk.h"
#include <iostream>



Disk::Disk() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        for (int j = 0; j < BLOCK_LENGTH; j++) {
            ldisk[i][j] = 0;
        }
    }
}

void Disk::read_block(int blk_num, char *b) {
    for (int i = 0; i < BLOCK_LENGTH; i++) {
        b[i] = ldisk[blk_num][i];
    }
}
   
void Disk::write_block(int blk_num, char *p) {
    // write the incoming block back to the ldisk
    for (int j = 0; j < BLOCK_LENGTH; j++) {
        ldisk[blk_num][j] = p[j];
    }
}

bool Disk::saveData(std::string disk_name) {
    // write the 2d array to a file
    std::ofstream out_file(disk_name);
    if (!out_file) {
        // this is a major error hopefully this never happens
        return false;
    } else {
        for (int i = 0; i < NUM_BLOCKS; i++) {
            for (int j = 0; j < BLOCK_LENGTH; j++) {
                out_file << ldisk[i][j];
                ldisk[i][j] = 0;
            }
        } 
        out_file.close();
    }
    return true;
}

bool Disk::loadData(std::string disk_name) {
   // open and read in the file 
    std::ifstream in_file(disk_name);
    //in_file.seekg(0, std::ios::beg);
    char temp;

    // check and see if we opened the file correctly or wrong name
    if (!in_file) 
        return false;

    for (int i = 0; i < NUM_BLOCKS; i++) {
        for (int j = 0; j < BLOCK_LENGTH; j++) {
            in_file.get(temp);
            ldisk[i][j] = temp;
        }
    }
    in_file.close();
    return true;
}
