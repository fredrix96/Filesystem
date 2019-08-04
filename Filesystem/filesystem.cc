// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include "filesystem.h"

using namespace std;

FileSystem::FileSystem(Disk &d) : disk(d)
{
  // check to see if number of files gives random stuff
  // if so format has yet to run and the disk contains garbage
  // ergo don't try to load stuff from the disk
  clearData(dataBlock.data);
  disk.read(ROOT_OFFSET, dataBlock.data);

  int tmp = NAME_SIZE;
  ourFS.nrOfFiles = 0x0000;
  ourFS.nrOfFiles = dataBlock.data[tmp];
  tmp += 1;
  ourFS.nrOfFiles = ourFS.nrOfFiles << 8;
  ourFS.nrOfFiles |= dataBlock.data[tmp];
  tmp += 1;
  clearData(dataBlock.data);

  if(ourFS.nrOfFiles < NUMBER_OF_BLOCKS)
  {
    // Reading all the neccessary information for FAT such at filenames, where they start and how big they are.
    PFStoVFS();
  }

}


intptr_t FileSystem::defaultCWD()
{
    return (intptr_t)ourFS.dirName;          // Pick a representation of directories that can cast into the return type.
}


void FileSystem::format()
{
    //clear all blocks
    for(uint64_t i = 0; i < NUMBER_OF_BLOCKS; i++)
    {
      dataBlock.data[BLOCK_SIZE] = { 0 };
      disk.write(i, dataBlock.data);
    }

    //std::cout << "format: not implemented" << std::endl;

  //  super.magicNumber = MAGIC_NUMBER;
  //  super.blocks = NUMBER_OF_BLOCKS;
  //  clearData(super.data);
  //  super.data[0] = super.magicNumber;
  //  uint8_t tmp_i = 0;
  //  for(uint8_t i = 1; i < super.blocks; i++)
  //  {
  //    super.data[i] = 255;
  //    super.blocks -= 255;
  //    tmp_i = i;
  //  }
  //  super.data[tmp_i + 1] = 8;
  //  super.blocks = NUMBER_OF_BLOCKS;
  //  disk.write(0, super.data);

    //create root dir
    ourFS.dirName = "/";
    clearData(dataBlock.data);
    for(int i = 0; i < NAME_SIZE; i++)
    {
      dataBlock.data[i] = ourFS.dirName[i];
    }

    // nrOfFiles
    ourFS.nrOfFiles = 0;

    int tmp = NAME_SIZE;
    dataBlock.data[tmp] = ((ourFS.nrOfFiles & 0xFF00) >> 8);
    tmp += 1;
    dataBlock.data[tmp] = (ourFS.nrOfFiles & 0x00FF);
    tmp += 1;
    disk.write(ROOT_OFFSET, dataBlock.data);
    clearData(dataBlock.data);

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





}


void FileSystem::dump() {
    //std::cout << "dump: not implemented" << std::endl;

    //read SuperBlock
    // disk.read(0, dataBlock.data);
    // cout << "------------SuperBlock-------------" << endl;
    // cout << "MagicNumber: " << (int)dataBlock.data[0] << endl;
    // uint32_t totalBlocks = 0;
    // uint32_t nrOfBlocks = NUMBER_OF_BLOCKS;
    // for(uint8_t i = 1; i < nrOfBlocks; i++)
    // {
    //   totalBlocks += (int)dataBlock.data[i];
    //   nrOfBlocks -= 255;
    // }
    // totalBlocks += 8; // Cough cough
    // cout << "Total blocks: " << totalBlocks << endl;
    // cout << "-----------------------------------" << endl;



// printing root
    cout << "Dir " << ourFS.dirName;
    cout << " with " << ourFS.nrOfFiles + ourFS.nrOfDirs << " entries" << endl;

/*
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
*/
// printing files
    for(int i = 0; i < ourFS.nrOfFiles; i++)
    {
      cout << "File " << ourFS.dirName << ourFS.files.at(i).fileName;
      cout << " with " << (ourFS.files.at(i).length * BLOCK_SIZE) + ourFS.files.at(i).size << " bytes" << endl;
    }

// printing directories
    for(int i = 0; i < ourFS.nrOfDirs; i++)
    {
      cout << "Dir " << ourFS.dirName << ourFS.dirs.at(i).dirName << "/";
      cout << " with " << (int)ourFS.dirs.at(i).files.size() + (int)ourFS.dirs.at(i).dirs.size() << " entries" << endl;
    }

}


