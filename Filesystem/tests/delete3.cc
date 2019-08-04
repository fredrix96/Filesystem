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


class Delete3 : public Program {
public:
    using Program::Program;

    /* If the implementation of delete is leaking space then the disk will fill
       causing this test to fail. Leave one of the old files in place to check
       the directory block is not being damaged and then repeatedly allocate,
       then free space on the disk until we have exceeded the size of the disk. */
    void execute() {
        std::cout << "This test-vector is on the server. Submit your work to see how it executes." << std::endl;
    }
};

int main(int argc, char **argv)
{
    std::cout << std::endl << "Test program delete3" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Delete3 p(lib);
    p.execute();
}




