// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include "filesystem.h"

using namespace std;

FileSystem::FileSystem(Disk &d) : disk(d)
{
  // Reading all the neccessary information for FAT such at filenames, where they start and how big they are.
  PFStoVFS();
}


intptr_t FileSystem::defaultCWD()
{
    return (intptr_t)ourFS.dirName;          // Pick a representation of directories that can cast into the return type.
}


void FileSystem::format()
{
    //std::cout << "format: not implemented" << std::endl;

    super.magicNumber = MAGIC_NUMBER;
    super.blocks = NUMBER_OF_BLOCKS;
    clearData(super.data);
    super.data[0] = super.magicNumber;
    uint8_t tmp_i = 0;
    for(uint8_t i = 1; i < super.blocks; i++)
    {
      super.data[i] = 255;
      super.blocks -= 255;
      tmp_i = i;
    }
    super.data[tmp_i + 1] = 8;
    super.blocks = NUMBER_OF_BLOCKS;
    disk.write(0, super.data);

    //create root dir
    ourFS.dirName = "/root";

    // Filling the disk with root-data

    // filename
    /*int tmp = 0;
    for(int i = 0; i < NAME_SIZE - 1; i++)
    {
      if (ourFS[0].fileName != NULL)
      {
        ourFS[0].data[i] = ourFS[0].fileName[i];
      }
      tmp += 1;
    }*/

    // dirname
    clearData(dataBlock);
    for(int i = 0; i < NAME_SIZE; i++)
    {
      dataBlock[i] = ourFS.dirName[i];
    }
    disk.write(1, dataBlock);
    clearData(dataBlock);

    // start pos
    /*ourFS[0].data[tmp] = (ourFS[0].start >> 8) & 0x00FF;
    tmp += 1;
    ourFS[0].data[tmp] = ourFS[0].start & 0x00FF;
    tmp += 1;

    // number of blocks
    ourFS[0].data[tmp] = (ourFS[0].length >> 8) & 0x00FF;
    tmp += 1;
    ourFS[0].data[tmp] = ourFS[0].length & 0x00FF;
    tmp += 1;

    // buffer position
    ourFS[0].data[tmp] = (ourFS[0].pos >> 8) & 0x00FF;
    tmp += 1;
    ourFS[0].data[tmp] = ourFS[0].pos & 0x00FF;
    tmp += 1;

    // next block position
    ourFS[0].data[tmp] = (ourFS[0].nextBPos >> 8) & 0x00FF;
    tmp += 1;
    ourFS[0].data[tmp] = ourFS[0].nextBPos & 0x00FF;
    tmp += 1;

    disk.write(1, ourFS[0].data);*/


    //clear all other blocks
    /*for(uint64_t i = 2; i < NUMBER_OF_BLOCKS; i++)
     {
       ourFS[i].fileName = NULL;
       ourFS[i].dirName = NULL;
       ourFS[i].start = -1;
       ourFS[i].length = 0;
       ourFS[i].pos = -1;
       ourFS[i].nextBPos = 0;
       dataBlock[BLOCK_SIZE] = { 0 };

       // Filling the disk with clear-data
       // filename
       /*int tmp = 0;
       for(int i = 0; i < NAME_SIZE; i++)
       {
         ourFS[0].data[i] = ourFS[0].fileName[i];
         tmp += 1;
       }

       // dirname
       for(int i = tmp; i < tmp * 2; i++)
       {
         ourFS[0].data[i] = ourFS[0].dirName[i];
         tmp += 1;
       }

       // start pos
       ourFS[0].data[tmp] = (ourFS[0].start >> 8) & 0x00FF;
       tmp += 1;
       ourFS[0].data[tmp] = ourFS[0].start & 0x00FF;
       tmp += 1;

       // number of blocks
       ourFS[0].data[tmp] = (ourFS[0].length >> 8) & 0x00FF;
       tmp += 1;
       ourFS[0].data[tmp] = ourFS[0].length & 0x00FF;
       tmp += 1;

       // buffer position
       ourFS[0].data[tmp] = (ourFS[0].pos >> 8) & 0x00FF;
       tmp += 1;
       ourFS[0].data[tmp] = ourFS[0].pos & 0x00FF;
       tmp += 1;

       // next block position
       ourFS[0].data[tmp] = (ourFS[0].nextBPos >> 8) & 0x00FF;
       tmp += 1;
       ourFS[0].data[tmp] = ourFS[0].nextBPos & 0x00FF;
       tmp += 1;

       disk.write(i, dataBlock);
     }*/
}


