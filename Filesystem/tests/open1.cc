// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"


/*
    What are we testing?
      * Locating the directory block
      * Creating an entry
      * Writing the structure correctly back to disk

*/


class Open1 : public Program {
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
       libc.format();
       libc.dump();             // Initial (empty) state of the disk.

       std::cout << "---- first state ----" << std::endl;
       createOrDie("file.txt", "ERROR - can't open file.txt");
       libc.dump();
       std::cout << "---- second state ----" << std::endl;
       createOrDie("file2.txt", "ERROR - can't open file2.txt");
       libc.dump();
       std::cout << "---- third state ----" << std::endl;
       createOrDie("longer file.name.txt", "ERROR - can't open longer file.name.txt");
       libc.dump();
    }
};


int main(int argc, char **argv)
{
   std::cout << "Test program open1" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Open1  p(lib);
    p.execute();
}
