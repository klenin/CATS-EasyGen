#define __my__release
//TODO:
//    arithmetic overflow check
//    exception handling
//    speedUp, FE, expressions cache

#include <cctype>
#include <cstdlib>
#include <cstring>

extern "C"
{
    #include "Numerics.h"
    #include "ParserObjectAttributes.h"
    #include "Random.h"
}

#include "easygen.h"

using namespace std;

const int maxStringLen = 10240;
const char* randseedParamName = "randseed";
const char defaultSpaceChar = ' ';
const char* formalInputFName = "formal.input";

bool wasSpaceChar;

void genError::processParseError()
{
    if (ParserIsErrorRaised()) {
        ParserErrorT* tmp = ParserGetLastError();
        ostringstream ss;
        ss << string(ParserGetErrorMessageByCode(tmp->code))
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
    char* res1 = static_cast<char*>(malloc(res.length() + 1));
    strcpy(res1, res.c_str());
    return res1;
}

static int isspace_s(char c)
{
    return isspace(static_cast<unsigned char>(c));
}

testInfo::testInfo(int argc, char* argv[])
{
    string s;
    size_t i, j, k;
    for (i = 1; i < static_cast<size_t>(argc); i++) {
        if (i > 1) s += " ";
        s += argv[i];
    }
    i = 0; k = s.length();
    while (i < k) {
        while (i < k && isspace_s(s[i])) ++i;
        if (i >= k) break;
        j = i;
        while (i < k && s[i] != '=' && !isspace_s(s[i])) ++i;
        if (i >= k) throw genError("illegal command line arguments");
        string val, name = s.substr(j, i-j);
        while (i < k && isspace_s(s[i]) && s[i] != '=') ++i;
        if (i >= k || s[i] != '=') throw genError("illegal command line arguments");
        ++i;
        while (i < k && isspace_s(s[i])) ++i;
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
            while (i < k && !isspace_s(s[i])) ++i;
            val = s.substr(j, i-j);
        }
        if (params.find(name) != params.end())
            throw genError("double param definition in command line");
        params[name] = val;
    }

    buf = loadFormatFile(formalInputFName);
    initialize(buf);
    ParserObjectRecordT* tmp = parseObjRecord();
    genError::processParseError();
    desc.a.pointerToData = 0; desc.a.recPart = tmp;
    desc.a = *ParserAllocateObjectRecordWithData(&desc.a, 1);
    wasSpaceChar = true;
    paramsToVars();
}

testInfo::~testInfo()
{
    print();
    ParserDestroyObjectRecordWithData(&desc.a);
    ParserDestroyObjectRecord(desc.a.recPart);
    finalize();
    free(buf);
}

int64_t testInfo::getIntParam(const string& name)
{
    string tmp = params[name];
    if (tmp.length() > LLONG_MAX_LEN) throw genError("too long integer in command line");
    istringstream ss(tmp); int64_t res;
    ss >> res;
    if (ss.fail()) throw genError("trying to get int param from non-int param");
    return res;
}

long double testInfo::getFloatParam(const string& name)
{
    string tmp = params[name];
    istringstream ss(tmp); long double res;
    ss >> res;
    if (ss.fail()) throw genError("trying to get float param from non-float param");
    return res;
}