void FileSystem::dump() {
    //std::cout << "dump: not implemented" << std::endl;

    //read SuperBlock
    disk.read(0, dataBlock);
    cout << "------------SuperBlock-------------" << endl;
    cout << "MagicNumber: " << (int)dataBlock[0] << endl;
    uint32_t totalBlocks = 0;
    uint32_t nrOfBlocks = NUMBER_OF_BLOCKS;
    for(uint8_t i = 1; i < nrOfBlocks; i++)
    {
      totalBlocks += (int)dataBlock[i];
      nrOfBlocks -= 255;
    }
    totalBlocks += 8; // Cough cough
    cout << "Total blocks: " << totalBlocks << endl;
    cout << "-----------------------------------" << endl;




    cout << "---------------Dir------------------" << endl;
    cout << "DirName: " << ourFS.dirName << endl;
    cout << "Starts at: " << (int)1 << endl;
    cout << "Directory occupies " << (int)(FAT_OFFSET - 1) << " blocks" << endl;
    cout << "------------------------------------" << endl;



    // filename
    /*for(int i = tmp; i < NAME_SIZE * 2 + 2; i++)
    {
      ourFS.fileName += temp.data[i];
      tmp += 1;
    }*/

    /*for(uint8_t i = 0; i < NUMBER_OF_BLOCKS - 1; i++)
    {
      disk.read(i + 1, temp[i].data);

      // Filling the virtual fs with disk data
      // filename
      int tmp = 0;
      for(int i = 0; i < NAME_SIZE; i++)
      {
        ourFS[0].fileName += temp[i].data[i];
        tmp += 1;
      }

      // dirname
      for(int i = tmp; i < tmp * 2; i++)
      {
        ourFS[0].dirName += temp[i].data[i];
        tmp += 1;
      }

      // start pos
      ourFS[0].start = 0x0000;
      ourFS[0].start = temp[i].data[tmp];
      tmp += 1;
      ourFS[0].start = ourFS[0].start << 8;
      ourFS[0].start |= temp[i].data[tmp];
      tmp += 1;

      // number of blocks
      ourFS[0].length = 0x0000;
      ourFS[0].length = temp[i].data[tmp];
      tmp += 1;
      ourFS[0].length = ourFS[0].start << 8;
      ourFS[0].length |= temp[i].data[tmp];
      tmp += 1;

      // buffer position
      ourFS[0].pos = 0x0000;
      ourFS[0].pos = temp[i].data[tmp];
      tmp += 1;
      ourFS[0].pos = ourFS[0].start << 8;
      ourFS[0].pos |= temp[i].data[tmp];
      tmp += 1;

      // next block position
      ourFS[0].nextBPos = 0x0000;
      ourFS[0].nextBPos = temp[i].data[tmp];
      tmp += 1;
      ourFS[0].nextBPos = ourFS[0].start << 8;
      ourFS[0].nextBPos |= temp[i].data[tmp];
      tmp += 1;
    }
*/


    cout << "--------------FAT------------------" << endl;
    for(int i = FAT_OFFSET; i < NUMBER_OF_BLOCKS; i = i + 4)
    {
      cout << i << " | " << ourFS.fat[i] << " || ";
      cout << i+1 << " | " << ourFS.fat[i+1] << " || ";
      if( (i + 2) != 2048)
      {
        cout << i+2 << " | " << ourFS.fat[i+2] << " || ";
        cout << i+3 << " | " << ourFS.fat[i+3] << endl;
      }
      else
      {
        cout << endl;
      }
    }
    cout << "-----------------------------------" << endl;
    cout << "Dump nrOfFiles: " << ourFS.nrOfFiles << endl;
    for(int i = 0; i < ourFS.nrOfFiles; i++)
    {
      cout << "--------------File-----------------" << endl;
      cout << "FileName: " << ourFS.files.at(i).fileName << endl;
      cout << "Starts at block: " << (int)ourFS.files.at(i).start << endl;
      cout << "Filled blocks (length): " << (int)ourFS.files.at(i).length << endl;
      cout << "-----------------------------------" << endl;
    }


    cout << "-------------Summary----------------" << endl;
    cout << "Number of files: " << ourFS.nrOfFiles << endl;
    cout << "------------------------------------" << endl;

}


