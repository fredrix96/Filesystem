// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include "base/disk.h"

void Disk::read(uint32_t block, uint8_t *buffer)
{
    uint32_t offset = block * BLOCK_SIZE + (1<<23);  // Partition starts at 8MB.
    // A real disk would not stop the system like this, but this will make it 
    // easier for you to find problems in the file-system during development.
    if( offset >= (1<<24) )
    {
        std::cout << "ERROR: Attempt to access block " << block << " out of range" 
                  << std::endl;
        exit(-1);
    }
    backing.seekg(offset, std::ios_base::beg);
    backing.read((char*)buffer, BLOCK_SIZE);
}

void Disk::write(uint32_t block, uint8_t *buffer)
{
    uint32_t offset = block * BLOCK_SIZE + (1<<23);  // Partition starts at 8MB.
    // A real disk would not stop the system like this, but this will make it 
    // easier for you to find problems in the file-system during development.
    if( offset >= (1<<24) )
    {
        std::cout << "ERROR: Attempt to access block " << block << " out of range" 
                  << std::endl;
        exit(-1);
    }
    backing.seekp(offset, std::ios_base::beg);
    backing.write((char*)buffer, BLOCK_SIZE);
    backing.flush();
}

int Disk::size()
{
    return backingSize;
}

/* The disk is simulated by a real .iso disk image in the file-system. This file
   is opened when the simulation starts, and remains open for the lifetime of
   the system. The disk is required to be a 16MB image so that we can use the
   third partition of 8MB as the data drive for the OS. This means that the disk
   images in the C++ simulation are compatible with the images in the real C
   code. This should help if you port your code to the real system for the higher
   grades.
*/
Disk::Disk() 
{
    backing.open("disk.iso", std::ios::in | std::ios::out | std::ios::binary);
    if(!backing.is_open())
    {
        std::cout << "ERROR: Unable to open disk image" << std::endl;
        exit(-1);
    }
    backing.seekg(0, backing.end);
    backingSize = backing.tellg();
    if(backingSize != (1<<24))    // 16MB
    {
        std::cout << "ERROR: Disk image is wrong size" << std::endl;
        exit(-1);
    }
    std::cout << "Disk image available: " << backingSize << " bytes" << std::endl;
}

