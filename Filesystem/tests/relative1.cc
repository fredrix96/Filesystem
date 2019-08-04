// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"

/*
    What are we testing?
    * Building trees (same tree as the pwd test-vector) 
    * Walking trees using relative paths for dirs.
*/


class Relative1 : public Program {
public:
    using Program::Program;

    void execute() {
        std::cout << "This test-vector is on the server. Submit your work to see how it executes." << std::endl;
    }
};


int main(int argc, char **argv)
{
   std::cout << "Test program relative1" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Relative1  p(lib);
    p.execute();
}


