#pragma once

#include "Components/Entity.h"
#include "Components/Component.h"
#include "Engine.h"
#include "Shader.h"
#include "Camera.h"
#include "Physics.h"
#include "Ray.h"
#include "Bounds.h"

#include "Tools.h"

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Mesh.h>
#include <ext.hpp>
#include <Light.h>
#include <filesystem>

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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Engine::StartGameLoop(const std::string path, const std::string projectPath) {
    std::string shaderPath = path + "/shaders/default/";

    m_ProjectPath = projectPath;

    std::cout << "Starting Game Loop..." << std::endl;
    std::cout << "Loading Shaders... at " << shaderPath << std::endl;

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    Shader* shader = new Shader();
    shader->LoadFromFiles(shaderPath +"/vertex.glsl", shaderPath+"/fragment.glsl");

    _defaultShader = shader;

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
        ImGuizmo::BeginFrame();

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

        EditorLoop(m_deltaTime);

        // bottom right
        ImGui::SetNextWindowPos(ImVec2(m_WindowWidth - 300, 20));
        ImGui::SetNextWindowSize(ImVec2(300, 200));

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
        float maxFPS = 0;
        for (int i = 0; i < 100; i++) {
            if (i < m_FPSGraph.size()) {
                fpsHistory[i] = m_FPSGraph[i];
                maxFPS = glm::max(maxFPS, m_FPSGraph[i]);
            }
        }

        //ImGui::PlotHistogram("FPS", fpsHistory, 100);

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        ImVec2 plotPos = ImGui::GetCursorScreenPos();
        ImVec2 plotSize = ImGui::GetContentRegionAvail();
        float xScale = plotSize.x / 100;
        float yScale = plotSize.y;

        for (int i = 1; i < 100; i++) {
            // Calculate the line start and end positions
            ImVec2 rectPos = ImVec2(plotPos.x + i * xScale, plotPos.y + (1 - fpsHistory[i] / maxFPS) * yScale);
            ImVec2 rectSize = ImVec2(xScale, (fpsHistory[i] / maxFPS) * yScale);

            // Choose the line color based on the FPS value
            ImU32 lineColor;
            if (fpsHistory[i] < 30) {
                lineColor = IM_COL32(255, 0, 0, 255); // Red
            }
            else if (fpsHistory[i] < 60) {
                lineColor = IM_COL32(255, 255, 0, 255); // Yellow
            }
            else {
                lineColor = IM_COL32(0, 255, 0, 255); // Green
            }

            // Draw the line
            drawList->AddRectFilled(rectPos, ImVec2(rectPos.x + rectSize.x, rectPos.y + rectSize.y), lineColor);
        }

        ImGui::End();

        DrawEditor();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (!ImGuizmo::IsOver() && !ImGui::GetIO().WantCaptureMouse) {
            int leftClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

            if (leftClick == GLFW_RELEASE) {
                leftClicked = false;
            }

            if (leftClick == GLFW_PRESS && !leftClicked) {
                // raycast
                float mouseX = (float)mousePosX;
                float mouseY = (float)mousePosY;

                leftClicked = true;

                Ray ray = m_MainCamera->ScreenPointToRay(glm::vec2(mouseX, mouseY), glm::vec2(m_WindowWidth, m_WindowHeight));

                for (auto& ent : m_Entities) {
                    Mesh* mesh = nullptr;
                    if (ent->TryGetComponent(mesh)) {
                        if (Physics::RayCollisionAABB(ray, mesh->GetBounds())) {
                            m_SelectedEntity = ent;
                            std::cout << "Selected Entity: " << ent->m_Name << std::endl;
                            break;
                        }
                    }
                }
            }
        }

        int rightClick = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

        if (rightClick == GLFW_RELEASE){
            glfwGetCursorPos(window, &mousePosX, &mousePosY);
        }

        if (rightClick == GLFW_PRESS) {
            float speed = 5.0f;

            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                speed *= 5.0f;

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
        else {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                m_CurrentOperation = ImGuizmo::TRANSLATE;
            }
            else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
                m_CurrentOperation = ImGuizmo::ROTATE;
            }
            else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
                m_CurrentOperation = ImGuizmo::SCALE;
            }
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
    _defaultShader->Bind();

    std::vector<Light*> lights;

    for (auto& entity : m_Entities) {
		Light* light = nullptr;
        if (entity->TryGetComponent<Light>(light)) {
            lights.push_back(light);
		}
	}

    for (int i = 0; i < lights.size(); i++) {
        std::string uniformName = "lights[" + std::to_string(i) + "]";

        _defaultShader->SetVec3(uniformName + ".position", lights[i]->m_Owner->m_Transform.m_Position);
        _defaultShader->SetVec3(uniformName + ".color", lights[i]->m_Color);
        _defaultShader->SetFloat(uniformName + ".intensity", lights[i]->m_Intensity);
        _defaultShader->SetFloat(uniformName + ".range", lights[i]->m_Range);
	}

    for (Entity* entity : m_Entities) {
		// render entity
        Mesh* mesh = nullptr;
        if (entity->TryGetComponent<Mesh>(mesh)) {
            mesh->Render(m_MainCamera, _defaultShader);
        }
	}

    _defaultShader->Unbind();
}