int FileSystem::open(intptr_t CWD, const char *path, uint32_t flags)
{
    int found = searchFile(path);
    if(found > -1)
    {
      cout << "Opening file " << path << endl;

    }
    else if(flags)
    {
      found = createFile(path);
      if(found == -1)
      {
        cout << "Filesystem full. File not created!" << endl;
      }

    }

    return found;
}


void FileSystem::write(uint32_t fd, uint8_t *buffer, uint32_t size)
{
  //std::cout << "write(" << fd << ", " << (intptr_t)buffer << ", " << size << "): not implemented" << std::endl;
  cout << " BUFFER::= ";
  for(uint32_t i = 0; i < size; i++)
  {
    cout << (int)buffer[i];
  }
  cout << endl;

  size_t counter = 0;
  int next = 0;
  int filePos = 0;
  uint32_t currentBlock = fd;
  vector<int> fatPos;

  // find the last block in the
  while (ourFS.fat[currentBlock] != 1)
  {
    currentBlock = ourFS.fat[currentBlock];
  }

  // searching through FAT table
  for(uint32_t i = FAT_OFFSET; i < NUMBER_OF_BLOCKS; i++)
  {
    // found start of the file
    if(i == currentBlock)
    {
      cout << "i==fd" << endl;
      // check to find the position of the file we are writing to in our files vector
      // this is so that we can update the files length.
      for(uint32_t j = 0; j < ourFS.files.size(); j++)
      {
        if(ourFS.files.at(j).start == fd)
        {
          filePos = j;
          //ourFS.files.at(filePos).length = 1;
        }
      }

      fatPos.push_back(fd);
      clearData(dataBlock);

      //write to file
      for(size_t j = 0; j < size; j++)
      {
        dataBlock[counter] = buffer[j];

        if( counter == BLOCK_SIZE - 1)
        {
          cout << "i = " << i << endl;
          cout << "size =  " << size << endl;
          counter = 0;
          // if our file has a next block we want to write to that one
          // so as not to have random FAT table positions pointing to random places
          // 1 in our FAT table means it's the last block in the chain
          cout << "ourFS.fat[currentBlock] " << ourFS.fat[currentBlock] << endl;
          if(ourFS.fat[currentBlock] == 1)
          {
            cout << "ourFS.fat[i] == 1" << endl;
            next = searchEmpty();
            if(next != -1)
            {
              cout << "1 next != -1" << endl;
              fatPos.push_back(next);
              ourFS.fat[currentBlock] = next;
              currentBlock = next;
              ourFS.fat[next] = 1;
              ourFS.files.at(filePos).length += 1;
              cout << "2 next != -1" << endl;
              dataBuff.push_back(dataBlock);
              cout << "3 next != -1" << endl;
              clearData(dataBlock);
              cout << "4 next != -1" << endl;
            }
            else
            {
              cout << "Filesystem full! Data could NOT be written!\n Aborting write... " << endl;
              break;
            }
          }
        }
        else
        {
          counter++;
        }
      }

      // Push back data as long as we have something more to push
      if ( counter != 0)
      {
        cout << "counter != 0" << endl;
        //ourFS.files.at(filePos).length += 1;
        dataBuff.push_back(dataBlock);
        clearData(dataBlock);
      }

      if(next != -1)
      {
        cout << "next!=-1 file.length = " << ourFS.files.at(filePos).length << endl;
        //cout << dataBuff.size() << endl;
        for(size_t i = 0; i < dataBuff.size(); i++)
        {


          /*cout << " 1BUFFER::= ";
          for(uint32_t i = 0; i < 8; i++)
          {
            cout << (int)dataBuff.at(i)[i];
          }
          cout << endl;*/



          disk.write(fatPos.at(i), dataBuff.at(i));

          clearData(dataBlock);
          disk.read(fatPos.at(i), dataBlock);

          cout << "BUFFER AFTER WRITE::= ";
          for(uint32_t i = 0; i < 125; i++)
          {
            cout << (int)dataBlock[i];
          }
          cout << endl;
          clearData(dataBlock);

          saveFiles();
        }
      }
      dataBuff.clear();
      break;
    }
  }
}

