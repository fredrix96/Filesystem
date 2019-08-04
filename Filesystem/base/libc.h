// (C) Dr Andrew Moss 2019
// See LICENSE for details.

class LibC {
  Kernel &k;
  intptr_t  CWD;
public:
  LibC(Kernel &k);
  void format();
  int  open(const char *, bool, bool);
  void dump();
  void close(int);
  uint32_t read(int, uint8_t *, uint32_t);
  void write(int, uint8_t *, uint32_t);
  int rm(const char *);
  int mkdir(const char *);
  int cd(const char *);
  int pwd(char *, size_t);
  int move(const char *, const char *);
};


