#define __my__release
//TODO:
//    arithmetic overflow check
//    exception handling
//    speedUp, FE, expressions cache

#include "easygen.h"
#include "stdlib.h"
#include "string.h"

using namespace std;

const int maxStringLen = 10240;
const char* randseedParamName = "randseed";
const char defaultSpaceChar = ' ';
const char* formalInputFName = "formal.input";

bool wasSpaceChar;

void genError::processParseError()
{
    if (wasError()) {
        parseError* tmp = getLastError();
        ostringstream ss;
        ss << string(errorMessageByCode(tmp->code))
            << ". Line: " << tmp->line << ", pos: " << tmp->pos;
        throw genError(ss.str());
    }
}

char* testInfo::loadFormatFile(const string& fileName)
{
    ifstream f(fileName.c_str());
    char tmp[maxStringLen];
    string res;
    while (1) {
        f.getline(tmp, maxStringLen);
        size_t len = strlen(tmp);
        tmp[len+1] = 0; tmp[len] = '\n';
        if (!f) break;
        res += tmp;
    }
    char* res1 = (char*)malloc(res.length() + 1);
    strcpy(res1, res.c_str());
    return res1;
}

testInfo::testInfo(int argc, char* argv[])
{
    string s;
    size_t i,j,k;
    for (i = 1; i < (size_t)argc; i++) {
        if (i > 1) s += " ";
        s += argv[i];
    }
    i = 0; k = s.length();
    while (i < k) {
        while (i < k && isspace(s[i])) ++i;
        if (i >= k) break;
        j = i;
        while (i < k && s[i] != '=' && !isspace(s[i])) ++i;
        if (i >= k) throw genError("illegal command line arguments");
        string val, name = s.substr(j, i-j);
        while (i < k && isspace(s[i]) && s[i] != '=') ++i;
        if (i >= k || s[i] != '=') throw genError("illegal command line arguments");
        ++i;
        while (i < k && isspace(s[i])) ++i;
        if (i >= k) throw genError("illegal command line arguments");
        j = i;
        if (s[i] == '\'') {
            ++i; bool screen = false, ok = false;
            val = "";
            while (i < k) {
                if (screen) {
                    if (s[i] == '\\' || s[i] == '\'') val += s[i];
                    else throw genError("illegal char after \\ in command line");
                } else {
                    if (s[i] == '\\') screen = true;
                    else if (s[i] == '\'') {ok = true; i++; break;}
                    else val += s[i];
                }
                i++;
            }
            if (!ok) throw genError("illegal string value in conmand line");
        } else {
            while (i < k && !isspace(s[i])) ++i;
            val = s.substr(j, i-j);
        }
        if (params.find(name) != params.end())
            throw genError("double param definition in command line");
        params[name] = val;
    }

    buf = loadFormatFile(formalInputFName);
    initialize(buf);
    objRecord* tmp = parseObjRecord();
    genError::processParseError();
    desc.a.pointerToData = 0; desc.a.recPart = tmp;
    desc.a = mallocRecord(desc.a, 1);
    wasSpaceChar = true;
    paramsToVars();
}

testInfo::~testInfo()
{
    print();
    destroyRecData(desc.a);
    objRecordDestructor(desc.a.recPart);
    finalize();
    free(buf);
}

INT64 testInfo::getIntParam(const string& name)
{
    string tmp = params[name];
    if (tmp.length() > getMaxIntLen()) throw genError("too long integer in command line");
    istringstream ss(tmp); INT64 res;
    ss >> res;
    if (ss.fail()) throw genError("trying to get int param from non-int param");
    return res;
}

real testInfo::getFloatParam(const string& name)
{
    string tmp = params[name];
    istringstream ss(tmp); real res;
    ss >> res;
    if (ss.fail()) throw genError("trying to get float param from non-float param");
    return res;
}

void testInfo::paramsToVars()
{
    for (map<string, string>::iterator i = params.begin(); i != params.end(); ++i) {
        if (i->first == randseedParamName) setRandSeed(getIntParam(i->first));
        else {
            objWithData tmp = findObject(i->first.c_str(), desc.a, 0);
            if (tmp.objPart) {
                switch (tmp.objPart->objKind) {
                    case oInteger:
                        desc.operator [](i->first) = getIntParam(i->first);
                        break;
                    case oFloat:
                        desc.operator [](i->first) = getFloatParam(i->first);
                        break;
                    case oString:
                        desc.operator [](i->first) = i->second;
                        break;
                }
                genError::processParseError();
            }
        }
    }
    genError::processParseError();
}

