// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include <string.h>
#include <stdint.h>
#include <vector>
#include "base/disk.h"

using namespace std;

class FileSystem
{
private:

  const static int NUMBER_OF_BLOCKS = 2048;
  const static int MAGIC_NUMBER = 133;
  const static int NAME_SIZE = 30;

  // block 0 is reserved for root, 1 for directroies 2-8 are reserved for files and 9 for FAT table
  const static int ROOT_OFFSET = 0;
  const static int DIR_OFFSET = 1;
  const static int FILE_OFFSET = 2;
  const static int FAT_OFFSET = 10;

  // struct SuperBlock
  // {
  //   uint32_t magicNumber;
  //   uint32_t blocks;
  //   uint8_t data[BLOCK_SIZE];
  // };

// should only need 40 bytes: 30 for name, and 2 each for start, length, pos, size and atBlock.
// theoretical limit for nrOfFiles in our FileSystem is 612-ish
  struct File
  {
    const char *fileName = new char[NAME_SIZE];
    uint16_t start;
    // length is nrOfBlocks fully filled by the file
    uint16_t length;
    //
    uint16_t pos;
    // size of the last NOT fully filled block
    uint16_t size;
    // used in read() to know hom many block in we are
    uint16_t atBlock;
  };

  struct Directory
  {
    //file properies
    vector<File> files;
    // 1 block for SuperBlock and then 9 others for directory stuff. The rest is then for files
    // 0 in fat means empty and 1 means it's the end block for the file
    // other numbers (10 or above) are which block is the next one
    // works because blocks 0-9 are reserved for FAT table and directory and whatnot
    uint16_t fat[NUMBER_OF_BLOCKS];
    uint16_t nrOfFiles;
    uint16_t nrOfDirs;

    //directory properties
    // A file or dir name can not go above the size of 30 (name size)
    const char *dirName = new char[NAME_SIZE];
    // theoretical limit for the amout of directories in our FileSystem is 136-ish
    vector<Directory> dirs;
  };

  struct DataBlock
  {
    uint8_t data[BLOCK_SIZE];
  };

  //SuperBlock super;

  // ourFS = root directory
  Directory ourFS;
  // buffer we use for r/w to blocks
  DataBlock dataBlock;

  vector<DataBlock> dataBuff;

public:
    Disk &disk;
    FileSystem(Disk &d);
    intptr_t defaultCWD();
    void format();
    void dump();
    int open(intptr_t CWD, const char *path, uint32_t flags);
    void write(uint32_t fd, uint8_t *buffer, uint32_t n);
    uint32_t read(int fd, uint8_t *buffer, uint32_t numBytes);
    int del(const char *path, intptr_t CWD);
    int mkdir(intptr_t CWD, const char *path);
    intptr_t cd(intptr_t CWD, const char *path);
    int pwd(intptr_t CWD, char *buffer, size_t bufferSize);
    int move(intptr_t CWD, const char *source, const char *target);
    // our functins
    void clearData(uint8_t *data);
    int searchFile(const char *name);
    int searchDir( const char *name);
    void dirStep(Directory dir, char *parentName, uint8_t depth, int pos);
    int searchEmpty();
    int createFile(const char *name);
    void saveFiles();
    void PFStoVFS();

};
