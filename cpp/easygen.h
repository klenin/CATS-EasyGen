#pragma once

#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iomanip>

using namespace std;

extern "C" {
   #include "parser.h"
}

class genError
{
   string err1;
public:
   genError(const string& s): err1(s) {}
   string err() {return err1;}
   static void processParseError();
};

class prxObject;

class prxRecord
{
   friend class testInfo;
   friend class prxObject;
   recWithData a;
   prxRecord() {a.recPart = 0; a.pointerToData = 0;}
   prxRecord(const recWithData& b) : a(b) {};
public:
   prxObject operator [] (const string& name);
   void print();
   void autoGen();
};
            
class prxObject
{
   friend class prxRecord;
   objWithData a;
   prxObject(const objWithData& b) : a(b) {};
public:
   prxRecord operator [] (int index);
   INT64 operator = (const INT64& value);
   int operator = (const int& value);
   string operator = (const string& value);
   real operator = (const real& value);
   operator INT64();
   operator int();
   operator real();
   operator string();
   void print();
   void autoGen();
};

class testInfo
{
   map<string, string> params;
   prxRecord desc;
   char* buf;
public:
   testInfo(int argc, char* argv[]);
   ~testInfo();
   char* loadFormatFile(const string& fileName);
   void paramsToVars();
   INT64 getIntParam(const string& name);
   real getFloatParam(const string& name);
   prxObject operator [](const string& name);
   void print();
   void autoGen();
};