uint32_t FileSystem::read(int fd, uint8_t *buffer, uint32_t numBytes)
{
    //std::cout << "read(" << fd << ", " << (intptr_t)buffer << ", " << numBytes << "): not implemented" << std::endl;

    int currentBlock = fd;
    std::cout << "read fd " << fd << std::endl;
    uint32_t bytesRead = 0;

    while(bytesRead < numBytes)
    {

      clearData(dataBlock);
      disk.read(currentBlock, dataBlock);

      cout << " IN READ MOTHA FUCKA ourBUFFER::= ";
      for(uint32_t i = 0; i < numBytes; i++)
      {
        cout << (int)dataBlock[i];
      }
      cout << endl;

      for(uint32_t i = 0; i < BLOCK_SIZE; i++)
      {
        buffer[i] = dataBlock[i];
        bytesRead++;
        if(bytesRead >= numBytes)
        {
          cout << "bytesRead " << bytesRead << std::endl;
          cout << "numBytes " << numBytes << std::endl;
          break;
        }

        if(i == BLOCK_SIZE -1)
        {
          std::cout << " i == BLOCK_SIZE -1 " << std::endl;
          currentBlock = ourFS.fat[currentBlock];
        }

      }
    }

    cout << " BUFFER::= ";
    for(uint32_t i = 0; i < numBytes; i++)
    {
      cout << (int)buffer[i];
    }
    cout << endl;

    return bytesRead;
}


int FileSystem::del(const char *path, intptr_t CWD)
{
    std::cout << "del(" << path << ", " << CWD << "): not implemented" << std::endl;
    return -1;
}


int FileSystem::mkdir(intptr_t CWD, const char *path)
{
    std::cout << "mkdir(" << CWD << ", " << path << "): not implemented" << std::endl;
    return -1;
}


intptr_t FileSystem::cd(intptr_t CWD, const char *path)
{
    std::cout << "cd(" << CWD << ", " << path << "): not implemented" << std::endl;
    return -1;
}


int FileSystem::pwd(intptr_t CWD, char *buffer, size_t bufferSize)
{
    std::cout << "pwd(" << CWD << ", " << (intptr_t)buffer << ", " << bufferSize << "): not implemented" << std::endl;
    return -1;
}


int FileSystem::move(intptr_t CWD, const char *source, const char *target)
{
    std::cout << "move(" << CWD << ", " << source << ", " << target << "): not implemented" << std::endl;
    return -1;
}

// start of our own functions

void FileSystem::clearData(uint8_t *data)
{
  for(uint64_t i = 0; i < NUMBER_OF_BLOCKS; i++)
  {
    data[i] = 0;
  }
}

int FileSystem::searchFile(const char *name)
{
  int found = -1;
  // if we have no files in our vector it doesn't go in the loop so returns -1
  //cout << "Size of our files vector: " << ourFS.files.size() << endl;
  for(uint32_t i = 0; i < ourFS.files.size() ; i++)
  {
    //cout << "Search for file in FAT at pos: " << i << endl;
    if( strcmp(ourFS.files.at(i).fileName, name) == 0)
    {
      found = ourFS.files.at(i).start;
      cout << "File with name " << name << " has been found!" << endl;
      break;
    }
  }

  return found;
}

