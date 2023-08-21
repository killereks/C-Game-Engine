#include "Engine.h"
#include "Shader.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Tools.h"

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

Engine::Engine(int width, int height) {
    GLFWwindow* window;

    m_WindowWidth = width;
    m_WindowHeight = height;

    lastFrameTime = 0.0f;

    /* Initialize the library */
    if (!glfwInit())
        return;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "TD Game", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }

    this->window = window;

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Glew Initialize Error!" << std::endl;
    }

    // setup opengl stuff
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Awake();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Engine::StartGameLoop(const std::string path) {
    std::cout << "Starting Game Loop..." << std::endl;
    std::cout << "Loading Shaders... at " << path << std::endl;

    const std::string prefix = "C:/Users/ssnee/Desktop/CPP-Game-Engine/TD Game";

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    Shader shader = Shader();
    shader.LoadFromFiles(prefix+"/src/shaders/default/vertex.glsl", prefix + "/src/shaders/default/fragment.glsl");

    _defaultShader = &shader;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        float m_frameTime = (float)glfwGetTime();
        float m_deltaTime = m_frameTime - this->lastFrameTime;
        this->lastFrameTime = m_frameTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glfwPollEvents();

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double update_time = glfwGetTime();
        Update(m_deltaTime);
        double total_time_to_update = glfwGetTime() - update_time;
        double render_time = glfwGetTime();
        Render();
        double total_time_to_render = glfwGetTime() - render_time;

        // bottom right
        ImGui::SetNextWindowPos(ImVec2(m_WindowWidth - 300, m_WindowHeight - 100));
        ImGui::SetNextWindowSize(ImVec2(300, 100));

        // disable resize
        ImGui::Begin("Frame Data", NULL, ImGuiWindowFlags_NoResize);
        ImGui::Text("Frame Time: %fms", m_deltaTime * 1000.0f);
        ImGui::Text("Total Update Time: %fms", total_time_to_update * 1000.0f);
        ImGui::Text("Total Render Time: %fms", total_time_to_render * 1000.0f);
        ImGui::Text("Predicted FPS: %f", 1.0f / total_time_to_render);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        int rightClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

        if (rightClick == GLFW_RELEASE){
            glfwGetCursorPos(window, &mousePosX, &mousePosY);
        }

        if (rightClick == GLFW_PRESS) {
            float speed = 5.0f;

            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                speed *= 2.0f;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                m_MainCamera->m_Transform.Translate(-m_MainCamera->m_Transform.Forward() * m_deltaTime * speed);
            else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                m_MainCamera->m_Transform.Translate(m_MainCamera->m_Transform.Forward() * m_deltaTime * speed);

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                m_MainCamera->m_Transform.Translate(-m_MainCamera->m_Transform.Right() * m_deltaTime * speed);
            else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                m_MainCamera->m_Transform.Translate(m_MainCamera->m_Transform.Right() * m_deltaTime * speed);

            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
                m_MainCamera->m_Transform.Translate(-m_MainCamera->m_Transform.Up() * m_deltaTime * speed);
            } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
                m_MainCamera->m_Transform.Translate(m_MainCamera->m_Transform.Up() * m_deltaTime * speed);
            }

            double mousePosXNow = 0.0f;
            double mousePosYNow = 0.0f;

            glfwGetCursorPos(window, &mousePosXNow, &mousePosYNow);

            double mouseDeltaX = mousePosXNow - mousePosX;
            double mouseDeltaY = mousePosYNow - mousePosY;

            mousePosX = mousePosXNow;
            mousePosY = mousePosYNow;

            // rotate camera based on the rotation
            cameraX -= mouseDeltaY * 0.1f;
            cameraY -= mouseDeltaX * 0.1f;

            if (cameraX > 89.0f)
                cameraX = 89.0f;

            if (cameraX < -89.0f)
                cameraX = -89.0f;

            m_MainCamera->m_Transform.SetRotationEuler(glm::vec3(cameraX, cameraY, 0.0f));
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }
}

Engine::~Engine() {
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}

void Engine::Awake() {
    m_MainCamera = new Camera();
}

void Engine::Update(float m_DeltaTime) {
    // attach this window to top left
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 100));

    ImGui::Begin("Game Options", NULL, ImGuiWindowFlags_NoResize);
    ImGui::Text("Total enemy count: %d", m_Entities.size());

    if (ImGui::Button("Spawn Enemy")){
        Entity* entity = new Entity();

        m_Entities.push_back(entity);
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(300, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 120));

    ImGui::Begin("Camera Options", NULL, ImGuiWindowFlags_NoResize);
    glm::vec3 right = m_MainCamera->m_Transform.Right();
    ImGui::Text("Right Vector: %f %f %f", right.x, right.y, right.z);
    glm::vec3 up = m_MainCamera->m_Transform.Up();
    ImGui::Text("Up Vector: %f %f %f", up.x, up.y, up.z);
    glm::vec3 forward = m_MainCamera->m_Transform.Forward();
    ImGui::Text("Forward Vector: %f %f %f", forward.x, forward.y, forward.z);
    ImGui::Text("Position: %f %f %f", m_MainCamera->m_Transform.m_Position.x, m_MainCamera->m_Transform.m_Position.y, m_MainCamera->m_Transform.m_Position.z);
    glm::vec3 rotation = m_MainCamera->m_Transform.GetEuler();
    ImGui::Text("Rotation: %f %f %f", rotation.x, rotation.y, rotation.z);
    ImGui::End();
}

void Engine::Render() {
    for (Entity* entity : m_Entities) {
		// render entity
		entity->Render(m_MainCamera, _defaultShader);
	}
}