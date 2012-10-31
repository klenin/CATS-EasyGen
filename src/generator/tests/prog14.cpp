#include "easygen.h"

int main(int argc, char* argv[])
{
   try {
      testInfo super(argc, argv);
      super["N"].autoGen();
      for (INT64 i = 1; i <= (INT64)super["N"]; i++)
         super["A"][i]["num"] = (INT64)i*i;
   } catch (genError a) {
      cout << a.err();
   }
   return 0;
}