int FileSystem::open(intptr_t CWD, const char *path, uint32_t flags)
{
    int found = searchFile(path);
    if(found > -1)
    {
      // cout << "Opening file " << path << endl;

    }
    else if(flags)
    {
      found = searchDir(path);
      if(found == -1)
      {
        found = createFile(path);
        if(found == -1)
        {
        //  cout << "Filesystem full. File not created!" << endl;
        }
      }

    }

    return found;
}


void FileSystem::write(uint32_t fd, uint8_t *buffer, uint32_t size)
{
  //std::cout << "write(" << fd << ", " << (intptr_t)buffer << ", " << size << "): not implemented" << std::endl;

  int counter = 0;
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
      // check to find the position of the file we are writing to in our files vector
      // this is so that we can update the files length.
      for(uint32_t j = 0; j < ourFS.files.size(); j++)
      {
        if(ourFS.files.at(j).start == fd)
        {
          filePos = j;
        }
      }
      ourFS.files.at(filePos).atBlock = fd;
      fatPos.push_back(fd);
      clearData(dataBlock.data);

      //write to file
      int extra = 0;
      for(uint32_t j = 0; j < size; j++)
      {
        dataBlock.data[counter] = buffer[j];
        if(ourFS.files.at(filePos).size < 4096)
        {
          ourFS.files.at(filePos).size += 1;
        }
        else
        {
          extra++;
        }

        if( counter == BLOCK_SIZE - 1)
        {
          if(extra == 0)
          {
            ourFS.files.at(filePos).size = extra;
          }
          else
          {
            ourFS.files.at(filePos).size =  extra;
            extra = 0;
          }
          counter = 0;
          // if our file has a next block we want to write to that one
          // so as not to have random FAT table positions pointing to random places
          // 1 in our FAT table means it's the last block in the chain
          if(ourFS.fat[currentBlock] == 1)
          {
            next = searchEmpty();
            if(next != -1)
            {
              fatPos.push_back(next);
              ourFS.fat[currentBlock] = next;
              currentBlock = next;
              ourFS.fat[next] = 1;
              ourFS.files.at(filePos).length += 1;

              dataBuff.push_back(dataBlock);
              clearData(dataBlock.data);
            }
            else
            {
            //  cout << "Filesystem full! Data could NOT be written!\n Aborting write... " << endl;
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
        dataBuff.push_back(dataBlock);
        clearData(dataBlock.data);
      }

      if(next != -1)
      {
        for(size_t i = 0; i < dataBuff.size(); i++)
        {
          clearData(dataBlock.data);
          dataBlock = dataBuff.at(i);
          disk.write(fatPos.at(i), dataBlock.data);
        }
      }
      dataBuff.clear();
      break;
    }
  }
  saveFiles();
}

uint32_t FileSystem::read(int fd, uint8_t *buffer, uint32_t numBytes)
{
    //std::cout << "read(" << fd << ", " << (intptr_t)buffer << ", " << numBytes << "): not implemented" << std::endl;
    int filePos = 0;
    //int currentBlock = fd;
    int bufferCounter = 0;
    bool limit = true;
    //std::cout << "read fd, numBytes " << fd << "  " << numBytes << std::endl;
    uint32_t bytesRead = 0;

    // Clearing the incomming buffer
  //  for(uint32_t i = 0; i < numBytes; i++)
  //  {
  //    buffer[i] = 0;
  //  }

    for(uint32_t j = 0; j < ourFS.files.size(); j++)
    {
      if(ourFS.files.at(j).start == fd)
      {
        filePos = j;
      }
    }

    //cout << "3 read filesize = " << ourFS.files.at(filePos).size << endl;

     //int currentPos = 0;
     //currentPos = ourFS.files.at(filePos).pos;
    // ourFS.files.at(filePos).pos = currentPos + numBytes;


    while(limit == true)
    {
      clearData(dataBlock.data);
      //std::cout << "currentBlock = " << currentBlock << std::endl;
      disk.read(ourFS.files.at(filePos).atBlock, dataBlock.data);

      for(uint32_t i = 0; i < BLOCK_SIZE; i++)
      {
        // std::cout << "ourFS.files.at(filePos).size = " << ourFS.files.at(filePos).size << std::endl;
        /*if(ourFS.fat[ourFS.files.at(filePos).atBlock] == 1 && (( ourFS.files.at(filePos).pos) <= 300 || ( ourFS.files.at(filePos).pos) >= 3300))
        {
          //std::cout << "( ourFS.files.at(filePos).pos) = " << (ourFS.files.at(filePos).pos) << std::endl;
          //std::cout << "ourFS.fat[currentBlock] = " << ourFS.fat[ourFS.files.at(filePos).atBlock] << std::endl;
        }*/

        if(ourFS.fat[ourFS.files.at(filePos).atBlock] == 1 && ourFS.files.at(filePos).size <= ( ourFS.files.at(filePos).pos))
        {
          //std::cout << "ourFS.files.at(filePos).atBlock = " << ourFS.files.at(filePos).atBlock << " ourFS.fat[ourFS.files.at(filePos).atBlock] = " << ourFS.fat[ourFS.files.at(filePos).atBlock] << std::endl;
          //cout << "(ourFS.files.at(filePos).pos) = " << (ourFS.files.at(filePos).pos) << endl;
          //cout << "size = " << ourFS.files.at(filePos).size << endl;
          limit = false;
          break;
        }
        else
        {
          buffer[i + bufferCounter] = (int)dataBlock.data[ourFS.files.at(filePos).pos];
          //std::cout << "ourFS.files.at(filePos).pos = " << ourFS.files.at(filePos).pos << std::endl;
          //std::cout << "i + bufferCounter = " << i + bufferCounter << std::endl;
          bytesRead++;
          ourFS.files.at(filePos).pos += 1;
          if(bytesRead >= numBytes)
          {
            //cout << "bytesRead " << bytesRead << std::endl;
            //cout << "numBytes " << numBytes << std::endl;
            break;
          }

          // go to next block
          if(i >= (BLOCK_SIZE - 1) || ourFS.files.at(filePos).pos >= (BLOCK_SIZE))
          {
            //std::cout << " i == BLOCK_SIZE -1 " << std::endl;
            ourFS.files.at(filePos).atBlock = ourFS.fat[ourFS.files.at(filePos).atBlock];
            ourFS.files.at(filePos).pos = 0;
            bufferCounter = i + 1;
            break;
          }
        }
      }
      if(bytesRead >= numBytes)
      {
        limit = false;
      }

      // cout << "HIS READ BUFFER ";
      // for(uint32_t i = 0; i < numBytes; i++)
      // {
      //   cout << (int)buffer[i];
      // }
      // cout << endl;

    }

    // cout << "OUR READ BUFFER ";
    // for(uint32_t i = 0; i < numBytes; i++)
    // {
    //   cout << (int)dataBlock.data[i + ourFS.files.at(filePos).atBlock];
    // }
    // cout << endl;



    saveFiles();

    return bytesRead;
}


int FileSystem::del(const char *path, intptr_t CWD)
{
    int result = -1;
    int start = -1;

    result = searchFile(path);
    start = result;

    if(result != -1)
    {
      while (ourFS.fat[result] != 1)
      {
        int temp = result;
        result = ourFS.fat[result];
        ourFS.fat[temp] = 0;

        // cleaning the data in the block
        clearData(dataBlock.data);
        disk.write(temp, dataBlock.data);


      }
      if(ourFS.fat[result] == 1)
      {
        ourFS.fat[result] = 0;
      //  std::cout << "File with name: " << path << " now removed from FAT table!" << std::endl;
      }
    }

    for(uint32_t i = 0; i < ourFS.files.size(); i++)
    {
      if(ourFS.files.at(i).start == start)
      {
        result = 0;
        ourFS.files.erase(ourFS.files.begin() + i);
        std::vector<File> temp;
        for (size_t j = 0; j < ourFS.files.size(); j++)
        {
          if (j != i)
          {
            temp.push_back(ourFS.files.at(j));

          }
        }
        ourFS.files = temp;
        ourFS.nrOfFiles -= 1;
      //  std::cout << "File with name: " << path << " now removed from our files vector!" << std::endl;
        break;
      }
    }

    saveFiles();
    return result;
}


int FileSystem::mkdir(intptr_t CWD, const char *path)
{
    //std::cout << "mkdir(" << CWD << ", " << path << "): not implemented" << std::endl;
    int result = -1;
//std::cout << "path: " << path << std::endl;
    int len = strlen(path);
    char *str = new char[len];
    for(int i = 0; i < len; i++)
    {
      str[i] = path[i];
    }
//std::cout << "mkdir after" << std::endl;
//    char limit[] = "/";
//    char *ptr = strtok(str, limit);
//    vector<char*> names;
//    int limitCounter = 0;
//
//    while(ptr != NULL)
//    {
//      names.push_back(ptr);
//      std::cout << "ptr != NULL names = " << names.back() << std::endl;
//      ptr = strtok(NULL, limit);
//    }


    int pos = 0;
    pos = searchDir(str);
  //  std::cout << "pos = " << pos << std::endl;


    if( pos == -1)
    {
      Directory newDir;
      newDir.dirName = str;
    //  std::cout << "Creating new directory with name = " << newDir.dirName << std::endl;
      ourFS.dirs.push_back(newDir);
      ourFS.nrOfDirs++;
      result = 0;
      saveFiles();
    }

    return result;
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
  for(uint32_t i = 0; i < NUMBER_OF_BLOCKS; i++)
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
    //  cout << "File with name " << name << " has been found!" << endl;
      break;
    }
  }


  return found;
}

