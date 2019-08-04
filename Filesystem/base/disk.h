// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#pragma once
#include <cstdint>
#include <fstream>
#include <iostream>

class Disk {
    std::fstream backing;
    int backingSize;
public:
    void read(uint32_t block, uint8_t *buffer);
    void write(uint32_t block, uint8_t *buffer);
    int  size();
    Disk();
};

#define BLOCK_SIZE 4096