int FileSystem::searchEmpty()
{
  int found = -1;
  //  searching for the first empty slot in our fat table
  // returns -1 if our filesystem is full
  cout << "searchEmpty" << endl;
  for(uint32_t i = FAT_OFFSET; i < BLOCK_SIZE ; i++)
  {
    if( ourFS.fat[i] == 0 )
    {
      found = i;
      break;
    }
  }

  return found;
}

int FileSystem::createFile(const char *name)
{
  int stored = -1;
  File temp;
  for(int i = FAT_OFFSET; i < BLOCK_SIZE; i++)
  {
    if(ourFS.fat[i] == 0 )
     {
       ourFS.fat[i] = 1;
       temp.fileName = name;
       temp.start = i;
       temp.length = 0;
       temp.pos = 0;
       ourFS.files.push_back(temp);
       stored = i;
       ourFS.nrOfFiles += 1;
       cout << "Creating file with name " << temp.fileName << endl;
       saveFiles();
       break;
     }
  }
  return stored;
}


// saves all nesseccary info for FAT to the disk
void FileSystem::saveFiles()
{
  // save the FAT table in block 9
  clearData(dataBlock);
  int counter = 0;
  for (int i = FAT_OFFSET; i < NUMBER_OF_BLOCKS; i++)
  {
    dataBlock[counter] = ((ourFS.fat[i] & 0xFF00 ) >> 8);
    counter += 1;
    dataBlock[counter] = (ourFS.fat[i] & 0x00FF );
    counter += 1;
  }
  disk.write(FAT_OFFSET-1, dataBlock);


  clearData(dataBlock);
  for(int i = 0; i < NAME_SIZE; i++)
  {
    dataBlock[i] = ourFS.dirName[i];
  }
  cout << "Saving nrOfFiles: " << ourFS.nrOfFiles << endl;
  int tmp = NAME_SIZE;

  dataBlock[tmp] = ((ourFS.nrOfFiles & 0xFF00) >> 8);
  tmp += 1;
  dataBlock[tmp] = (ourFS.nrOfFiles & 0x00FF);
  tmp += 1;
  disk.write(1, dataBlock);
  cout << "Saved, now nrOfFiles = " << ourFS.nrOfFiles << endl;

  // Writing all the file struct to Disk
  int i = 0;
  int k = 0;
  int j = 2;
  int lengthOfFile = (NAME_SIZE + 6);
  File temp;
  clearData(dataBlock);
  while(k < ourFS.nrOfFiles)
  {
    if(i == BLOCK_SIZE)
    {
      i = 0;
      disk.write(j, dataBlock);
      clearData(dataBlock);
      j++;
    }
    int m = 0;
    for(int l = i * lengthOfFile; l < (i * lengthOfFile) + lengthOfFile; l++)
    {
      if( m < 30)
      {
        //cout << "m < 30, fileName: " << ourFS.files.at(k).fileName[m] << " l, i = " << l << " " << i << endl;
        dataBlock[l] = ourFS.files.at(k).fileName[m];
        //cout << "dataBlock[l]" << dataBlock[l] << endl;
      }
      else if( m < 31)
      {
        //  cout << "m < 31" << endl;
        dataBlock[l] = 0x0000;
        dataBlock[l] = ((ourFS.files.at(k).start & 0xFF00) >> 8);
      }
      else if( m < 32)
      {
        //  cout << "m < 32" << endl;
        dataBlock[l] = ((ourFS.files.at(k).start & 0x00FF));
      }
      else if( m < 33)
      {
        //  cout << "m < 33" << endl;
        dataBlock[l] = 0x0000;
        dataBlock[l] = ((ourFS.files.at(k).length & 0xFF00) >> 8);
      }
      else if(m < 34)
      {
        //  cout << "m < 34" << endl;
        dataBlock[l] = ((ourFS.files.at(k).length & 0x00FF));
      }
      else if( m < 35)
      {
        //  cout << "m < 35" << endl;
        dataBlock[l] = 0x0000;
        dataBlock[l] = ((ourFS.files.at(k).pos & 0xFF00) >> 8);
      }
      else
      {
        //  cout << "m == 35" << endl;
        dataBlock[l] = ((ourFS.files.at(k).pos & 0x00FF));
      }
      m++;
    }

    i++;
    k++;
  }
  //cout << "before saves write" << endl;
  disk.write(j, dataBlock);
  cout << "after saves write" << endl;
}