int FileSystem::searchDir(const char *name)
{
  int found = -1;

  //cout << "Size of dir array = " << ourFS.dirs.size() << endl;
  for(uint32_t i = 0; i < ourFS.dirs.size() ; i++)
  {
  //  cout << "name = " << name << endl;
  //  cout << "Searching for dir with name: " << name << " in directory with name: " << ourFS.dirName << " in dirs at pos: " << i << endl;
  //  std::cout << "ourFS.dirs.at(i).dirName = " << ourFS.dirs.at(i).dirName << std::endl;
    if( strcmp(ourFS.dirs.at(i).dirName, name) == 0)
    {
      found = i;
    //  cout << "Dir with name " << name << " has been found in dir " << ourFS.dirName << "! " << endl;
      break;
    }
  }
  if(found == -1)
  {
    found = searchFile(name);
  }

  return found;
}

void FileSystem::dirStep(Directory dir, char *parentName, uint8_t depth, int pos)
{
  // write it self to disk
  // parentName is NAME_SIZE + 1 to account for "/" infront of it
  for(int i = 0; i < (NAME_SIZE * 2) + 2; i++)
  {
    if(i <= NAME_SIZE)
    {
      dataBlock.data[pos] = parentName[i];
    }
  //  else if(i == NAME_SIZE + 1)
  //  {
  //    dataBlock.data[pos] = '/';
  //  }
    else if(i == NAME_SIZE + 1)
    {
      dataBlock.data[pos] = depth;
    }
    else
    {
      dataBlock.data[pos] = dir.dirName[i];
    }
    pos++;
  }

// write it's children(and their children) to disk
  int children = dir.dirs.size();
  char dirName[NAME_SIZE];
  for (size_t i = 1; i < NAME_SIZE + 1; i++)
  {
    dirName[i] = dir.dirName[i];
  }
  while (children != 0)
  {
    dirStep(dir.dirs.at(children), dirName, depth + 1, pos);
    children--;
  }

}


