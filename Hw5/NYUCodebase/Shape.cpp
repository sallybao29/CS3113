#include "Shape.hpp"

Shape::Shape(float width, float height) : size(width, height, 0.0f) {}

const std::vector<Vector3>& Shape::points() const {
    return vertices;
}

const std::vector<float> Shape::triangles() const {
    return triangleVertices;
}

void Shape::SetSize(float width, float height) {
    size.x = width;
    size.y = height;
}

/*----------------------------------- Rectangle --------------------------------------*/
Rectangle::Rectangle(float width, float height) : Shape(width, height) {
    // Set vertices of each corner
    vertices.emplace_back(-0.5f * size.x, 0.5f * size.y, 0.0f);
    vertices.emplace_back(-0.5f * size.x, -0.5f * size.y, 0.0f);
    vertices.emplace_back(0.5f * size.x, -0.5f * size.y, 0.0f);
    vertices.emplace_back(0.5f * size.x, 0.5f * size.y, 0.0f);
    
    // Set vertices of each triangle
    triangleVertices.insert(triangleVertices.end(), {
        vertices[1].x, vertices[1].y,
        vertices[3].x, vertices[3].y,
        vertices[0].x, vertices[0].y,
        vertices[3].x, vertices[3].y,
        vertices[1].x, vertices[1].y,
        vertices[2].x, vertices[2].y
    });
}

void Rectangle::SetSize(float width, float height) {
    Shape::SetSize(width, height);
    
    vertices[0].x = -0.5 * size.x;
    vertices[0].y = 0.5 * size.y;

    vertices[1].x = -0.5 * size.x;
    vertices[1].y = -0.5 * size.y;
    
    vertices[2].x = 0.5 * size.x;
    vertices[2].y = -0.5 * size.y;
    
    vertices[2].x = 0.5 * size.x;
    vertices[2].y = 0.5 * size.y;
    
    triangleVertices[0] = vertices[1].x;
    triangleVertices[1] = vertices[1].y;
    triangleVertices[2] = vertices[3].x;
    triangleVertices[3] = vertices[3].y;
    triangleVertices[4] = vertices[0].x;
    triangleVertices[5] =  vertices[0].y;
    triangleVertices[6] = vertices[3].x;
    triangleVertices[7] = vertices[3].y;
    triangleVertices[8] = vertices[1].x;
    triangleVertices[9] = vertices[1].y;
    triangleVertices[10] = vertices[2].x;
    triangleVertices[11] = vertices[2].y;
}

/*----------------------------------- Triangle --------------------------------------*/
Triangle::Triangle(float width, float height) : Shape(width, height) {
    // Set vertices of each corner
    vertices.emplace_back(0.0, 0.5f * size.y, 0.0f);
    vertices.emplace_back(-0.5f * size.x, -0.5f * size.y, 0.0f);
    vertices.emplace_back(0.5f * size.x, -0.5f * size.y, 0.0f);
    
    // Set vertices of each triangle
    triangleVertices.insert(triangleVertices.end(), {
        vertices[0].x, vertices[0].y,
        vertices[1].x, vertices[1].y,
        vertices[2].x, vertices[2].y,
    });
}

void Triangle::SetSize(float width, float height) {
    Shape::SetSize(width, height);
    
    vertices[0].y = 0.5f * size.y;
    
    vertices[1].x = -0.5 * size.x;
    vertices[1].y = -0.5 * size.y;
    
    vertices[2].x = 0.5 * size.x;
    vertices[2].y = -0.5 * size.y;
    
    triangleVertices[0] = vertices[0].y;
    triangleVertices[1] = vertices[1].x;
    triangleVertices[2] = vertices[1].y;
    triangleVertices[3] = vertices[2].x;
    triangleVertices[4] = vertices[2].y;
}


