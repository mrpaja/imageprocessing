#include "Rectangle.h"
#include "Constants.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

Rectangle::Rectangle(const Eigen::Vector2f& p1, const float width, const float height)
    : m_p1(p1)
    , m_width(width)
    , m_height(height)
{
    createRectangle();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    // unbind
    glBindVertexArray(0);
}

void Rectangle::createRectangle(void)
{
    // point 1;
    Eigen::Vector2f xDir{1.0f, 0.0f};
    Eigen::Vector2f yDir{0.0f, 1.0f};
    vertices.block(0, 0, 1, 2) = (m_p1).transpose();
    // point 2
    vertices.block(1, 0, 1, 2) = (m_p1 + m_width * xDir).transpose();

    // point3
    vertices.block(2, 0, 1, 2) = (m_p1 + m_width * xDir + m_height * yDir).transpose();
    // point4
    vertices.block(3, 0, 1, 2) = (m_p1 + yDir * m_height).transpose();

    std::cout << vertices(0, 0) << ", " << vertices(0, 1) << "\n";
    std::cout << vertices(1, 0) << ", " << vertices(1, 1) << "\n";
    std::cout << vertices(2, 0) << ", " << vertices(2, 1) << "\n";
    std::cout << vertices(3, 0) << ", " << vertices(3, 1) << "\n";
    indices = {0, 1, 2, 0, 2, 3};
}

void Rectangle::Draw(void) const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}
