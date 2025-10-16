#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Vec3.h"
#include "Mat4.h"
#include <memory>

class RigidBody {
public:
    // Physical properties
    Vec3 position;
    Vec3 velocity;
    Vec3 acceleration;
    Vec3 force;
    
    // Rotation
    Vec3 rotation;
    Vec3 angularVelocity;
    
    // Physical properties
    float mass;
    float restitution; // Bounciness
    float friction;
    
    // Shape properties (sphere for simplicity)
    float radius;
    
    // State
    bool isStatic;

    RigidBody(const Vec3& pos = Vec3(), float radius = 1.0f, float mass = 1.0f)
        : position(pos), velocity(Vec3::zero()), acceleration(Vec3::zero()), 
          force(Vec3::zero()), rotation(Vec3::zero()), angularVelocity(Vec3::zero()),
          mass(mass), restitution(0.8f), friction(0.1f), radius(radius), isStatic(false) {}

    // Apply force to the body
    void applyForce(const Vec3& f) {
        if (!isStatic) {
            force += f;
        }
    }

    // Apply impulse (instant change in velocity)
    void applyImpulse(const Vec3& impulse) {
        if (!isStatic) {
            velocity += impulse / mass;
        }
    }

    // Update physics (Euler integration) - WITH GROUND COLLISION
    void update(float deltaTime) {
        if (isStatic) return;

        // Linear motion
        acceleration = force / mass;
        velocity += acceleration * deltaTime;
        position += velocity * deltaTime;

        // Angular motion (simple rotation)
        rotation += angularVelocity * deltaTime;

        // Reset force
        force = Vec3::zero();

        // Apply damping
        velocity = velocity * 0.99f;
        angularVelocity = angularVelocity * 0.95f;

        // === GROUND COLLISION DETECTION ===
        // Ground is at y = 0, so check if we hit it
        if (position.y - radius < 0) {
            // Push above ground
            position.y = radius;
            
            // Bounce with restitution (only when moving downward)
            if (velocity.y < 0) {
                velocity.y = -velocity.y * restitution;
            }
            
            // Apply friction to horizontal movement
            velocity.x *= (1.0f - friction);
            velocity.z *= (1.0f - friction);
        }
    }

    // Get transformation matrix for rendering
    Mat4 getTransformation() const {
        Mat4 translation = Mat4::translation(position);
        Mat4 rotationX = Mat4::rotationX(rotation.x);
        Mat4 rotationY = Mat4::rotationY(rotation.y);
        Mat4 rotationZ = Mat4::rotationZ(rotation.z);
        Mat4 scale = Mat4::scale(Vec3(radius, radius, radius));
        
        return translation * rotationZ * rotationY * rotationX * scale;
    }

    // Check collision with another sphere
    bool checkSphereCollision(const RigidBody& other) const {
        float distance = position.distance(other.position);
        return distance < (radius + other.radius);
    }

    // Get collision normal with another sphere
    Vec3 getCollisionNormal(const RigidBody& other) const {
        return (other.position - position).normalized();
    }

    // Resolve collision with another sphere
    void resolveCollision(RigidBody& other) {
        if (isStatic && other.isStatic) return;

        Vec3 collisionNormal = getCollisionNormal(other);
        Vec3 relativeVelocity = other.velocity - velocity;

        float velocityAlongNormal = relativeVelocity.dot(collisionNormal);

        // Don't resolve if objects are moving apart
        if (velocityAlongNormal > 0) return;

        // Calculate impulse scalar
        float restitution = std::min(this->restitution, other.restitution);
        float j = -(1 + restitution) * velocityAlongNormal;
        
        if (!isStatic && !other.isStatic) {
            j /= (1/mass + 1/other.mass);
        } else if (isStatic) {
            j /= (1/other.mass);
        } else {
            j /= (1/mass);
        }

        // Apply impulse
        Vec3 impulse = collisionNormal * j;
        
        if (!isStatic) applyImpulse(impulse);
        if (!other.isStatic) other.applyImpulse(impulse * -1);

        // Position correction to prevent sinking
        float penetration = (radius + other.radius) - position.distance(other.position);
        if (penetration > 0) {
            Vec3 correction = collisionNormal * (penetration / (isStatic || other.isStatic ? 1.0f : 2.0f));
            if (!isStatic) position -= correction * 0.5f;
            if (!other.isStatic) other.position += correction * 0.5f;
        }
    }
};

#endif