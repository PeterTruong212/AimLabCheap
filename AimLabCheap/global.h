#ifndef GLOBALS_H
#define GLOBALS_H

#include <glm/glm.hpp>

// Kích thước cửa sổ
extern const unsigned int SCR_WIDTH;
extern const unsigned int SCR_HEIGHT;

// Thiết lập camera
extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;
extern float yaw, pitch, lastX, lastY;
extern bool firstMouse;

// Thời gian
extern float deltaTime, lastFrame;

// Các cờ điều kiện
extern bool showMenu;
extern bool shouldExit;

extern bool sphereHit;
extern glm::vec3 sphereCenter;
extern float sphereRadius;

extern int score;
extern int shotsFired;

extern bool isDashing;
extern float dashStartTime;

#endif // GLOBALS_H
