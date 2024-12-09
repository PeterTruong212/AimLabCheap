#include "global.h"

const unsigned int SCR_WIDTH = 2560;
const unsigned int SCR_HEIGHT = 1440;

// Thiết lập camera
glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
float yaw = -90.0f, pitch = 0.0f, lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Thời gian
float deltaTime = 0.0f, lastFrame = 0.0f;

// Các cờ điều kiện
bool showMenu = false;
bool shouldExit = false;

bool sphereHit = false;
glm::vec3 sphereCenter(0.0f, 1.0f, 0.0f);
float sphereRadius = 1.0f;

int score = 0;
int shotsFired = 0;