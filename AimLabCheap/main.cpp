﻿#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include "shader_loader.h" 

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "global.h"
#include "sphere.h"
#include "input.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 
#include "crosshair.h"
#include "ground.h"
#include "jack.h"
#include "obstacle.h"

// Hàm hiển thị menu
void drawMenu() {
    // Vẽ menu sử dụng ImGui
    ImGui::Begin("Menu");

    ImGui::Text("Press ESC to continue or exit.");
    if (ImGui::Button("Continue")) {
        showMenu = false;
        glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Ẩn chuột khi tiếp tục
    }
    if (ImGui::Button("Exit")) {
        shouldExit = true;
    }

    ImGui::End();
}

const int NUM_SPHERES = 5;
const int NUM_OBSTACLES = 10;

// Định nghĩa vùng không gian cố định
const glm::vec3 regionCenter(0.0f, 0.0f, -50.0f); // Tâm của vùng
const float regionWidth = 50.0f;
const float regionHeight = 20.0f;
const float regionDepth = 30.0f;

const float minSize = 0.5f;
const float maxSize = 1.0f;

// Mảng lưu trữ các khối cầu
Sphere spheres[NUM_SPHERES];
// Vector lưu trữ các khối hộp
std::vector<Obstacle> obstacles;

bool checkSphereObstacleOverlap(const Sphere& sphere, const Obstacle& obstacle) {
    // Tìm điểm gần nhất trên khối hộp đến tâm hình cầu
    glm::vec3 closestPoint = glm::max(obstacle.position - obstacle.size / 2.0f, glm::min(sphere.GetCenter(), obstacle.position + obstacle.size / 2.0f));

    // Tính khoảng cách từ tâm hình cầu đến điểm gần nhất
    float distance = glm::distance(sphere.GetCenter(), closestPoint);

    return distance < sphere.GetRadius();
}

// Hàm kiểm tra xem hai hình cầu có chồng chéo lên nhau không
bool checkSphereOverlap(const Sphere& sphere1, const Sphere& sphere2) {
    float distance = glm::distance(sphere1.GetCenter(), sphere2.GetCenter());
    return distance < sphere1.GetRadius() + sphere2.GetRadius(); 
}

// Hàm kiểm tra va chạm giữa hai khối hộp (bạn cần tự viết)
bool checkObstacleOverlap(const Obstacle& obstacle1, const Obstacle& obstacle2) {
    // Kiểm tra xem các khối hộp có giao nhau trên mỗi trục tọa độ
    bool overlapX = std::abs(obstacle1.position.x - obstacle2.position.x) < (obstacle1.size.x + obstacle2.size.x) / 2.0f;
    bool overlapY = std::abs(obstacle1.position.y - obstacle2.position.y) < (obstacle1.size.y + obstacle2.size.y) / 2.0f;
    bool overlapZ = std::abs(obstacle1.position.z - obstacle2.position.z) < (obstacle1.size.z + obstacle2.size.z) / 2.0f;

    // Nếu các khối hộp giao nhau trên cả ba trục tọa độ, chúng chồng lên nhau
    return overlapX && overlapY && overlapZ;
}

// Hàm tạo khối cầu ngẫu nhiên trong vùng cố định
void createRandomSphere(Sphere& sphere) {
    float x, y, z, radius;

    // Kiểm tra trùng lặp với các khối cầu khác
    bool overlap;
    do {
        overlap = false;

        // Sinh tọa độ ngẫu nhiên trong vùng cố định
        x = regionCenter.x + (rand() / (float)RAND_MAX - 0.5f) * regionWidth;
        y = regionCenter.y + (rand() / (float)RAND_MAX - 0.5f) * regionHeight + 10.0f;
        z = regionCenter.z + (rand() / (float)RAND_MAX - 0.5f) * regionDepth;

        // Sinh bán kính ngẫu nhiên
        radius = minSize + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxSize - minSize)));

        // Thiết lập vị trí và bán kính cho hình cầu
        sphere.SetCenter(glm::vec3(x, y, z));
        sphere.SetRadius(radius);

        // Kiểm tra va chạm với các hình cầu khác
        for (int i = 0; i < NUM_SPHERES; ++i) {
            if ((&sphere - spheres) != i && checkSphereOverlap(sphere, spheres[i])) {
                overlap = true;
                break;
            }
        }

        for (const Obstacle& obstacle : obstacles) {
            if (checkSphereObstacleOverlap(sphere, obstacle)) {
                overlap = true;
                break;
            }
        }
    } while (overlap);

    // Thiết lập trạng thái hit cho hình cầu (chưa bị bắn trúng)
    sphere.SetHit(false);
}