prxObject testInfo::operator [](const string& name)
{
    return desc.operator [] (name);
}

void testInfo::print()
{
    desc.print();
}

void testInfo::autoGen()
{
    desc.autoGen();
}

prxObject prxRecord::operator [](const string& name)
{
    objWithData tmp = byName(a, name.c_str());
    genError::processParseError();
    if (tmp.objPart) return prxObject(tmp);
    else throw genError("object with name '" + name + "' not found");
}

void prxRecord::print()
{
    if ((!a.pointerToData || !a.pointerToData->data) && a.recPart->n)
        throw genError("uninitialized data while printing");
    for (int i = 0; i < a.recPart->n; i++) {
        objWithData st;
        st.objPart = &(a.recPart->seq[i]);
        st.pointerToData = &(a.pointerToData->data[i]);
        prxObject tmp(st);
        tmp.print();
    }
}

void prxRecord::autoGen()
{
    autoGenRecord(a);
}

prxRecord prxObject::operator [] (int index)
{
    recWithData tmp = byIndex(a, index);
    genError::processParseError();
    return prxRecord(tmp);
}

INT64 prxObject::operator = (const INT64& value)
{
    if (a.objPart->objKind != oInteger)
        throw genError("trying to assign integer to non-int object");
    setIntValue(a, value);
    genError::processParseError();
    return value;
}

int prxObject::operator = (const int& value)
{
    return *this = (INT64)value;
}

real prxObject::operator = (const real& value)
{
    if (a.objPart->objKind != oFloat)
        throw genError("trying to assign float to non-float object");
    setFloatValue(a, value);
    genError::processParseError();
    return value;
}

string prxObject::operator = (const string& value)
{
    if (a.objPart->objKind != oString)
        throw genError("trying to assign string to non-string object");
    setStrValue(a, value.c_str());
    genError::processParseError();
    return value;
}

prxObject::operator INT64()
{
    if (a.objPart->objKind != oInteger)
        throw genError("trying to get integer from non-int object");
    INT64 res = getIntValue(a);
    genError::processParseError();
    return res;
}

prxObject::operator int()
{
    return (int)(INT64)(*this);
}

prxObject::operator real()
{
    if (a.objPart->objKind != oFloat)
        throw genError("trying to get float from non-float object");
    real res = getFloatValue(a);
    genError::processParseError();
    return res;
}

prxObject::operator string()
{
    if (a.objPart->objKind != oString)
        throw genError("trying to get string from non-string object");
    string res(getStrValue(a));
    genError::processParseError();
    return res;
}

void prxObject::print()
{
    if ((!a.pointerToData || !a.pointerToData->value) &&
        a.objPart->objKind != oNewLine && a.objPart->objKind != oSoftLine)
            throw genError("uninitialized data while printing");
    int objk = a.objPart->objKind;
    switch (objk) {
        case oNewLine: case oSoftLine:
            cout << endl; wasSpaceChar = true;
            break;
        case oInteger: case oFloat: case oString:
            if (!wasSpaceChar) cout << defaultSpaceChar;
            if (objk == oInteger) {
                cout << getIntValue(a);
            } else if (objk == oString) {
                cout << getStrValue(a);
            } else if (objk == oFloat) {
                cout << fixed << setprecision(getFloatDig(a)) << getFloatValue(a);
            }
            genError::processParseError();
            wasSpaceChar = 0;
            break;
        case oSeq:
            int n = getSeqLen(a);
            for (int i = 1; i <= n; i++) {
                prxRecord tmp(byIndex(a, i));
                genError::processParseError();
                tmp.print();
            }
            break;
    }
}

void prxObject::autoGen()
{
    switch (a.objPart->objKind) {
        case oNewLine: case oSoftLine: cout << endl; break;
        case oInteger: autoGenInt(a); break;
        case oFloat: autoGenFloat(a); break;
        case oString: autoGenStr(a); break;
        case oSeq: autoGenSeq(a); break;
    }
}


#ifndef __my__release
/*int main(int argc, char* argv[])
{
    freopen("output.txt","w",stdout);

    try {
        testInfo super(argc, argv);
        super.autoGen();
    } catch (genError a) {
        cout << a.err();
    }
    */
    /*freopen("format.txt","r",stdin);
    char s[10000]; cin.getline(s,10000);
    parseError* res = validate(s);
    cout << res->code;
    fclose(stdin);*/

/*    fclose(stdout);
    return 0;
} */


#endif
