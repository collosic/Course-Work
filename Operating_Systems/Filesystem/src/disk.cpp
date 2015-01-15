#include "disk.h"
#include <iostream>



Disk::Disk() {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        for (int j = 0; j < BLOCK_LENGTH; j++) {
            ldisk[i][j] = 0;
        }
    }
}

   

