#ifndef OPENLGL_3D_DATA_OBJECT_H
#define OPENLGL_3D_DATA_OBJECT_H

#include "Color.h"
#include "Eigen/Dense"
#include <GL/glew.h>
#include <vector>

class OpenGL3DDataObject
{
public:
    OpenGL3DDataObject() { glGenVertexArrays(1, &VAO); }
    ~OpenGL3DDataObject(void);
    void InitializeVertexBuffer(const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>& vertices,
                                std::vector<int>                                                indices);

    void InitializeColorBuffer(const Color& color);
    void InitializeNormalBuffer(const Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor>& normals);
    void InitializeTextureUVBuffer(const Eigen::Matrix<float, Eigen::Dynamic, 2, Eigen::RowMajor>& uvCoordinates);
    void DrawObject(GLenum mode) const;

private:
    // Vertex Array Object
    unsigned int VAO = 0;
    // Vertex Element Object (EBO)
    unsigned int indexBuffer = 0;

    // Vertex Buffer
    unsigned int vertexBuffer  = 0;
    unsigned int vertexAttrIdx = 0;

    // Color Buffer
    unsigned int colorBuffer  = 0;
    unsigned int colorAttrIdx = 1;

    // Texture Buffer
    unsigned int textureBuffer  = 0;
    unsigned int textureAttrIdx = 2;

    // Normal Buffer
    unsigned int normalsBuffer = 0;
    unsigned int normalAttrIdx = 3;

    unsigned int numberOfIndices  = 0;
    unsigned int numberOfVertices = 0;

    std::vector<unsigned int> buffersInUseVector;
};

#endif