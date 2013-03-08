#ifndef __EASYGEN_H__
#define __EASYGEN_H__

#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <sstream>
// Header <cstdint> requires C++0x, so use it's old name.
#include <stdint.h>


using namespace std;

extern "C" {
    #include "Parser.h"
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
    ParserObjectRecordWithDataT a;
    prxRecord() : a() { a.recPart = NULL; a.pointerToData = NULL; }
    prxRecord(const ParserObjectRecordWithDataT& b) : a(b) {}
public:
    prxObject operator [] (const string& name);
    void print();
    void autoGen();
};

class prxObject
{
    friend class prxRecord;
    ParserObjectWithDataT a;
    prxObject(const ParserObjectWithDataT& b) : a(b) {}
public:
    prxRecord operator [] (int index);
    prxObject& operator = (const int64_t& value);
    prxObject& operator = (const int& value);
    prxObject& operator = (const string& value);
    prxObject& operator = (const long double& value);
    operator int64_t();
    operator int();
    operator long double();
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
    int64_t getIntParam(const string& name);
    long double getFloatParam(const string& name);
    prxObject operator [](const string& name);
    void print();
    void autoGen();
};

#endif // __EASYGEN_H__
