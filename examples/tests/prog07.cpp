#include "easygen.h"

int main(int argc, char* argv[])
{
   try {
      testInfo super(argc, argv);
      super["a"][1]["x"] = (INT64)1;
      super["a"][2]["x"] = (INT64)2;
      super["a"][3]["x"] = (INT64)3;
   } catch (genError a) {
      cout << a.err();
   }
   return 0;
}