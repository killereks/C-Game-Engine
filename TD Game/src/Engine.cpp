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

#include "yaml-cpp/yaml.h"

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Mesh.h>
#include <ext.hpp>
#include <Light.h>
#include <filesystem>
#include <Icons/IconsFontAwesome5.h>

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
    std::string shaderPath = path + "/shaders/";

    m_ProjectPath = projectPath;

    std::cout << "Starting Game Loop..." << std::endl;
    std::cout << "Loading Shaders... at " << shaderPath << std::endl;

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    Shader* shader = new Shader("Default");
    shader->LoadFromFiles(shaderPath +"/default/vertex.glsl", shaderPath+"/default/fragment.glsl");

    _defaultShader = shader;

    Shader* lightShader = new Shader("Shadow");
    lightShader->LoadFromFiles(shaderPath + "/shadows/vertex.glsl", shaderPath + "/shadows/fragment.glsl");

    _lightShader = lightShader;

    auto io = ImGui::GetIO();

    io.Fonts->AddFontDefault();
    float baseFontSize = 20.0f;
    float iconFontSize = baseFontSize * 2.0f / 3.0f;

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;
    std::string fontPath = path + "/fa-solid-900.ttf";
    std::cout << "Attempting to load font from " << fontPath << std::endl;
    io.Fonts->AddFontFromFileTTF(fontPath.c_str(), iconFontSize, &icons_config, icons_ranges);

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

        float predictedFps = 1.0f / total_time_to_render;

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode, nullptr);

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

                Entity* curSelectedEntity = nullptr;
                float minHitDist = 1000000.0f;

                for (auto& ent : m_Entities) {
                    Mesh* mesh = nullptr;
                    if (ent->TryGetComponent(mesh)) {
                        float hitDist = 0.0f;
                        if (Physics::RayCollisionAABB(ray, mesh->GetBounds(), hitDist)) {
                            if (hitDist < minHitDist) {
								minHitDist = hitDist;
								curSelectedEntity = ent;
							}
                        }
                    }
                }

                if (curSelectedEntity) {
					m_SelectedEntity = curSelectedEntity;
				}
                else {
					m_SelectedEntity = nullptr;
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

    delete lightMapper;

    for (auto entity : m_Entities) {
		delete entity;
	}
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& q) {
	out << YAML::Flow;
	out << YAML::BeginSeq << q.x << q.y << q.z << q.w << YAML::EndSeq;
	return out;
}

namespace YAML {
    template<>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs) {
            if (!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();

            return true;
        }
    };

    template<>
    struct convert<glm::vec4> {
        static Node encode(const glm::vec4& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs) {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();

            return true;
        }
    };

    template<>
    struct convert<glm::quat> {
        static Node encode(const glm::quat& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::quat& rhs) {
            if (!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();

            return true;
        }
    };
}

static void SerializeEntity(YAML::Emitter& out, Entity* ent) {
    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << ent->m_Name;

    // transform
    out << YAML::Key << "Transform";
    out << YAML::BeginMap;

    out << YAML::Key << "Position" << YAML::Value << ent->m_Transform.m_Position;
    out << YAML::Key << "Rotation" << YAML::Value << ent->m_Transform.m_Rotation;
    out << YAML::Key << "Scale" << YAML::Value << ent->m_Transform.m_Scale;

    out << YAML::EndMap;

    if (ent->HasComponent<Mesh>()) {
        out << YAML::Key << "Mesh";
        out << YAML::BeginMap;

        Mesh* mesh = ent->GetComponent<Mesh>();
        out << YAML::Key << "File" << YAML::Value << mesh->filePath;
        out << YAML::Key << "CastShadows" << YAML::Value << mesh->m_CastShadows;

        out << YAML::EndMap;
    }

    if (ent->HasComponent<Light>()) {
        out << YAML::Key << "Light";
        out << YAML::BeginMap;

        Light* light = ent->GetComponent<Light>();
        out << YAML::Key << "Intensity" << YAML::Value << light->m_Intensity;
        out << YAML::Key << "Color" << YAML::Value << light->m_Color;
        out << YAML::Key << "Range" << YAML::Value << light->m_Range;

        out << YAML::EndMap;
    }

    out << YAML::EndMap;
}

