#include "easygen.h"

int main(int argc, char* argv[])
{
    try {
        testInfo super(argc, argv);
        super.autoGen();
    } catch (genError a) {
        cout << a.err() << endl;
    }
    return 0;
}