int FileSystem::searchEmpty()
{
  int found = -1;
  //  searching for the first empty slot in our fat table
  // returns -1 if our filesystem is full
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
    //   cout << "Creating file with name " << temp.fileName << endl;
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
  clearData(dataBlock.data);
  int counter = 0;
  for (int i = FAT_OFFSET; i < NUMBER_OF_BLOCKS; i++)
  {
    dataBlock.data[counter] = ((ourFS.fat[i] & 0xFF00 ) >> 8);
    counter += 1;
    dataBlock.data[counter] = (ourFS.fat[i] & 0x00FF );
    counter += 1;
  }
  disk.write(FAT_OFFSET-1, dataBlock.data);

// saving root directory, nrOfFiles and nrOfDirs.
  clearData(dataBlock.data);
  for(int i = 0; i < NAME_SIZE; i++)
  {
    dataBlock.data[i] = ourFS.dirName[i];
  }
  //cout << "Saving nrOfFiles: " << ourFS.nrOfFiles << endl;
  int tmp = NAME_SIZE;

  dataBlock.data[tmp] = ((ourFS.nrOfFiles & 0xFF00) >> 8);
  tmp += 1;
  dataBlock.data[tmp] = (ourFS.nrOfFiles & 0x00FF);
  tmp += 1;
  dataBlock.data[tmp] = ((ourFS.nrOfDirs & 0xFF00) >> 8);
  tmp += 1;
  dataBlock.data[tmp] = (ourFS.nrOfDirs & 0x00FF);
  tmp += 1;
  disk.write(ROOT_OFFSET, dataBlock.data);
  //cout << "Saved, now nrOfFiles = " << ourFS.nrOfFiles << endl;


  // writing all dirs to Disk
  clearData(dataBlock.data);

  // write roots'(ourFS) children(and their children) to disk
//  int children = ourFS.dirs.size();
//  char dirName[NAME_SIZE] = { 'r', 'o', 'o', 't' };
//  while (children != 0)
//  {
//    dirStep(ourFS.dirs.at(children), dirName, 0, 0);
//    children--;
//  }
  counter = 0;
  for (size_t i = 0; i < ourFS.dirs.size(); i++)
  {
  //  std::cout << "before in saving dir" << std::endl;
    for (size_t j = 0; j < NAME_SIZE; j++)
    {
      //std::cout << "while in saving dir" << std::endl;
      dataBlock.data[counter] = ourFS.dirs.at(i).dirName[j];
      counter += 1;
    }
  }
  //std::cout << "after in saving dir" << std::endl;
  disk.write(DIR_OFFSET, dataBlock.data);
  clearData(dataBlock.data);

  // Writing all the file struct to Disk
  int i = 0;
  int k = 0;
  int j = FILE_OFFSET;
  int lengthOfFile = (NAME_SIZE + 10);
  File temp;
  clearData(dataBlock.data);
  while(k < ourFS.nrOfFiles)
  {
    // check that we haven't filled our buffer
    if( k * lengthOfFile >= BLOCK_SIZE - lengthOfFile)
    {
      i = 0;
      disk.write(j, dataBlock.data);
      clearData(dataBlock.data);
      j++;
    }
    int m = 0;
    for(int l = i * lengthOfFile; l < (i * lengthOfFile) + lengthOfFile; l++)
    {
      if( m < 30)
      {
        dataBlock.data[l] = ourFS.files.at(k).fileName[m];
      }
      else if( m < 31)
      {
        dataBlock.data[l] = 0x0000;
        dataBlock.data[l] = ((ourFS.files.at(k).start & 0xFF00) >> 8);
      }
      else if( m < 32)
      {
        dataBlock.data[l] = ((ourFS.files.at(k).start & 0x00FF));
      }
      else if( m < 33)
      {
        dataBlock.data[l] = 0x0000;
        dataBlock.data[l] = ((ourFS.files.at(k).length & 0xFF00) >> 8);
      }
      else if(m < 34)
      {
        dataBlock.data[l] = ((ourFS.files.at(k).length & 0x00FF));
      }
      else if( m < 35)
      {
        dataBlock.data[l] = 0x0000;
        dataBlock.data[l] = ((ourFS.files.at(k).pos & 0xFF00) >> 8);
      }
      else if( m < 36)
      {
        dataBlock.data[l] = ((ourFS.files.at(k).pos & 0x00FF));
      }
      else if( m < 37)
      {
        dataBlock.data[l] = 0x0000;
        dataBlock.data[l] = ((ourFS.files.at(k).size & 0xFF00) >> 8);
      }
      else if( m < 38)
      {
        dataBlock.data[l] = ((ourFS.files.at(k).size & 0x00FF));
      }
      else if( m < 39)
      {
        dataBlock.data[l] = 0x0000;
        dataBlock.data[l] = ((ourFS.files.at(k).atBlock & 0xFF00) >> 8);
      }
      else
      {
        dataBlock.data[l] = ((ourFS.files.at(k).atBlock & 0x00FF));
      }
      m++;
    }

    i++;
    k++;
  }
  disk.write(j, dataBlock.data);
}