void Engine::SaveScene(std::string path)
{
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

    for (Entity* ent : m_Entities){
        SerializeEntity(out, ent);
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(path);
    fout << out.c_str();
}

void Engine::LoadScene(std::string path)
{
    std::ifstream stream(path);
    std::stringstream strStream;
    strStream << stream.rdbuf();

    YAML::Node data = YAML::Load(strStream.str());
    // check if valid
    if (!data["Entities"]) {
        std::cout << "Invalid Scene File" << std::endl;
        return;
    }

    m_SelectedEntity = nullptr;

    // clear current scene
    for (Entity* ent : m_Entities) {
		delete ent;
	}

    m_Entities.clear();

    // load entities
    YAML::Node entities = data["Entities"];
    if (entities) {
        for (auto entityData : entities) {
            std::string name = entityData["Entity"].as<std::string>();
            Entity* ent = new Entity(name);

            // transform
            YAML::Node transform = entityData["Transform"];
            if (transform) {
				ent->m_Transform.m_Position = transform["Position"].as<glm::vec3>();
				ent->m_Transform.m_Rotation = transform["Rotation"].as<glm::quat>();
				ent->m_Transform.m_Scale = transform["Scale"].as<glm::vec3>();
			}

            // light
			YAML::Node light = entityData["Light"];
            if (light) {
				Light* lightComponent = ent->AddComponent<Light>();
				lightComponent->m_Intensity = light["Intensity"].as<float>();
				lightComponent->m_Color = light["Color"].as<glm::vec3>();
				lightComponent->m_Range = light["Range"].as<float>();
			}

            // mesh
            YAML::Node mesh = entityData["Mesh"];
            if (mesh) {
                Mesh* meshComponent = ent->AddComponent<Mesh>();
                meshComponent->filePath = mesh["File"].as<std::string>();
                meshComponent->m_CastShadows = mesh["CastShadows"].as<bool>();

                meshComponent->LoadFromFileOBJ(meshComponent->filePath);
            }

			m_Entities.push_back(ent);
        }
    }
}

void Engine::Awake() {
    m_MainCamera = new Camera(90.0f, 0.1f, 1000.0f);

    m_MainCamera->m_Transform.m_Position = glm::vec3(0.0f, 2.0f, 5.0f);

    lightMapper = new LightMapper(2048, 2048);

    // update every entity
    for (auto entity : m_Entities) {
        for (auto component : entity->m_Components) {
            component->Init();
        }
    }

    // setup basic scene
    Entity* ent = new Entity("Plane");
    Mesh* mesh = ent->AddComponent<Mesh>();
    mesh->CreatePlane(glm::vec3(20));

    ent->m_Transform.m_Position = glm::vec3(0.0f, -1.0f, 0.0f);

    m_Entities.push_back(ent);

    ent = new Entity("Sphere");
    mesh = ent->AddComponent<Mesh>();
    mesh->CreateSphere(1.0f, 32, 32);

    ent->m_Transform.m_Position = glm::vec3(0.0f, 2.0f, 0.0f);

    m_Entities.push_back(ent);

    ent = new Entity("Light");
    Light* light = ent->AddComponent<Light>();

    ent->m_Transform.m_Position = glm::vec3(1.0f, 5.0f, -1.0f);
    ent->m_Transform.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

    m_Entities.push_back(ent);
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

    Light directional_light;
    bool foundLight = false;

    Camera* lightCam = nullptr;

    for (auto& entity : m_Entities) {
		Light* light = nullptr;
        if (entity->TryGetComponent<Light>(light)) {
            directional_light = *light;
            foundLight = true;
		}
	}

    if (foundLight){
        // light camera
        lightCam = new Camera(-20, 20, -20, 20, -30, 30);

        directional_light.m_Owner->m_Transform.LookAt(glm::vec3(0.0f));

        lightMapper->Bind();

        _lightShader->Bind();

        glm::mat4 lightSpaceMatrix = lightCam->GetProjectionMatrix() * directional_light.m_Owner->m_Transform.GetModelMatrix();

        _lightShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);

        // enable face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        // LIGHT PASS
        for (Entity* entity : m_Entities) {
            // render entity
            Mesh* mesh = nullptr;
            if (entity->TryGetComponent<Mesh>(mesh)) {
                if (!mesh->m_CastShadows) continue;

                _lightShader->SetMat4("model", mesh->m_Owner->m_Transform.GetModelMatrix());
                mesh->Draw();
            }
        }
        lightMapper->Unbind();
    }

    glViewport(0, 0, m_WindowWidth, m_WindowHeight);

    glCullFace(GL_BACK);

    // RENDER PASS
    for (Entity* entity : m_Entities) {
		// render entity
        Mesh* mesh = nullptr;
        if (entity->TryGetComponent<Mesh>(mesh)) {
            mesh->SetupRender(m_MainCamera, _defaultShader);

            if (foundLight) {
                lightMapper->SetUniforms(_defaultShader);

                glm::mat4 lightProjection = lightCam->GetProjectionMatrix() * directional_light.m_Owner->m_Transform.GetModelMatrix();
                _defaultShader->SetMat4("lightProjection", lightProjection);
                _defaultShader->SetVec3("lightDir", directional_light.m_Owner->m_Transform.Forward());
            }

            _defaultShader->SetVec3("camFwd", m_MainCamera->m_Transform.Forward());

            if (m_SelectedEntity == entity) {
                _defaultShader->SetInt("selected", 1);
            }
            else {
                _defaultShader->SetInt("selected", 0);
            }

            mesh->Draw();
        }
	}

    _defaultShader->Unbind();

    delete lightCam;
}

