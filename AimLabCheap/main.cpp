#include <GL/glew.h>
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

// Dữ liệu mặt đất (một hình vuông nằm ngang)
float groundVertices[] = {
    // Vị trí              // Tọa độ texture
    -100.0f, -1.0f, -100.0f, 0.0f, 0.0f,  // Góc trái dưới
     100.0f, -1.0f, -100.0f, 1.0f, 0.0f,  // Góc phải dưới
     100.0f, -1.0f,  100.0f, 1.0f, 1.0f,  // Góc phải trên
    -100.0f, -1.0f,  100.0f, 0.0f, 1.0f   // Góc trái trên
};


// Chỉ số vẽ mặt đất (hình vuông)
unsigned int groundIndices[] = {
      0, 1, 2,    // Tam giác 1
      0, 2, 3     // Tam giác 2
};

// Dữ liệu crosshair: 4 điểm (2 đường thẳng)
float crosshairVertices[] = {
    // Đường ngang
    -0.02f,  0.0f,  // Trái
     0.02f,  0.0f,  // Phải

     // Đường dọc
      0.0f, -0.02f,  // Dưới
      0.0f,  0.02f   // Trên
};

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

// Vẽ hình cầu
void Sphere::Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 lightPos, glm::vec3 viewPos) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, center); // Dịch chuyển đến vị trí center
    model = glm::scale(model, glm::vec3(radius)); // Scale hình cầu theo size

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // Truyền giá trị ánh sáng vào shader
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewPos));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36 * 18 * 6, GL_UNSIGNED_INT, 0);
}

const int NUM_SPHERES = 10;

// Định nghĩa vùng không gian cố định
const glm::vec3 regionCenter(0.0f, 0.0f, -50.0f); // Tâm của vùng
const float regionWidth = 50.0f;
const float regionHeight = 20.0f;
const float regionDepth = 30.0f;

const float minSize = 0.5f;
const float maxSize = 1.0f;

// Mảng lưu trữ các khối cầu
Sphere spheres[NUM_SPHERES];

// Hàm kiểm tra xem hai hình cầu có chồng chéo lên nhau không
bool checkSphereOverlap(const Sphere& sphere1, const Sphere& sphere2) {
    float distance = glm::distance(sphere1.GetCenter(), sphere2.GetCenter());
    return distance < sphere1.GetRadius() + sphere2.GetRadius(); // Sử dụng GetRadius()
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
    } while (overlap);

    // Thiết lập trạng thái hit cho hình cầu (chưa bị bắn trúng)
    sphere.SetHit(false);
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
                spheres[i].SetHit(true);
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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FPS Camera with Sphere", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback); // Đăng ký callback cho chuột

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Shader cho mặt đất
    GLuint groundShaderProgram = createShaderProgram("ground_vertex_shader.txt", "ground_fragment_shader.txt");
    // Kiểm tra lỗi shader
    GLint success;
    glGetProgramiv(groundShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(groundShaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Tạo VAO, VBO cho mặt đất
    GLuint groundVAO, groundVBO, groundEBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glGenBuffers(1, &groundEBO);

    glBindVertexArray(groundVAO);

    // Tải dữ liệu vào VBO và EBO cho mặt đất
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);

    // Định nghĩa các thuộc tính của dữ liệu mặt đất
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Khởi tạo shader cho crosshair
    GLuint crosshairShaderProgram = createShaderProgram("crosshair_vertex_shader.txt", "crosshair_fragment_shader.txt");
    // Kiểm tra lỗi shader
    glGetProgramiv(crosshairShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(crosshairShaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Tạo VAO, VBO cho crosshair
    GLuint crosshairVAO, crosshairVBO;
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);

    glBindVertexArray(crosshairVAO);

    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Khởi tạo ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Các tham số ánh sáng
    glm::vec3 lightPos(1.0f, 1.0f, 1.0f);  // Vị trí nguồn sáng

    // Tạo và tải texture cho mặt đất
    GLuint groundTexture;
    glGenTextures(1, &groundTexture);
    glBindTexture(GL_TEXTURE_2D, groundTexture);

    // Thiết lập các tham số texture 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Tải ảnh texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("ground_texture.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    // Tạo các khối cầu ban đầu
    for (int i = 0; i < NUM_SPHERES; ++i) {
        spheres[i].Init("sphere_vertex_shader.txt", "sphere_fragment_shader.txt");
        createRandomSphere(spheres[i]);
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

        glUseProgram(groundShaderProgram);

        glm::mat4 groundModel = glm::mat4(1.0f);

        // Lấy đúng vị trí uniform từ groundShaderProgram
        GLuint groundModelLoc = glGetUniformLocation(groundShaderProgram, "model");
        GLuint groundViewLoc = glGetUniformLocation(groundShaderProgram, "view");
        GLuint groundProjLoc = glGetUniformLocation(groundShaderProgram, "projection");

        glUniformMatrix4fv(groundProjLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(groundViewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(groundModelLoc, 1, GL_FALSE, glm::value_ptr(groundModel));

        // Liên kết texture với uniform
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glUniform1i(glGetUniformLocation(groundShaderProgram, "groundTexture"), 0);

        // Vẽ mặt đất
        glBindVertexArray(groundVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // Sử dụng glDrawElements để vẽ với EBO 
        glBindVertexArray(0);

        // Vẽ các khối cầu
        for (int i = 0; i < NUM_SPHERES; ++i) {
            if (!spheres[i].IsHit()) {
                spheres[i].Draw(projection, view, lightPos, cameraPos);
            }
            else {
                createRandomSphere(spheres[i]); // Tạo lại khối cầu mới nếu bị bắn trúng
            }
        }

        // Vẽ crosshair
        glUseProgram(crosshairShaderProgram);

        // Thiết lập ma trận orthographic projection cho crosshair
        glm::mat4 orthoProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
        glUniformMatrix4fv(glGetUniformLocation(crosshairShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(orthoProjection));

        // Thiết lập ma trận view đơn vị cho crosshair
        glm::mat4 identityView = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(crosshairShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(identityView));

        glBindVertexArray(crosshairVAO);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Hiển thị điểm số ở góc dưới bên trái
        ImGui::SetNextWindowPos(ImVec2(10, 10)); // Điều chỉnh vị trí nếu cần
        ImGui::Begin("Score", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Score: %d", score);

        // Tính toán và hiển thị độ chính xác
        float accuracy = shotsFired > 0 ? (float)score / shotsFired * 100 : 0.0f;
        ImGui::Text("Accuracy: %.1f%%", accuracy);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &crosshairVAO);
    glDeleteBuffers(1, &crosshairVBO);
    glDeleteProgram(crosshairShaderProgram);

    glDeleteVertexArrays(1, &groundVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteBuffers(1, &groundEBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}