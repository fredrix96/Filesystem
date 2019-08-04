// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"


/*
    What are we testing?
      * Initialising the empty disk
      * Correct output format for the dump
*/


class Emptydump : public Program {
public:
    using Program::Program;
    // Simple helper to simplify the test-case.
    void createOrDie(const char *path, const char *msg)
    {
         int fd=libc.open(path,true,false);
         if(fd<0) {
            std::cout << msg << std::endl;
            exit(-1);
         }
         libc.close(fd);
    }

    void execute() {
       std::cout << "Test program emptydump" << std::endl;
       libc.format();
       libc.dump();             // Initial (empty) state of the disk.
    }
};


int main(int argc, char **argv)
{
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Emptydump  p(lib);
    p.execute();
}

