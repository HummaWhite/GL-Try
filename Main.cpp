#include "DeferedEngine.h"

int main()
{
    DeferedEngine engine;
    Inputs::bindEngine(&engine);
    Inputs::setup();
    return engine.run();
}
