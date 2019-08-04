// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"

/*
    What are we testing?
    * That we can move files over local/relative/absolute paths.
    * That collisions are avoided.
    * That illegal targets are avoided.
*/


class Move3 : public Program {
public:
    using Program::Program;


    void execute() {
        std::cout << "This test-vector is on the server. Submit your work to see how it executes." << std::endl;
    }
};


int main(int argc, char **argv)
{
    std::cout << "Test program move3" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Move3  p(lib);
    p.execute();
}




