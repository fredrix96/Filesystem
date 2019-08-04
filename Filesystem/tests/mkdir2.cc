// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"

/*
    What are we testing?
    * Creating directories in the root
    * Avoiding file/directory collisions
    * Persistence of the storage.
*/


class Mkdir2 : public Program {
public:
    using Program::Program;

    void execute() {
        std::cout << "This test-vector is on the server. Submit your work to see how it executes." << std::endl;
    }
};


int main(int argc, char **argv)
{
   std::cout << "Test program mkdir2" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Mkdir2  p(lib);
    p.execute();
}

