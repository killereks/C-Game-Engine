#include <iostream>
#include <vector>

#include "entity/entity.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Engine {
public:
    Engine(int width, int height);
    ~Engine();

    void Awake();
    void Update(float m_DeltaTime);
    void Render();

    void StartGameLoop();

private:
    int m_WindowWidth;
    int m_WindowHeight;

    float lastFrameTime;

    GLFWwindow* window;

    Shader _defaultShader;

    std::vector<Entity*> m_Entities;
};