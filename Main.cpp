#include "IBLDemo.h"

int main()
{
    IBLDemo iblDemo;
    Inputs::bindEngine(&iblDemo);
    Inputs::setup();
    return iblDemo.run();
}
