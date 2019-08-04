// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <stdio.h>
#include "base/system.h"


/*
    What are we testing?
      * The same sequence of writes as in the write test-vector (because we know
        that it should work already).
      * The data being written survives a round-trip - this is in the read tests.
*/


class Read2 : public Program {
public:
    using Program::Program;

    /* Perform a sequence of reads on the file and verify that the bytes
       received in the buffer are from the recognisable pattern created
       in the previous test program. Use canary checks to make sure the
       buffer does not overflow.

       Note: As this code verifies all the possible paths through the read
       method that you will write, it is substantially longer and more
       complex. Your read method should be much smaller (and simpler) than
       this code.
    */
    void verify(const char *path, size_t size, uint8_t seed) {
        uint32_t bufferSize = seed*seed*seed;
        uint8_t  buffer[bufferSize+2];

        buffer[0] = 17;                // Canary is random number chosen by a fair die.
        buffer[bufferSize+1] = 17;     // Canary is random number chosen by a fair die.

        int fd = libc.open(path, false, false);
        uint32_t offset=0;

        while( offset+bufferSize < size )
        {
          std::cout << "offset, bufferSize" << offset << " " << bufferSize << std::endl;
            uint32_t bytesRead = libc.read(fd, buffer+1, bufferSize);
            if( bytesRead != bufferSize )
            {
                std::cout << "ERROR Only read " << (offset+bytesRead) << " from file "
                          << path << std::endl;
                return;
            }
            if( buffer[0] != 17 )
            {
                std::cout << "ERROR Canary(pre) check failed offset=" << offset << " in file "
                          << path << std::endl;
                return;
            }
            if( buffer[bufferSize+1] != 17 )
            {
                std::cout << "ERROR Canary(post) check failed offset=" << offset << " in file "
                          << path << std::endl;
                return;
            }
            for(uint32_t i=0; i<bufferSize; i++ )
            {
                if( buffer[i+1] != (uint8_t)(seed*(offset+i)) )   // Cast to crop
                {
                    std::cout << "ERROR Wrong data read from file offset=" << offset
                              << " in file " << path << std::endl;
                    return;
                }

            }
            offset += bufferSize;
        }
        // Previous loop grabs all the full buffers. This leaves two possibilities:
        //    1. We are at the end of the file (filesize is a multiple of the bufferSize)
        //    2. There is a left-over piece smaller than bufferSize.
        // Check that the final read returns the correct number of bytes in both cases.
        uint32_t bytesLeft = size - offset;
        buffer[bytesLeft+1] = 17;     // Reset canary position
        uint32_t bytesRead = libc.read(fd, buffer+1, bufferSize);
        if( bytesRead != bytesLeft)
        {
            std::cout << "ERROR Final read returned " << bytesRead << " instead of "
                      << bytesLeft << std::endl;
            return;
        }
        if( buffer[0] != 17 )
        {
            std::cout << "ERROR Canary(pre) check failed in final buffer in file "
                      << path << std::endl;
            return;
        }
        if( buffer[bytesLeft+1] != 17 )
        {
            std::cout << "ERROR Canary(post) check failed in final buffer in file "
                      << path << std::endl;
            return;
        }
        for(uint32_t i=0; i<bytesLeft; i++ )
        {
            if( buffer[i+1] != (uint8_t)(seed*(offset+i)) )    // Cast to crop
            {
                std::cout << "ERROR Wrong data read from final buffer " << i << " in file "
                          << path << std::endl;
                return;
            }

        }
        std::cout << "Verified " << path << std::endl;
        libc.close(fd);
    }

    void execute() {
        verify("twos",   100,       2);
        verify("threes", 4096,      3);
        verify("fives",  20480-123, 5);

        std::cout << "---- second state (should be unchanged) ----" << std::endl;
        libc.dump();
    }
};

int main(int argc, char **argv)
{
    std::cout << std::endl << "Test program read2" << std::endl;
    // Load/create the program and run it. This simulates the link/loader.
    Kernel k;
    LibC   lib(k);
    Read2 p(lib);
    p.execute();
}
