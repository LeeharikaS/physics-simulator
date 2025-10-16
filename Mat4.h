#ifndef MAT4_H
#define MAT4_H

#include "Vec3.h"
#include <array>

class Mat4 {
private:
    std::array<std::array<float, 4>, 4> data;

public:
    // Constructor - identity matrix by default
    Mat4() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                data[i][j] = (i == j) ? 1.0f : 0.0f;
            }
        }
    }

    // Access elements
    float& operator()(int row, int col) { return data[row][col]; }
    const float& operator()(int row, int col) const { return data[row][col]; }

    // Matrix multiplication
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result(i, j) = 0;
                for (int k = 0; k < 4; k++) {
                    result(i, j) += data[i][k] * other(k, j);
                }
            }
        }
        return result;
    }

    // Transform point (vector * matrix)
    Vec3 operator*(const Vec3& vec) const {
        float x = data[0][0] * vec.x + data[0][1] * vec.y + data[0][2] * vec.z + data[0][3];
        float y = data[1][0] * vec.x + data[1][1] * vec.y + data[1][2] * vec.z + data[1][3];
        float z = data[2][0] * vec.x + data[2][1] * vec.y + data[2][2] * vec.z + data[2][3];
        float w = data[3][0] * vec.x + data[3][1] * vec.y + data[3][2] * vec.z + data[3][3];
        
        if (w != 0.0f) {
            return Vec3(x/w, y/w, z/w);
        }
        return Vec3(x, y, z);
    }

    // Static methods for common transformations
    static Mat4 translation(const Vec3& translation) {
        Mat4 result;
        result(0, 3) = translation.x;
        result(1, 3) = translation.y;
        result(2, 3) = translation.z;
        return result;
    }

    static Mat4 scale(const Vec3& scale) {
        Mat4 result;
        result(0, 0) = scale.x;
        result(1, 1) = scale.y;
        result(2, 2) = scale.z;
        return result;
    }

    static Mat4 rotationX(float angle) {
        Mat4 result;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        result(1, 1) = cosA; result(1, 2) = -sinA;
        result(2, 1) = sinA; result(2, 2) = cosA;
        return result;
    }

    static Mat4 rotationY(float angle) {
        Mat4 result;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        result(0, 0) = cosA; result(0, 2) = sinA;
        result(2, 0) = -sinA; result(2, 2) = cosA;
        return result;
    }

    static Mat4 rotationZ(float angle) {
        Mat4 result;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        result(0, 0) = cosA; result(0, 1) = -sinA;
        result(1, 0) = sinA; result(1, 1) = cosA;
        return result;
    }

    // Print matrix for debugging
    void print() const {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                std::cout << data[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

#endif