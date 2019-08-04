// (C) Dr Andrew Moss 2019
// See LICENSE for details.

class Kernel {
    Disk       disk;
    FileSystem fs;
    public:
    Kernel() : fs(disk) {}
    // TUTORIAL: Put something in about simulating return-values.
    //           Might need an exercise on pointers if they have no idea.
    void syscall(int num, intptr_t arga, intptr_t argb, intptr_t argc, intptr_t argd)
    {
        intptr_t *retValue;
        switch(num) {
            case SYS_FORMAT:      
                fs.format();  
                return;
            case SYS_DUMP:        
                fs.dump();    
                return;
            case SYS_DEFAULTCWD:  
                retValue = (intptr_t*)arga;
                *retValue = fs.defaultCWD();
                return;
            case SYS_OPEN:        
                retValue = (intptr_t*)argd;
                *retValue = fs.open(argc, (const char*)arga, (uint32_t)argb);
                return;
            case SYS_WRITE:
                fs.write(arga, (uint8_t*)argb, (uint32_t)argc);  
                return;
            case SYS_READ:        
                retValue = (intptr_t*)argd;
                *retValue = fs.read((int)arga, (uint8_t*)argb, (uint32_t)argc);
                return;
            case SYS_DEL:
                retValue = (intptr_t*)argc;
                *retValue = fs.del((const char *)arga, argb);
                return;
            case SYS_MKDIR:
                retValue = (intptr_t*)argc;
                *retValue = fs.mkdir(argb, (const char *)arga);
                return;
            case SYS_CD:
                retValue = (intptr_t*)argc;
                *retValue = fs.cd(argb, (const char *)arga);
                return;
            case SYS_PWD:
                retValue = (intptr_t*)argc;
                *retValue = fs.pwd(argb, (char *)arga, (size_t)argd);
                return;
            case SYS_MOVE:
                retValue = (intptr_t*)argd;
                *retValue = fs.move(argb, (const char *)arga, (const char *)argc);
                return;
            default:
                std::cout << "Unrecognised syscall: " << num << std::endl;
                return;
      }
    }
};