void Engine::DrawEditor() {
    if (ImGuiFileDialog::Instance()->Display("ChooseFBX")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            std::string modelName = filePathName.substr(filePathName.find_last_of("/\\") + 1);

            Entity* ent = new Entity(GetValidName(modelName));
            Mesh* mesh = ent->AddComponent<Mesh>();
            mesh->LoadFromFileFBX(filePathName);

            m_Entities.push_back(ent);
        }

        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseOBJ")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

            std::string modelName = filePathName.substr(filePathName.find_last_of("/\\") + 1);

            Entity* ent = new Entity(GetValidName(modelName));
            Mesh* mesh = ent->AddComponent<Mesh>();
            mesh->LoadFromFileOBJ(filePathName);

            m_Entities.push_back(ent);
        }

        ImGuiFileDialog::Instance()->Close();
    }
    // create top toolbar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu(ICON_FA_FILE" File")) {
            if (ImGui::MenuItem("Save Scene")) {
                SaveScene(m_ProjectPath + "/main.scene");
            }
            if (ImGui::BeginMenu("Open")) {
                if (ImGui::MenuItem("FBX")) {
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFBX", "Choose FBX", ".fbx", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
                }
                if (ImGui::MenuItem("OBJ")) {
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseOBJ", "Choose OBJ", ".obj", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(ICON_FA_CUBE" Entity Creator")) {
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

        if (ImGui::BeginMenu(ICON_FA_CAMERA" Camera Settings")) {
            ImGui::BeginGroup();
            ImGui::DragFloat("FOV", &m_MainCamera->m_FOV, 1.f, 1.f, 179.f, "%.0f");
            ImGui::DragFloat("Near", &m_MainCamera->m_Near, 0.1f, 0.1f, 100.f, "%.1f");
            ImGui::DragFloat("Far", &m_MainCamera->m_Far, 1.f, 1.f, 1000.f, "%.0f");
            ImGui::EndGroup();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::Begin(ICON_FA_SITEMAP " Hierarchy");

    ImGui::BeginChild("Hierarchy", ImVec2(0, 0), true);

    int index = 0;

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    for (Entity* entity : m_Entities) {
        if (m_SelectedEntity == entity) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		}
        else {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		}

        float windowWidth = ImGui::GetWindowWidth();
        float buttonWidth = windowWidth - 50;

        std::string name = ICON_FA_CUBE" " + entity->m_Name;
        if (ImGui::Button(name.c_str(), ImVec2(buttonWidth, 0))) {
            m_SelectedEntity = entity;
        }

        ImGui::SameLine();
        ImGui::PushID(index);

        if (ImGui::Button(ICON_FA_TRASH, ImVec2(50, 0))) {
			m_Entities.erase(m_Entities.begin() + index);
			delete entity;
			m_SelectedEntity = nullptr;
            --index;
		}
        ImGui::PopID();

        index++;

        ImGui::PopStyleColor();
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    ImGui::EndChild();

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));

    ImGui::Begin(ICON_FA_BINOCULARS" Inspector", NULL, ImGuiWindowFlags_NoResize);

    if (m_SelectedEntity != nullptr) {
        ImGui::Text(m_SelectedEntity->m_Name.c_str());

        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.6f, 0.6f, 0.6f, 0.6f));

        if (ImGui::CollapsingHeader(ICON_FA_ARROWS_ALT " Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

            ImGui::BeginGroup();
            ImGui::PushItemWidth(-FLT_MIN);
            // position
            ImGui::PushID("Position");
            ImGui::Text("Position");
            ImGui::InputFloat3("##Position", &m_SelectedEntity->m_Transform.m_Position.x);
            ImGui::PopID();

            glm::vec3 rotation = m_SelectedEntity->m_Transform.GetEuler();

            // rotation
            ImGui::PushID("Rotation");
            ImGui::Text("Rotation");
            ImGui::InputFloat3("##Rotation", &rotation[0]);
            ImGui::PopID();

            m_SelectedEntity->m_Transform.SetRotationEuler(rotation);
            // scale
            ImGui::PushID("Scale");
            ImGui::Text("Scale");
            ImGui::InputFloat3("##Scale", &m_SelectedEntity->m_Transform.m_Scale.x);
            ImGui::PopID();

            ImGui::Separator();

            if (ImGui::Button("Look At Origin", ImVec2(-FLT_MIN, 0))) {
                m_SelectedEntity->m_Transform.LookAt(glm::vec3(0.0f));
            }

            ImGui::Separator();

            ImGui::PopItemWidth();

            ImGui::EndGroup();
        }

        ImGui::PopStyleColor();

        // display components
        ImGui::BeginGroup();

        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.6f, 0.6f, 0.6f, 0.6f));

        for (Component* component : m_SelectedEntity->m_Components) {
            float windowWidth = ImGui::GetWindowWidth();

            if (ImGui::CollapsingHeader(component->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                component->DrawInspector();
                component->DrawGizmos();

                if (ImGui::Button(ICON_FA_TRASH" Remove Component", ImVec2(-FLT_MIN, 0))) {
                    m_SelectedEntity->RemoveComponent(component);
                }
            }
            ImGui::Separator();
		}

        ImGui::PopStyleColor();

        ImGui::EndGroup();

        // display add component button
        ImGui::BeginGroup();

        if (ImGui::Button(ICON_FA_PLUS" Add Component", ImVec2(-FLT_MIN, 0))) {
			ImGui::OpenPopup("AddComponentPopup");
		}

        if (ImGui::BeginPopup("AddComponentPopup")) {
            if (ImGui::MenuItem("Mesh")) {
				Mesh* mesh = m_SelectedEntity->AddComponent<Mesh>();
				mesh->CreateCube(glm::vec3(1.0f, 1.0f, 1.0f));
			}
            if (ImGui::MenuItem("Light")) {
				Light* light = m_SelectedEntity->AddComponent<Light>();
			}
			ImGui::EndPopup();
        }

        ImGui::EndGroup();
    }
    ImGui::End();

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(1);

    ImGui::Begin("File System", 0, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    for (auto& p : std::filesystem::directory_iterator(m_ProjectPath)) {
        if (p.is_directory()) {
            std::string name = p.path().filename().string();
            if (ImGui::Button(name.c_str(), ImVec2(-FLT_MIN, 0))) {
				std::cout << name << std::endl;
			}
        }
        else {
            std::string name = p.path().filename().string();
            std::string ext = p.path().extension().string();

            if (ext == ".scene") {
                if (ImGui::Button((ICON_FA_CUBES" " + name).c_str(), ImVec2(-FLT_MIN, 0))) {
					LoadScene(p.path().string());
                }
            }
            else if (ext == ".png" || ext == ".jpg") {
				if (ImGui::Button((ICON_FA_IMAGE" " + name).c_str(), ImVec2(-FLT_MIN, 0))) {}
            }
            else {
                ImGui::Text(name.c_str());
            }
        }
	}

    ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	ImGui::End();

    ImGui::Begin("Shadow Map");

    ImGui::Image((void*)lightMapper->shadowMapFBO, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();

    ImGui::End();
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