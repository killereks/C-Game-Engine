#include <iostream>
#include <vector>

#include "entity/entity.h"
#include "Camera.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Engine {
public:
    Engine(int width, int height);
    ~Engine();

    Camera* m_MainCamera;

    void Awake();
    void Update(float m_DeltaTime);
    void Render();

    void StartGameLoop(std::string path);

private:
    int m_WindowWidth;
    int m_WindowHeight;

    double mousePosX = 0, mousePosY = 0;
    float cameraX = 0, cameraY = 0;

    float lastFrameTime;

    GLFWwindow* window;

    Shader* _defaultShader;

    std::vector<Entity*> m_Entities;
};