void FileSystem::PFStoVFS()
{
  // Filling the virtual fs with disk data
  // reading in the FAT table
  clearData(dataBlock);
  disk.read(FAT_OFFSET-1, dataBlock);
  int counter = 0;
  for (int i = FAT_OFFSET; i < NUMBER_OF_BLOCKS; i++)
  {
    // the 0x0000 isn't really needed but good for clarity
    ourFS.fat[i] = 0x0000;
    ourFS.fat[i] = dataBlock[counter];
    counter += 1;
    ourFS.fat[i] = ourFS.fat[i] << 8;
    ourFS.fat[i] |= dataBlock[counter];
    counter += 1;
  }
  clearData(dataBlock);
  cout << "Before reading in nrOfFiles are: " << ourFS.nrOfFiles << endl;

  disk.read(1, dataBlock);
  // dirname
  for(int i = 0; i < NAME_SIZE; i++)
  {// the check should work since if dirName has a zero in it it should be ascii 60
    if(dataBlock[i] != 0)
    {
      ourFS.dirName += dataBlock[i];
    }
  }

  // Checking how many files we have (comes directly after the name of the directory)
  int tmp = NAME_SIZE;
  ourFS.nrOfFiles = 0x0000;
  ourFS.nrOfFiles = dataBlock[tmp];
  tmp += 1;
  ourFS.nrOfFiles = ourFS.nrOfFiles << 8;
  ourFS.nrOfFiles |= dataBlock[tmp];
  tmp += 1;
  cout << "After read nrOfFiles: " << ourFS.nrOfFiles << endl;
  clearData(dataBlock);

  // Reading in all the file structs to VFS
  int i, k = 0;
  int j = 2;
  int lengthOfFile = (NAME_SIZE + 6);
  File temp;
  char *fileName = new char[NAME_SIZE];
  disk.read(j, dataBlock);
  while(k < ourFS.nrOfFiles)
  {
    if(i == BLOCK_SIZE)
    {
      j++;
      i = 0;
      clearData(dataBlock);
      disk.read(j, dataBlock);
    }

    int m = 0;
    for(int l = i * lengthOfFile; l < i * lengthOfFile + lengthOfFile; l++)
    {
      if( m < 30)
      {
        fileName[m] = dataBlock[l];
      }
      else if( m < 31)
      {
        temp.start = 0x0000;
        temp.start = dataBlock[l];
      }
      else if( m < 32)
      {
        temp.start = temp.start << 8;
        temp.start |= dataBlock[l];
      }
      else if( m < 33)
      {
        temp.length = 0x0000;
        temp.length = dataBlock[l];
      }
      else if(m < 34)
      {
        temp.length = temp.length << 8;
        temp.length |= dataBlock[l];
      }
      else if( m < 35)
      {
        temp.pos = 0x0000;
        temp.pos = dataBlock[l];
      }
      else
      {
        temp.pos = temp.pos << 8;
        temp.pos |= dataBlock[l];
      }
      m++;
    }
    temp.fileName = fileName;
    fileName = new char[NAME_SIZE];
    ourFS.files.push_back(temp);

    i++;
    k++;
  }
  clearData(dataBlock);


  cout << "Last nrOfFiles: " << ourFS.nrOfFiles << endl;
}
