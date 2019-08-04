// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"


/*
    What are we testing?
      * The write call is correct allocating space and updating the file metadata.
      * The file metadata is correctly shown in the dump format.
    What are we not testing (yet)?
      * The data being written survives a round-trip - this is in the read tests.

*/


class Write1 : public Program {
public:
    using Program::Program;

    // Simple helper to simplify the test-case.
    void forceOpenAndWrite(const char *path, size_t size) {
        uint16_t fd = libc.open(path,true,true);
        if(fd<0) {
            std::cout << "Well this does not look good." << std::endl;
            return;
        }
        uint8_t  buffer[4096];
        for(size_t i=0; i<size/4096; i++) {
            // Fill the buffer with a recognisable pattern. This does not affect the
            // test result (although we will do the same in the reading tests), but it
            // does allow you to hexdump parts of disk.iso to check that your code is
            // working.
            for(size_t j=0; j<4096; j++) {
                buffer[j] = i*j;
            }
            libc.write(fd,buffer,4096);
        }
        if(size%4096!=0) {
            size_t leftOver = size%4096;
            memset(buffer,0,4096);
            for(size_t i=0; i<leftOver; i++)
                buffer[i] = i;
            libc.write(fd,buffer,leftOver);
        }
        libc.close(fd);
    }


    void execute() {
       libc.format();

       forceOpenAndWrite("small", 100);         // Tiny wee bit of a block
       forceOpenAndWrite("medium", 4096);       // Whole block
       forceOpenAndWrite("large", 20480);       // Five blocks

       std::cout << "---- first state ----" << std::endl;
       libc.dump();
    }
};

int main(int argc, char **argv)
{
    std::cout << "Test program write1" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Write1 p(lib);
    p.execute();
}
