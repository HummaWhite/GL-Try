#include "Engine.h"
#include "Inputs.h"
#include "SceneLoader.h"

int main()
{
    Engine engine;
    Inputs::bindEngine(&engine);
    Inputs::setup();
    return engine.run();
}
