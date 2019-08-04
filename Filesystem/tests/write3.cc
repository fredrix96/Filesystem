// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"


/*
    What are we testing?
      * Passing buffers larger than a single block to the calls.
      * Corner cases for where the data crosses block-boundaries.
    What are we not testing (yet)?
      * The data being written survives a round-trip - this is in the read tests.

*/

class Write3 : public Program {
public:
    using Program::Program;

    // Simple helper to simplify the test-case.
    void forceOpenAndWrite(const char *path, size_t size) {
        uint16_t fd = libc.open(path,true,true);
        if(fd<0) {
            std::cout << "Well this does not look good." << std::endl;
            return;
        }
        uint8_t  buffer[20480];     // Ten blocks
        for(size_t i=0; i<size/20480; i++) {
            // Fill the buffer with a recognisable pattern. This does not affect the
            // test result (although we will do the same in the reading tests), but it
            // does allow you to hexdump parts of disk.iso to check that your code is
            // working.
            for(size_t j=0; j<20480; j++) {
                buffer[j] = i*j;
            }
            libc.write(fd,buffer,20480);
        }
        if(size%20480!=0) {                   // This is 10x the size in the previous tests
            size_t leftOver = size%20480;
            memset(buffer,0,20480);
            for(size_t i=0; i<leftOver; i++)
                buffer[i] = i;
            libc.write(fd,buffer,leftOver);
        }
        libc.close(fd);
    }


    void execute() {
       forceOpenAndWrite("small", 1000);         // Now spans two blocks (1455 bytes)
       forceOpenAndWrite("medium", 4095+4096+1); // Fill third block, entire 4th and partial 5th.
       forceOpenAndWrite("large", 204700);       // 120 blocks minus a little bit

       std::cout << "---- third state ----" << std::endl;
       libc.dump();
    }
};

int main(int argc, char **argv)
{
    std::cout << std::endl << "Test program write3" << std::endl;

    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Write3 p(lib);
    p.execute();
}