void testInfo::paramsToVars()
{
    for (map<string, string>::iterator i = params.begin(); i != params.end(); ++i) {
        if (i->first == randseedParamName) SetRandSeed(getIntParam(i->first));
        else {
            ParserObjectWithDataT tmp = ParserFindObject(i->first.c_str(), desc.a, 0);
            if (tmp.objPart) {
                switch (tmp.objPart->objKind) {
                    case PARSER_OBJECT_KIND_INTEGER:
                        desc.operator [](i->first) = getIntParam(i->first);
                        break;
                    case PARSER_OBJECT_KIND_FLOAT:
                        desc.operator [](i->first) = getFloatParam(i->first);
                        break;
                    case PARSER_OBJECT_KIND_STRING:
                        desc.operator [](i->first) = i->second;
                        break;
                    default:
                        throw genError("unexpected object kind");
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
    ParserObjectWithDataT tmp = ParserFindObjectByName(a, name.c_str());
    genError::processParseError();
    if (tmp.objPart) return prxObject(tmp);
    else throw genError("object with name '" + name + "' not found");
}

void prxRecord::print()
{
    if ((!a.pointerToData || !a.pointerToData->data) && a.recPart->n)
        throw genError("uninitialized data while printing");
    for (int i = 0; i < a.recPart->n; i++) {
        ParserObjectWithDataT st;
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
    ParserObjectRecordWithDataT tmp = ParserGetSequenceElement(a, index);
    genError::processParseError();
    return prxRecord(tmp);
}

prxObject& prxObject::operator = (const int64_t& value)
{
    if (a.objPart->objKind != PARSER_OBJECT_KIND_INTEGER)
        throw genError("trying to assign integer to non-int object");
    ParserSetIntegerValue(a, value);
    genError::processParseError();
    return *this;
}

prxObject& prxObject::operator = (const int& value)
{
    *this = static_cast<int64_t>(value);
    return *this;
}

prxObject& prxObject::operator = (const long double& value)
{
    if (a.objPart->objKind != PARSER_OBJECT_KIND_FLOAT)
        throw genError("trying to assign float to non-float object");
    ParserSetFloatValue(a, value);
    genError::processParseError();
    return *this;
}

prxObject& prxObject::operator = (const string& value)
{
    if (a.objPart->objKind != PARSER_OBJECT_KIND_STRING)
        throw genError("trying to assign string to non-string object");
    ParserSetStringValue(a, value.c_str());
    genError::processParseError();
    return *this;
}

prxObject::operator int64_t()
{
    if (a.objPart->objKind != PARSER_OBJECT_KIND_INTEGER)
        throw genError("trying to get integer from non-int object");
    int64_t res = getIntValue(a);
    genError::processParseError();
    return res;
}

prxObject::operator int()
{
    return static_cast<int>(static_cast<int64_t>(*this));
}

prxObject::operator long double()
{
    if (a.objPart->objKind != PARSER_OBJECT_KIND_FLOAT)
        throw genError("trying to get float from non-float object");
    long double res = getFloatValue(a);
    genError::processParseError();
    return res;
}

prxObject::operator string()
{
    if (a.objPart->objKind != PARSER_OBJECT_KIND_STRING)
        throw genError("trying to get string from non-string object");
    string res(getStrValue(a));
    genError::processParseError();
    return res;
}

void prxObject::print()
{
    if ((!a.pointerToData || !a.pointerToData->value) &&
        a.objPart->objKind != PARSER_OBJECT_KIND_NEWLINE &&
        a.objPart->objKind != PARSER_OBJECT_KIND_SOFTLINE)
            throw genError("uninitialized data while printing");
    int objk = a.objPart->objKind;
    switch (objk) {
        case PARSER_OBJECT_KIND_NEWLINE:
        case PARSER_OBJECT_KIND_SOFTLINE:
            cout << endl; wasSpaceChar = true;
            break;
        case PARSER_OBJECT_KIND_INTEGER:
        case PARSER_OBJECT_KIND_FLOAT:
        case PARSER_OBJECT_KIND_STRING:
            if (!wasSpaceChar) cout << defaultSpaceChar;
            if (objk == PARSER_OBJECT_KIND_INTEGER) {
                cout << getIntValue(a);
            } else if (objk == PARSER_OBJECT_KIND_STRING) {
                cout << getStrValue(a);
            } else if (objk == PARSER_OBJECT_KIND_FLOAT) {
                cout << fixed << setprecision(getFloatDig(a)) << getFloatValue(a);
            }
            genError::processParseError();
            wasSpaceChar = 0;
            break;
        case PARSER_OBJECT_KIND_SEQUENCE:
            int n = ParserGetSequenceLength(a);
            for (int i = 1; i <= n; i++) {
                prxRecord tmp(ParserGetSequenceElement(a, i));
                genError::processParseError();
                tmp.print();
            }
            break;
    }
}

void prxObject::autoGen()
{
    switch (a.objPart->objKind) {
        case PARSER_OBJECT_KIND_NEWLINE: case PARSER_OBJECT_KIND_SOFTLINE: cout << endl; break;
        case PARSER_OBJECT_KIND_INTEGER: autoGenInt(a); break;
        case PARSER_OBJECT_KIND_FLOAT: autoGenFloat(a); break;
        case PARSER_OBJECT_KIND_STRING: autoGenStr(a); break;
        case PARSER_OBJECT_KIND_SEQUENCE: autoGenSeq(a); break;
        default:
            throw genError("unexpected object kind");
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
    ParserErrorT* res = validate(s);
    cout << res->code;
    fclose(stdin);*/

/*    fclose(stdout);
    return 0;
} */


#endif
