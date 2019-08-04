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
class Open2 : public Program {
public:
    using Program::Program;

    void checkIfOpens(const char *path, bool shouldWork, const char *msg)
    {
       int fd=libc.open(path,false,false);
       if( (fd<0 && shouldWork) || (fd>=0 && !shouldWork) )
       {
          std::cout << msg << std::endl;
          exit(-1);
       }
       libc.close(fd);
    }

    void execute() {
       std::cout << "------ state before test ----------" << std::endl;
       libc.dump();             // Check the state matches the end of open1
       checkIfOpens("should not be found", false, "ERROR - opened a file that does not exist!");
       checkIfOpens("file2.txt",           true,  "ERROR - could not reopen existing file!");
       std::cout << "------ state after test (should be same) ------" << std::endl;
       libc.dump();             // State should be unchanged
    }
};


int main(int argc, char **argv)
{
    std::cout << std::endl << "Test program open2" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Open2  p(lib);
    p.execute();
}
