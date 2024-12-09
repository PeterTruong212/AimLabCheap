#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "global.h"

// Callback chuột cho di chuyển camera
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Xử lý đầu vào từ bàn phím
void processInput(GLFWwindow* window);

#endif
