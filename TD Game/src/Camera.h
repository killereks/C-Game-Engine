//
// Created by killereks on 21/08/2023.
//

#ifndef CPP_GAME_ENGINE_CAMERA_H
#define CPP_GAME_ENGINE_CAMERA_H

#include "Components/Transform.h"

class Camera {
public:
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    Transform m_Transform;
};


#endif //CPP_GAME_ENGINE_CAMERA_H
