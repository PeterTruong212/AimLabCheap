﻿#include "input.h"

// Callback chuột cho di chuyển camera
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    if (showMenu) {
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Xử lý đầu vào từ bàn phím
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        showMenu = true;  // Hiển thị menu khi bấm ESC
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Hiển thị con trỏ chuột khi ESC
    }

    float cameraSpeed = 2.5f * deltaTime;

    // Cách 2: Chiếu cameraFront lên mặt phẳng XZ
    glm::vec3 frontXZ = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * frontXZ;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * frontXZ;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(frontXZ, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(frontXZ, cameraUp)) * cameraSpeed;

    if (shouldExit) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        isDashing = true;
        dashStartTime = glfwGetTime(); // Sử dụng biến float dashStartTime để lưu thời điểm
    }

    if (isDashing) {
        float dashDuration = 0.02f; // Khoảng thời gian lướt (giây)
        float dashDistance = 3.0f; // Khoảng cách lướt
        glm::vec3 dashDirection = glm::vec3(0.0f);

        // Xác định hướng lướt dựa trên phím WASD
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            dashDirection += cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            dashDirection -= cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            dashDirection -= glm::normalize(glm::cross(cameraFront, cameraUp));
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            dashDirection += glm::normalize(glm::cross(cameraFront, cameraUp));
        }

        // Nếu không nhấn phím WASD, lướt theo hướng camera
        if (dashDirection == glm::vec3(0.0f)) {
            dashDirection = cameraFront;
        }

        // Loại bỏ thành phần y và chuẩn hóa vector
        dashDirection = glm::normalize(glm::vec3(dashDirection.x, 0.0f, dashDirection.z));

        float elapsedTime = glfwGetTime() - dashStartTime;
        if (elapsedTime < dashDuration) {
            cameraPos += dashDirection * dashDistance * deltaTime / dashDuration;
        }
        else {
            isDashing = false;
        }
    }
}