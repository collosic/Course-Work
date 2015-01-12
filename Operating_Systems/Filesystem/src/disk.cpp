#include "disk.h"
#include <iostream>



Disk::Disk() {
    ldisk = new byte*[NUM_BLOCKS];
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        ldisk[i] = new byte[BLOCK_LENGTH];
    }
    ldisk[7][0] = 'a';
}



