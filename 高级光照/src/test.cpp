#include "Engine.h"
#include "Inputs.h"

int main()
{
    Engine engine(1280, 720);
    Inputs::bindEngine(&engine);
    Inputs::setup();
    return engine.run();
}
