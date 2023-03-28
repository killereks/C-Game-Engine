#include "Engine.h"
#include "Shader.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <filesystem>
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

    Awake();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // LOAD DEFAULT SHADER
    std::string cwd = std::filesystem::current_path().string();
    _defaultShader.LoadFromFiles(cwd + "/src/shaders/default/vertex.glsl", cwd + "/src/shaders/default/fragment.glsl");
}

void Engine::StartGameLoop() {
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

        Update(m_deltaTime);
        Render();

        float total_time_to_render = (float)glfwGetTime() - m_frameTime;

        ImGui::Begin("Frame Data");
        ImGui::Text("Frame Time: %fms", m_deltaTime * 1000.0f);
        ImGui::Text("Render Time: %fms", total_time_to_render * 1000.0f);
        ImGui::Text("Predicted FPS: %f", 1.0f / total_time_to_render);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
    
}

void Engine::Update(float m_DeltaTime) {
    ImGui::Begin("Game Options");
    ImGui::Text("Total enemy count: %d", m_Entities.size());

    if (ImGui::Button("Spawn Enemy")){
        Entity* entity = new Entity();

        glm::vec2 pos = glm::vec2(std::rand() / (float)RAND_MAX, std::rand() / (float)RAND_MAX);
        entity->SetPosition(pos);
        entity->SetRotation(std::rand() / (float)RAND_MAX * 6.28f);
        entity->SetScale(glm::vec2(0.1f, 0.1f));

        m_Entities.push_back(entity);
    }
    ImGui::End();
}

void Engine::Render() {
    for (Entity* entity : m_Entities) {
		// render entity
		entity->Render(_defaultShader);
	}
}