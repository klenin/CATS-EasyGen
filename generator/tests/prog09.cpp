#include "easygen.h"

int main(int argc, char* argv[])
{
   try {
      testInfo super(argc, argv);
      super["a"][1]["x"] = (INT64)11;
      super.autoGen();
   } catch (genError a) {
      cout << a.err();
   }
   return 0;
}