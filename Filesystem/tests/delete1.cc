// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"


/*
    What are we testing?
      * That the delete call removes access to existing files.
      * That the delete call fails on non-existing files.
      * That the occupied space is recovered.
    What are we not testing?
      * The effect on directories (not in the assignment until later).
      * The effect on links       (not in the assignment at all).
*/


class Delete1 : public Program {
public:
    using Program::Program;


    void execute() {
        std::cout << "This test-vector is on the server. Submit your work to see how it executes." << std::endl;
    }
};

int main(int argc, char **argv)
{
    std::cout << "Test program delete1" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Delete1 p(lib);
    p.execute();
}
