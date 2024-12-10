#ifndef SPHERE_H
#define SPHERE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "shader_loader.h" 
#include "global.h"

class Sphere {
public:
    ~Sphere();
    void Init(const char* vertexShaderPath, const char* fragmentShaderPath);
    void Draw(glm::mat4 projection, glm::mat4 view, glm::vec3 lightPos, glm::vec3 viewPos);
    glm::vec3 GetCenter() const { return center; }
    void SetCenter(const glm::vec3& newCenter) { center = newCenter; }
    float GetRadius() const { return radius; }
    bool IsHit() const { return hit; }
    void SetHit(bool value) { hit = value; }
    void SetRadius(float newRadius) { radius = newRadius; }

private:

    void createSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, int sectorCount, int stackCount);

    GLuint shaderProgram;
    GLuint VAO, VBO, EBO;
    glm::vec3 center;
    float radius;
    bool hit;
};

// Hàm kiểm tra va chạm tia-hình cầu
bool raySphereIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& sphereCenter, float sphereRadius);
// Hàm kiểm tra xem hai hình cầu có chồng chéo lên nhau không
bool checkSphereOverlap(const Sphere& sphere1, const Sphere& sphere2);
// Hàm tạo khối cầu ngẫu nhiên
void createRandomSphere(Sphere& sphere);

#endif