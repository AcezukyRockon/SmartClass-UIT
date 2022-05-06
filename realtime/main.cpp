#include <iostream>
#include <unistd.h>
using namespace std;

int main () {
  unsigned int microsecond = 1000000;
  for(int i = 1; i<=100; i++){cout << i << endl;usleep(microsecond);}
  return 0;
}