void Engine::DrawEditor() {
    if (ImGuiFileDialog::Instance()->Display("ChooseFBX")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            std::string modelName = filePathName.substr(filePathName.find_last_of("/\\") + 1);

            Entity* ent = new Entity(GetValidName(modelName));
            Mesh* mesh = ent->AddComponent<Mesh>();
            mesh->LoadFromFile(filePathName);

            m_Entities.push_back(ent);
        }

        ImGuiFileDialog::Instance()->Close();
    }
    // create top toolbar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {

            }
            if (ImGui::MenuItem("Open FBX")) {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseFBX", "Choose FBX", ".fbx", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Entity Creator")) {
            if (ImGui::BeginMenu("Create 3D Object")) {
                if (ImGui::MenuItem("Plane (1x1)")) {
                    Entity* ent = new Entity(GetValidName("Plane"));

                    Mesh* mesh = ent->AddComponent<Mesh>();
                    mesh->CreatePlane(glm::vec2(1.0f, 1.0f));

                    m_Entities.push_back(ent);
                }
                if (ImGui::MenuItem("Cube (1x1)")) {
                    Entity* ent = new Entity(GetValidName("Cube"));

                    Mesh* mesh = ent->AddComponent<Mesh>();
                    mesh->CreateCube(glm::vec3(1.0f, 1.0f, 1.0f));

                    m_Entities.push_back(ent);
                }
                if (ImGui::MenuItem("Sphere (1x1)")) {
                    Entity* ent = new Entity(GetValidName("Sphere"));

                    Mesh* mesh = ent->AddComponent<Mesh>();
                    mesh->CreateSphere(1.0f, 16, 16);

                    m_Entities.push_back(ent);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Create Lights")) {
                if (ImGui::MenuItem("Light")) {
                    Entity* ent = new Entity(GetValidName("Light"));

                    Light* light = ent->AddComponent<Light>();

                    m_Entities.push_back(ent);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Camera Settings")) {
            ImGui::BeginGroup();
            ImGui::DragFloat("FOV", &m_MainCamera->m_FOV, 1.f, 1.f, 179.f, "%.0f");
            ImGui::DragFloat("Near", &m_MainCamera->m_Near, 0.1f, 0.1f, 100.f, "%.1f");
            ImGui::DragFloat("Far", &m_MainCamera->m_Far, 1.f, 1.f, 1000.f, "%.0f");
            ImGui::End();

            ImGui::EndMenu();
        }
    }

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode, nullptr);

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

        if (ImGui::Button(entity->m_Name.c_str(), ImVec2(-FLT_MIN, 0))) {
            m_SelectedEntity = entity;

            std::cout << "Selected Entity: " << m_SelectedEntity->m_Name << index << std::endl;
        }

        index++;

        ImGui::PopStyleColor();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    ImGui::EndChild();

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

    ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoResize);

    if (m_SelectedEntity != nullptr) {
        ImGui::Text(m_SelectedEntity->m_Name.c_str());

        if (ImGui::CollapsingHeader("Transform")) {
            ImGui::BeginGroup();
            // position
            ImGui::PushID("Position");
            ImGui::Text("Position");
            ImGui::InputFloat3("##Position", &m_SelectedEntity->m_Transform.m_Position.x);
            ImGui::PopID();

            // rotation
            ImGui::PushID("Rotation");
            ImGui::Text("Rotation");
            ImGui::InputFloat4("##Rotation", &m_SelectedEntity->m_Transform.m_Rotation.x);
            ImGui::PopID();

            // scale
            ImGui::PushID("Scale");
            ImGui::Text("Scale");
            ImGui::InputFloat3("##Scale", &m_SelectedEntity->m_Transform.m_Scale.x);
            ImGui::PopID();

            ImGui::EndGroup();
        }

        // display components
        ImGui::BeginGroup();
        for (Component* component : m_SelectedEntity->m_Components) {
            if (ImGui::CollapsingHeader(component->GetName().c_str())) {
                component->DrawInspector();
                component->DrawGizmos();
            }
		}
        ImGui::EndGroup();
    }
    ImGui::End();

    ImGui::PopStyleColor(2);

    ImGui::Begin("File System");

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    for (auto& p : std::filesystem::directory_iterator(m_ProjectPath)) {
        if (p.is_directory()) {
            std::string name = p.path().filename().string();
            if (ImGui::Button(name.c_str(), ImVec2(-FLT_MIN, 0))) {
				std::cout << name << std::endl;
			}
		}
	}

    ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	ImGui::End();

	ImGui::Render();
}

std::string Engine::GetValidName(std::string name) {
    for (Entity* ent : m_Entities) {
        if (ent->m_Name == name) {
            return GetValidName(name + " (1)");
        }
    }

    return name;
}

void Engine::EditorLoop(float dt) {
    if (m_SelectedEntity != nullptr) {
        EditTransform(m_SelectedEntity);
    }
}

void Engine::EditTransform(Entity* ent) {
    // W = Translation
    // E = Rotation
    // R = Scale

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    glm::mat4 viewMatrix = m_MainCamera->GetViewMatrix();
    glm::mat4 projectionMatrix = m_MainCamera->GetProjectionMatrix();
    glm::mat4 matrix = ent->m_Transform.GetModelMatrix();

    float* matrixPtr = glm::value_ptr(matrix);
    float* viewMatrixPtr = glm::value_ptr(viewMatrix);
    float* projectionMatrixPtr = glm::value_ptr(projectionMatrix);

    ImGuizmo::Manipulate(viewMatrixPtr, projectionMatrixPtr, m_CurrentOperation, ImGuizmo::WORLD, matrixPtr);

    glm::vec3 translation, rotation, scale;
    ImGuizmo::DecomposeMatrixToComponents(matrixPtr, &translation.x, &rotation.x, &scale.x);

    ent->m_Transform.m_Position = translation;
    ent->m_Transform.SetRotationEuler(rotation);
    ent->m_Transform.m_Scale = scale;
}