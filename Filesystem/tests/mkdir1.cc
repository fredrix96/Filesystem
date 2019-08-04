// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"

/*
    What are we testing?
    * Creating directories in the root
    * Avoiding file/directory collisions
*/


class Mkdir1 : public Program {
public:
    using Program::Program;

    void execute() {
        std::cout << "This test-vector is on the server. Submit your work to see how it executes." << std::endl;
    }
};


int main(int argc, char **argv)
{
   std::cout << "Test program mkdir1" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Mkdir1  p(lib);
    p.execute();
}
