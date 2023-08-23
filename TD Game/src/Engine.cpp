#include "Components/Entity.h"
#include "Components/Component.h"
#include "Engine.h"
#include "Shader.h"
#include "Camera.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Tools.h"

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Mesh.h>

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
        return;
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
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Engine::StartGameLoop(const std::string path) {
    std::cout << "Starting Game Loop..." << std::endl;
    std::cout << "Loading Shaders... at " << path << std::endl;

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        float m_frameTime = (float)glfwGetTime();
        float m_deltaTime = m_frameTime - this->lastFrameTime;
        this->lastFrameTime = m_frameTime;

        int curWindowWidth, curWindowHeight;
        glfwGetWindowSize(window, &curWindowWidth, &curWindowHeight);

        if (curWindowWidth != m_WindowWidth || curWindowHeight != m_WindowHeight) {
            m_WindowWidth = curWindowWidth;
            m_WindowHeight = curWindowHeight;
            glViewport(0, 0, m_WindowWidth, m_WindowHeight);
        }

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
        ImGui::SetNextWindowPos(ImVec2(m_WindowWidth - 300, 0));
        ImGui::SetNextWindowSize(ImVec2(300, 150));

        float predictedFps = 1.0f / total_time_to_render;

        // disable resize
        ImGui::Begin("Frame Data", NULL, ImGuiWindowFlags_NoResize);
        ImGui::Text("Frame Time: %fms", m_deltaTime * 1000.0f);
        ImGui::Text("Total Update Time: %fms", total_time_to_update * 1000.0f);
        ImGui::Text("Total Render Time: %fms", total_time_to_render * 1000.0f);
        ImGui::Text("Predicted FPS: %f", predictedFps);

        m_FPSGraph.push_back(predictedFps);

        if (m_FPSGraph.size() > 100) {
            m_FPSGraph.erase(m_FPSGraph.begin());
        }

        // plot FPS as a graph
        float fpsHistory[100];
        for (int i = 0; i < 100; i++) {
            if (i < m_FPSGraph.size()) {
                fpsHistory[i] = m_FPSGraph[i];
            }
        }

        ImGui::PlotLines("FPS", fpsHistory, 100);

        ImGui::End();

        DrawEditor();

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

    delete m_MainCamera;
	delete _defaultShader;

    for (auto entity : m_Entities) {
		delete entity;
	}
}

void Engine::Awake() {
    m_MainCamera = new Camera();

    m_MainCamera->m_Transform.m_Position = glm::vec3(0.0f, 2.0f, 5.0f);

    // update every entity
    for (auto entity : m_Entities) {
        for (auto component : entity->m_Components) {
            component->Init();
        }
    }

    const std::string prefix = "C:/Users/ssnee/Desktop/CPP-Game-Engine/TD Game";
    Shader* shader = new Shader();
    shader->LoadFromFiles(prefix + "/src/shaders/default/vertex.glsl", prefix + "/src/shaders/default/fragment.glsl");

    _defaultShader = shader;
}

void Engine::Update(float m_DeltaTime) {
    // update every entity
    for (auto& entity : m_Entities) {
        for (auto& component : entity->m_Components) {
			component->Update(m_DeltaTime);
		}
    }
}

void Engine::Render() {
    for (Entity* entity : m_Entities) {
		// render entity
        Mesh* mesh = nullptr;
        if (entity->TryGetComponent<Mesh>(mesh)) {
            mesh->Render(m_MainCamera, _defaultShader);
        }
	}
}

