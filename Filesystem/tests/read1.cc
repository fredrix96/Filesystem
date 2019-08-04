// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"


/*
    What are we testing?
      * That we can recover the same data written into files (using a recognisable
        pattern for the data).
      * That the buffer-sizes / block-sizes are correctly decoupled.
      * The boundary conditions for blocks.
      * The boundary conditions for the end of files.
    What are we not testing?
      * Real-life corner cases: reading from invalid file-descriptors, reading from
        deleted files, reading after close, read conditions when the disk is full...
*/


class Read1 : public Program {
public:
    using Program::Program;

    // Simple helper to simplify the test-case.
    void openAndWrite(const char *path, size_t size, uint8_t seed) {
        uint16_t fd = libc.open(path,true,true);
        if(fd<0) {
            std::cout << "Well this does not look good." << std::endl;
            return;
        }
        uint8_t  buffer[BLOCK_SIZE*10];   // Assume each call has size <= sizeof(buffer)
        for(size_t j=0; j<size; j++)
            buffer[j] = seed*j;
        libc.write(fd,buffer,size);
        libc.close(fd);
    }


    void execute() {
       libc.format();

       openAndWrite("twos",   100,       2);       // Tiny wee bit of a block
       openAndWrite("threes", 4096,      3);       // Whole block
       openAndWrite("fives",  20480-123, 5);       // Almost five blocks

       std::cout << "---- first state ----" << std::endl;
       libc.dump();
    }
};

int main(int argc, char **argv)
{
    std::cout << "Test program read1" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Read1 p(lib);
    p.execute();
}
