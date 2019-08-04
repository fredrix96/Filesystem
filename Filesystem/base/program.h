// (C) Dr Andrew Moss 2019
// See LICENSE for details.

class Program {
public:
  LibC &libc;
  intptr_t CWD;
  virtual void execute()=0;
  Program(LibC &l) : libc(l) {}
};