void Engine::DrawEditor() {
    // create top toolbar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {

            }
        }

        if (ImGui::BeginMenu("Entity Creator")) {
            if (ImGui::MenuItem("Create Plane (1x1)")) {
                Entity* ent = new Entity(GetValidName("Plane"));

                Mesh* mesh = ent->AddComponent<Mesh>();
                mesh->CreatePlane(glm::vec2(1.0f, 1.0f));

                m_Entities.push_back(ent);
            }
            if (ImGui::MenuItem("Create Cube (1x1)")) {
				Entity* ent = new Entity(GetValidName("Cube"));

				Mesh* mesh = ent->AddComponent<Mesh>();
				mesh->CreateCube(glm::vec3(1.0f, 1.0f, 1.0f));

				m_Entities.push_back(ent);
			}
        }
    }

    // hierarchy
    ImGui::SetNextWindowPos(ImVec2(0, 50));
    ImGui::SetNextWindowSize(ImVec2(300, m_WindowHeight-50));

    ImGui::Begin("Hierarchy");

    ImGui::BeginChild("Hierarchy", ImVec2(0, 0), true);

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    int index = 0;

    for (Entity* entity : m_Entities) {
        if (m_SelectedEntity == entity) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		}
        else {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		}

        if (ImGui::Button(entity->Name().c_str(), ImVec2(-FLT_MIN, 0))) {
            m_SelectedEntity = entity;

            std::cout << "Selected Entity: " << m_SelectedEntity->Name() << index << std::endl;
        }

        index++;

        ImGui::PopStyleColor();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    ImGui::EndChild();

    ImGui::End();

    // inspector
    ImGui::SetNextWindowPos(ImVec2(m_WindowWidth - 500, 50));
    ImGui::SetNextWindowSize(ImVec2(500, m_WindowHeight-50));

    ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoResize);
    if (m_SelectedEntity != nullptr) {
        ImGui::Text(m_SelectedEntity->Name().c_str());

        ImGui::BeginChild("Transform", ImVec2(0, 0), true);

        ImGui::Columns(4, "TransformColumns");
        // position
        ImGui::PushID("Position");
        ImGui::Text("Position");
        ImGui::NextColumn();
        ImGui::InputFloat("X", &m_SelectedEntity->m_Transform.m_Position.x, 0.0f, 0.0f, "%.3f");
        ImGui::NextColumn();
        ImGui::InputFloat("Y", &m_SelectedEntity->m_Transform.m_Position.y, 0.0f, 0.0f, "%.3f");
        ImGui::NextColumn();
        ImGui::InputFloat("Z", &m_SelectedEntity->m_Transform.m_Position.z, 0.0f, 0.0f, "%.3f");
        ImGui::NextColumn();
        ImGui::PopID();

        // rotation
        ImGui::PushID("Rotation");
        ImGui::Text("Rotation");
        ImGui::NextColumn();
        ImGui::InputFloat("X", &m_SelectedEntity->m_Transform.m_Rotation.x, 0.0f, 0.0f, "%.3f");
        ImGui::NextColumn();
        ImGui::InputFloat("Y", &m_SelectedEntity->m_Transform.m_Rotation.y, 0.0f, 0.0f, "%.3f");
        ImGui::NextColumn();
        ImGui::InputFloat("Z", &m_SelectedEntity->m_Transform.m_Rotation.z, 0.0f, 0.0f, "%.3f");
        ImGui::NextColumn();
        ImGui::PopID();

        // scale
        ImGui::PushID("Scale");
        ImGui::Text("Scale");
        ImGui::NextColumn();
        ImGui::InputFloat("X", &m_SelectedEntity->m_Transform.m_Scale.x, 0.0f, 0.0f, "%.3f");
        ImGui::NextColumn();
        ImGui::InputFloat("Y", &m_SelectedEntity->m_Transform.m_Scale.y, 0.0f, 0.0f, "%.3f");
        ImGui::NextColumn();
        ImGui::InputFloat("Z", &m_SelectedEntity->m_Transform.m_Scale.z, 0.0f, 0.0f, "%.3f");
        ImGui::PopID();

        ImGui::EndChild();
    }
    ImGui::End();
}

std::string Engine::GetValidName(std::string name) {
    for (Entity* ent : m_Entities) {
        if (ent->Name() == name) {
            return GetValidName(name + " (1)");
        }
    }

    return name;
}