void FileSystem::PFStoVFS()
{
  // Filling the virtual fs with disk data
  // reading in the FAT table
  clearData(dataBlock.data);
  disk.read(FAT_OFFSET-1, dataBlock.data);
  int counter = 0;
  for (int i = FAT_OFFSET; i < NUMBER_OF_BLOCKS; i++)
  {
    // the 0x0000 isn't really needed but good for clarity
    ourFS.fat[i] = 0x0000;
    ourFS.fat[i] = dataBlock.data[counter];
    counter += 1;
    ourFS.fat[i] = ourFS.fat[i] << 8;
    ourFS.fat[i] |= dataBlock.data[counter];
    counter += 1;
  }
  clearData(dataBlock.data);
  //cout << "Before reading in nrOfFiles are: " << ourFS.nrOfFiles << endl;

  disk.read(ROOT_OFFSET, dataBlock.data);
  // root directory
  char *rootName = new char[NAME_SIZE];
  for(int i = 0; i < NAME_SIZE; i++)
  {
    // the check should work since if dirName has a zero in it it should be ascii 60
    if(dataBlock.data[i] != 0)
    {
      rootName[i] = dataBlock.data[i];
    }
  }
  ourFS.dirName = rootName;

  // Checking how many files we have (comes directly after the name of the directory)
  int tmp = NAME_SIZE;
  ourFS.nrOfFiles = 0x0000;
  ourFS.nrOfFiles = dataBlock.data[tmp];
  tmp += 1;
  ourFS.nrOfFiles = ourFS.nrOfFiles << 8;
  ourFS.nrOfFiles |= dataBlock.data[tmp];
  tmp += 1;
  ourFS.nrOfDirs = 0x0000;
  ourFS.nrOfDirs = dataBlock.data[tmp];
  tmp += 1;
  ourFS.nrOfDirs = ourFS.nrOfDirs << 8;
  ourFS.nrOfDirs |= dataBlock.data[tmp];
  tmp += 1;
  // cout << "After read nrOfFiles: " << ourFS.nrOfFiles << endl;


  clearData(dataBlock.data);


/*
  // reading directories
  disk.read(DIR_OFFSET, dataBlock.data);
  //  our idea for a more robust system with directory trees
  struct Name
  {
    char parent[NAME_SIZE];
    uint8_t depth = -1;
    char me[NAME_SIZE];
  };

  vector<Name> names;
  int pos = 0;
  counter = 0;
  int dirNameSize = (NAME_SIZE * 2) + 2;
  while(counter < ourFS.nrOfDirs)
  {
    Name name;
    for(int j = 0; j < dirNameSize; j++)
    {
      if(j <= NAME_SIZE)
      {
        name.parent[j] = dataBlock.data[pos];
      }
    //  else if(j == NAME_SIZE + 1)
    //  {
    //    // do nothing for "/"
    //  }
      else if(j == NAME_SIZE + 1)
      {
        name.depth = dataBlock.data[pos];
      }
      else
      {
        name.me[j] = dataBlock.data[pos];
      }
      pos++;
    }

    names.push_back(name);
    counter++;
  }

  // if our depth == 0 we are a child of root -> do da thing!
  // else 1. save our name to a temp Directory.
  //      2. search for the next dir with our parents name in vector "names"
  //      3. push_back this to a list
  //      4. repeat 2-3 until we get the path from root to me
  //      5. loop through the path from root.dirs to parent.dirs and push_back me
  for(size_t i = 0; i < names.size(); i++)
  {
    char *dirName = new char[NAME_SIZE];
    // if my depth is == 0, I am a child to root directory
    if( names.at(i).depth == 0)
    {
      Directory tmp;
      for (size_t j = 0; j < NAME_SIZE; j++)
      {
        dirName[j] = names.at(i).me[j];
      }
      tmp.dirName = dirName;
      ourFS.dirs.push_back(tmp);
    }
    else
    {
      //  1.
      Directory tmp;
      for (size_t j = 0; j < NAME_SIZE; j++)
      {
        dirName[j] = names.at(i).me[j];
      }
      tmp.dirName = dirName;
      vector<char*> path;


      //  in the names vector look for our parent (me is at i and parent is at j)
      //  if found push_back the parentName to path
      //  then change our pos in names to parent pos (i = j)
      //  4.
      int k = i;
      while(names.at(k).depth != 0 )
      {
        //  2.
        // j is one less than i since we don't have to check ourselves
        for (size_t j = k - 1; j >= 0; j--)
        {
          // i is me and j cloud be my parent, check that parentName matches name at j
          //  also check that the depth of my parent is 1 less then my depth or else it's not my parent
          if(names.at(k).parent == names.at(j).me && names.at(k).depth - 1 == names.at(j).depth)
          {
            //  3.
            path.push_back(names.at(j).me);
            k = j;
            break;
          }
        }
      }

      //  5.
      Directory currentDir;
      currentDir = ourFS;
      while (path.size() > 0)
      {
        for(size_t j = 0; j < currentDir.dirs.size(); j++)
        {
          if(currentDir.dirs[j].dirName == path.back())
          {
            //add to dirs
            currentDir = currentDir.dirs[j];
            break;

          }
        }
        path.pop_back();
      }
      currentDir.dirs.push_back(tmp);
    }
    dirName = new char[NAME_SIZE];
  }
*/
  // reading directories
  disk.read(DIR_OFFSET, dataBlock.data);

  for (size_t i = 0; i < ourFS.nrOfDirs; i++)
  {
    char *dirName = new char[NAME_SIZE];
    for(int j = 0; j < NAME_SIZE; j++)
    {// the check should work since if dirName has a zero in it it should be ascii 60
      if(dataBlock.data[j] != 0)
      {
        dirName[i] = dataBlock.data[j];
      }
    }
    Directory newDir;
    newDir.dirName = dirName;
    ourFS.dirs.push_back(newDir);
  }
  clearData(dataBlock.data);

  // Reading in all the file structs to VFS
  int i = 0;
  int k = 0;
  int j = FILE_OFFSET;
  int lengthOfFile = (NAME_SIZE + 10);
  File temp;
  char *fileName = new char[NAME_SIZE];
  disk.read(j, dataBlock.data);
  while(k < ourFS.nrOfFiles)
  {
    if(k * lengthOfFile >= BLOCK_SIZE - lengthOfFile)
    {
      j++;
      i = 0;
      clearData(dataBlock.data);
      disk.read(j, dataBlock.data);
    }

    int m = 0;
    for(int l = i * lengthOfFile; l < i * lengthOfFile + lengthOfFile; l++)
    {
      if( m < 30)
      {
        fileName[m] = dataBlock.data[l];
      }
      else if( m < 31)
      {
        temp.start = 0x0000;
        temp.start = dataBlock.data[l];
      }
      else if( m < 32)
      {
        temp.start = temp.start << 8;
        temp.start |= dataBlock.data[l];
      }
      else if( m < 33)
      {
        temp.length = 0x0000;
        temp.length = dataBlock.data[l];
      }
      else if(m < 34)
      {
        temp.length = temp.length << 8;
        temp.length |= dataBlock.data[l];
      }
      else if( m < 35)
      {
        temp.pos = 0x0000;
        temp.pos = dataBlock.data[l];
      }
      else if( m < 36)
      {
        temp.pos = temp.pos << 8;
        temp.pos |= dataBlock.data[l];
      }
      else if( m < 37)
      {
        temp.size = 0x0000;
        temp.size = dataBlock.data[l];
      }
      else if( m < 38)
      {
        temp.size = temp.size << 8;
        temp.size |= dataBlock.data[l];
      }
      else if( m < 39)
      {
        temp.atBlock = 0x0000;
        temp.atBlock = dataBlock.data[l];
      }
      else
      {
        temp.atBlock = temp.atBlock << 8;
        temp.atBlock |= dataBlock.data[l];
      }
      m++;
    }
    temp.fileName = fileName;
    fileName = new char[NAME_SIZE];
    ourFS.files.push_back(temp);

    i++;
    k++;
  }
  clearData(dataBlock.data);


//   cout << "Last nrOfFiles: " << ourFS.nrOfFiles << endl;
}
