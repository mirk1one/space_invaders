#pragma once

// A class for storing vetices
//  Vertex
class Vertex {
public:

    float x, y, z;     // 3d coordinate of the vertex
    float Nx, Ny, Nz;     // Normal to the surface (if present)
    float u, v;						// texture coordinates
    float r, g, b;				// Color (0.0 -  1.0)

    Vertex() {}
    Vertex(float x, float y, float z) : x(x), y(y), z(z) { }
    Vertex(float x, float y, float z, float u, float v) : x(x), y(y), z(z),
        u(u), v(v) { }
    inline void SetP(float x, float y, float z) {
        this->x = x; this->y = y;
        this->z = z;
    }
    inline void SetN(float x, float y, float z) { Nx = x; Ny = y; Nz = z; }
};