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
