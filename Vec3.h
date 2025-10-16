#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

class Vec3 {
public:
    float x, y, z;

    // Constructors
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Basic arithmetic operations
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3 operator/(float scalar) const {
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    // Compound assignment operators
    Vec3& operator+=(const Vec3& other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }

    // Dot product
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Magnitude and normalization
    float magnitude() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    float magnitudeSquared() const {
        return x*x + y*y + z*z;
    }

    Vec3 normalized() const {
        float mag = magnitude();
        if (mag > 0) return *this / mag;
        return *this;
    }

    // Distance between two points
    float distance(const Vec3& other) const {
        return (*this - other).magnitude();
    }

    // Utility functions
    void print() const {
        std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
    }

    // Common vectors
    static Vec3 zero() { return Vec3(0, 0, 0); }
    static Vec3 up() { return Vec3(0, 1, 0); }
    static Vec3 right() { return Vec3(1, 0, 0); }
    static Vec3 forward() { return Vec3(0, 0, 1); }
};

#endif