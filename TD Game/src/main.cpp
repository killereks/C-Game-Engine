#include "Engine.h"
#include "Shader.h"
#include "Tools.h"

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(int argc, char** argv){
    Engine engine(1080, 720);

    std::string cwd = argv[0];

    engine.StartGameLoop(cwd);

    return 0;
}