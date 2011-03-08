#include "easygen.h"

int main(int argc, char* argv[])
{
   try {
      testInfo super(argc, argv);
      super["y"] = (real)0.23;
   } catch (genError a) {
      cout << a.err();
   }
   return 0;
}