void createRandomObstacle(Obstacle& obstacle, int i) {
    glm::vec3 size(20.0f, 10.0f, 1.0f); // Kích thước khối hộp
    glm::vec3 color(0.0f, 0.0f, 1.0f); // Màu xanh dương

    // Tính toán vị trí cho từng khối hộp
    switch (i) {
    case 0: // Phía trước, góc trái
        obstacle.position = glm::vec3(-20.0f, 5.0f, -35.0f);
        obstacle.size = size;
        break;
    case 1: // Phía trước, góc phải
        obstacle.position = glm::vec3(10.0f, 15.0f, -35.0f);
        obstacle.size = size;
        break;
    case 2: // Phía sau, góc trái
        obstacle.position = glm::vec3(-20.0f, 5.0f, -66.0f);
        obstacle.size = size;
        break;
    case 3: // Phía sau, góc phải
        obstacle.position = glm::vec3(10.0f, 15.0f, -66.0f);
        obstacle.size = size;
        break;
    case 4: // Bên trái, giữa
        obstacle.position = glm::vec3(-26.0f, 10.0f, -50.0f);
        obstacle.size = glm::vec3(1.0f, size.y, 20.0f); 
        break;
    case 5: // Bên phải, giữa
        obstacle.position = glm::vec3(26.0f, 10.0f, -50.0f);
        obstacle.size = glm::vec3(1.0f, size.y, 20.0f); 
        break;
        // Thêm các case khác nếu bạn muốn tạo thêm khối hộp
    }

    obstacle.color = color;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        shotsFired++; // Tăng số lần bắn lên 1

        // Tạo tia (ray)
        glm::vec3 rayStart = cameraPos; // Bắt đầu từ vị trí camera
        glm::vec3 rayDir = cameraFront; // Hướng nhìn của camera

        // Kiểm tra va chạm với tất cả khối cầu
        for (int i = 0; i < NUM_SPHERES; ++i) {
            if (!spheres[i].IsHit() && raySphereIntersection(rayStart, rayDir, spheres[i].GetCenter(), spheres[i].GetRadius())) {
                score++; // Tăng điểm số lên 1
                createRandomSphere(spheres[i]);

                // Thoát khỏi vòng lặp nếu đã bắn trúng một hình cầu
                break;
            }
        }
    }
}

int main() {
    // Khởi tạo GLFW và cấu hình cửa sổ
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // khởi tạo cửa sổ 
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Aimlab", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback); // Đăng ký callback cho di chuyển chuột
    glfwSetMouseButtonCallback(window, mouse_button_callback); // Đăng ký callback cho bấm chuột
    
    // Khởi tạo GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Bật kiểm tra độ sâu. Depth Test đảm bảo rằng các fragment nằm xa hơn sẽ không ghi đè lên các fragment gần hơn.
    glEnable(GL_DEPTH_TEST); 

    Jack jack;
    jack.Init("jack_vertex.txt", "jack_fragment.txt");
   
    Ground ground;
    ground.Init("ground_vertex_shader.txt", "ground_fragment_shader.txt"); 

    Crosshair crosshair;
    crosshair.Init("crosshair_vertex_shader.txt", "crosshair_fragment_shader.txt"); 

    // Khởi tạo ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Các tham số ánh sáng
    glm::vec3 lightPos(1.0f, 1.0f, 1.0f);  // Vị trí nguồn sáng

    // Tạo các khối cầu ban đầu
    for (int i = 0; i < NUM_SPHERES; ++i) {
        spheres[i].Init("sphere_vertex_shader.txt", "sphere_fragment_shader.txt");
        createRandomSphere(spheres[i]);
    }

    obstacles.resize(6); // Đảm bảo kích thước của vector obstacles là 6
    for (int i = 0; i < obstacles.size(); ++i) {
        obstacles[i].Init("obstacle_vertex_shader.txt", "obstacle_fragment_shader.txt");
        createRandomObstacle(obstacles[i], i); // Gọi hàm với tham số i
    }

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Hiển thị menu nếu cần
        if (showMenu) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            drawMenu();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        // Truyền ma trận chiếu, nhìn và mô hình vào shader
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        ground.Draw(projection, view, ground.texture);  
        jack.Draw(projection, view, jack.texture);

        // Vẽ các khối cầu
        for (int i = 0; i < NUM_SPHERES; ++i) {
            if (!spheres[i].IsHit()) {
                spheres[i].Draw(projection, view, lightPos, cameraPos);
            }
            else {
                createRandomSphere(spheres[i]); // Tạo lại khối cầu mới nếu bị bắn trúng
            }
        }

        for (Obstacle& obstacle : obstacles) { 
            obstacle.Draw(projection, view, lightPos, cameraPos); 
        } 

        // Vẽ crosshair
        crosshair.Draw();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Hiển thị điểm số
        ImGui::SetNextWindowPos(ImVec2(1200, 10)); // Điều chỉnh vị trí
        ImGui::Begin("Score", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Score: %d", score);

        float accuracy = shotsFired > 0 ? (float)score / shotsFired * 100 : 0.0f;
        ImGui::Text("Accuracy: %.1f%%", accuracy);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}