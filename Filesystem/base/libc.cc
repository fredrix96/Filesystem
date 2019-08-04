// (C) Dr Andrew Moss 2019
// See LICENSE for details.

#include "base/system.h"


LibC::LibC(Kernel &k) : k(k)
{
    k.syscall(SYS_DEFAULTCWD, (intptr_t)&CWD, 0, 0, 0);
}


void LibC::format()
{
    k.syscall(SYS_FORMAT, 0, 0, 0, 0);
}

int  LibC::open(const char *path, bool creat, bool append)
{
    uint32_t flags=0;
    if(creat)  flags |= 1;
    if(append) flags |= 2;
    intptr_t fd=-1;
    k.syscall(SYS_OPEN, (intptr_t)path, flags, CWD, (intptr_t)&fd);
    return (int)fd;
}

void LibC::dump()
{
    k.syscall(SYS_DUMP, 0, 0, 0, 0);
}

void LibC::close(int fd)
{
}

uint32_t LibC::read(int fd, uint8_t *buffer, uint32_t numBytes)
{
    intptr_t retVal=-1;
    k.syscall(SYS_READ, fd, (intptr_t)buffer, numBytes, (intptr_t)&retVal);
    return (uint32_t)retVal;
}

void LibC::write(int fd, uint8_t *buffer, uint32_t numBytes)
{
    k.syscall(SYS_WRITE, fd, (intptr_t)buffer, numBytes, 0);
}


int LibC::rm(const char *path)
{
intptr_t retVal=-1;
    k.syscall(SYS_DEL, (intptr_t)path, CWD, (intptr_t)&retVal, 0);
    return (int)retVal;
}


int LibC::mkdir(const char *path)
{
intptr_t retVal=-1;
    k.syscall(SYS_MKDIR, (intptr_t)path, CWD, (intptr_t)&retVal, 0);
    return (int)retVal;
}


int LibC::cd(const char *path)
{
intptr_t retVal=-1;
    // Note: two return values - the status and the new CWD.
    k.syscall(SYS_CD, (intptr_t)path, CWD, (intptr_t)&retVal, (intptr_t)&CWD);
    // Only overwrite the CWD if the call was a success otherwise propagate failure.
    if(retVal==-1)
        return -1;
    CWD = retVal;
    return 0;
}


int LibC::pwd(char *buffer, size_t size)
{
intptr_t retVal=-1;
    k.syscall(SYS_PWD, (intptr_t)buffer, CWD, (intptr_t)&retVal, (intptr_t)size);
    return (int)retVal;
}


int LibC::move(const char *source, const char *target)
{
intptr_t retVal=-1;
    k.syscall(SYS_MOVE, (intptr_t)source, CWD, (intptr_t)target, (intptr_t)&retVal);
    return (int)retVal;
}


