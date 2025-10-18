#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "RigidBody.h"
#include <vector>
#include <memory>

class PhysicsWorld {
private:
    std::vector<std::shared_ptr<RigidBody>> bodies;
    Vec3 gravity;
    int totalCollisions;
    int frameCollisions;
    int previousFrameCollisions;  // ADD THIS MISSING VARIABLE

public:
    PhysicsWorld(const Vec3& gravity = Vec3(0, -9.81f, 0))
        : gravity(gravity), totalCollisions(0), frameCollisions(0), previousFrameCollisions(0) {}

    // Add a body to the simulation
    void addBody(std::shared_ptr<RigidBody> body) {
        bodies.push_back(body);
    }

    // Create and add a body (convenience method)
    std::shared_ptr<RigidBody> createBody(const Vec3& position, float radius = 1.0f, float mass = 1.0f) {
        auto body = std::make_shared<RigidBody>(position, radius, mass);
        addBody(body);
        return body;
    }

    // Update the entire simulation - FIXED VERSION
    void update(float deltaTime) {
        // Store last frame's collisions for display
        previousFrameCollisions = frameCollisions;
        
        // Reset frame collisions for THIS frame's counting
        frameCollisions = 0;
        
        // Apply forces
        for (auto& body : bodies) {
            if (!body->isStatic) {
                body->applyForce(gravity * body->mass);
            }
        }

        // Update bodies
        for (auto& body : bodies) {
            body->update(deltaTime);
        }

        // Check and resolve collisions - COUNT PROPERLY
        for (size_t i = 0; i < bodies.size(); i++) {
            for (size_t j = i + 1; j < bodies.size(); j++) {
                if (bodies[i]->checkSphereCollision(*bodies[j])) {
                    // COUNT THE COLLISION FOR THIS FRAME
                    frameCollisions++;
                    totalCollisions++;
                    
                    bodies[i]->resolveCollision(*bodies[j]);
                }
            }
        }

        // Handle ground collision
        for (auto& body : bodies) {
            if (body->position.y - body->radius < 0) {
                body->position.y = body->radius;
                body->velocity.y = -body->velocity.y * body->restitution;
                body->velocity.x *= (1.0f - body->friction);
                body->velocity.z *= (1.0f - body->friction);
            }
        }
    }

    // Get all bodies for rendering
    const std::vector<std::shared_ptr<RigidBody>>& getBodies() const {
        return bodies;
    }

    // Get methods - RETURN THE RIGHT VALUE
    int getTotalCollisions() const {
        return totalCollisions;
    }
    
    int getFrameCollisions() const {
        return previousFrameCollisions;  // Return last frame's count
    }
    
    // Reset collision counters
    void resetCollisionCount() {
        totalCollisions = 0;
        frameCollisions = 0;
        previousFrameCollisions = 0;
    }

    // Clear all bodies
    void clear() {
        bodies.clear();
        resetCollisionCount();
    }
};

#endif