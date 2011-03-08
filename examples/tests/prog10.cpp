#include "easygen.h"

int main(int argc, char* argv[])
{
   try {
      testInfo super(argc, argv);
   } catch (genError a) {
      cout << a.err();
   }
   return 0;
}