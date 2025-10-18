#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>
#include <memory>

// Include our headers
#include "Vec3.h"
#include "Mat4.h" 
#include "RigidBody.h"
#include "PhysicsWorld.h"

// Enhanced console renderer with both collision detection AND detailed ball info
class ConsoleRenderer {
private:
    static const int WIDTH = 70;
    static const int HEIGHT = 25;
    
public:
    static void clearScreen() {
        std::cout << "\033[2J\033[1;1H";
    }
    
    static void render(const PhysicsWorld& world) {
        clearScreen();
        
        std::cout << "=== PHYSICS SIMULATOR - COLLISION DETECTION & BALL TRACKING ===" << std::endl;
        std::cout << "=============================================================" << std::endl;
        
        // Create display grid
        char grid[HEIGHT][WIDTH];
        
        // Initialize grid
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                grid[y][x] = ' ';
            }
        }
        
        // Draw ground
        int groundY = HEIGHT - 3;
        for (int x = 0; x < WIDTH; x++) {
            grid[groundY][x] = '=';
        }
        
        // Draw left border with scale
        for (int y = 0; y < HEIGHT; y++) {
            grid[y][0] = '|';
            // Add height markers every 5 rows
            if (y % 5 == 0 && y != groundY) {
                float height = (groundY - y) / 2.0f;
                std::string marker = std::to_string(static_cast<int>(height)) + "m";
                for (size_t i = 0; i < marker.length() && i < 4; i++) {
                    if (1 + i < WIDTH) grid[y][1 + i] = marker[i];
                }
            }
        }
        
        // Track currently colliding balls for visualization
        std::vector<std::pair<int, int>> currentCollisions;
        auto bodies = world.getBodies();
        std::vector<bool> ballColliding(bodies.size(), false);
        
        // Draw physics bodies
        std::vector<std::shared_ptr<RigidBody>> dynamicBalls;
        for (size_t i = 0; i < bodies.size(); i++) {
            const auto& body = bodies[i];
            if (!body->isStatic) {
                dynamicBalls.push_back(body);
                
                // Convert to screen coordinates
                int screenX = static_cast<int>((body->position.x + 6.0f) / 12.0f * WIDTH);
                int screenY = groundY - static_cast<int>(body->position.y * 1.8f);
                
                // Clamp to screen
                screenX = std::max(1, std::min(WIDTH-2, screenX));
                screenY = std::max(0, std::min(HEIGHT-2, screenY));
                
                if (screenY <= groundY) {
                    // Check if this ball is colliding with any other ball
                    bool isColliding = false;
                    for (size_t j = 0; j < bodies.size(); j++) {
                        if (i != j && !bodies[j]->isStatic) {
                            if (body->checkSphereCollision(*bodies[j])) {
                                isColliding = true;
                                ballColliding[i] = true;
                                // Store collision pair (avoid duplicates)
                                if (i < j) {
                                    currentCollisions.push_back({i, j});
                                }
                            }
                        }
                    }
                    
                    // Use different character for colliding balls (ASCII only)
                    grid[screenY][screenX] = isColliding ? '@' : 'O';
                    
                    // Draw connection line for colliding balls (visualization)
                    if (isColliding) {
                        for (size_t j = 0; j < bodies.size(); j++) {
                            if (i != j && body->checkSphereCollision(*bodies[j])) {
                                int otherX = static_cast<int>((bodies[j]->position.x + 6.0f) / 12.0f * WIDTH);
                                otherX = std::max(1, std::min(WIDTH-2, otherX));
                                
                                // Draw a simple line between colliding balls
                                int startX = std::min(screenX, otherX);
                                int endX = std::max(screenX, otherX);
                                for (int x = startX; x <= endX; x++) {
                                    if (grid[screenY][x] == ' ') {
                                        grid[screenY][x] = '-';
                                    }
                                }
                            }
                        }
                    }
                    
                    // Draw a small line to ground if ball is on ground
                    if (body->position.y <= body->radius + 0.15f) {
                        for (int y = screenY + 1; y <= groundY; y++) {
                            if (y < HEIGHT && grid[y][screenX] == ' ') {
                                grid[y][screenX] = '.';
                            }
                        }
                    }
                }
            }
        }
        
        // Print the grid
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                std::cout << grid[y][x];
            }
            std::cout << std::endl;
        }
        
        std::cout << "=============================================================" << std::endl;
        
        // COLLISION STATISTICS DISPLAY
        std::cout << "*** COLLISION STATISTICS ***" << std::endl;
        std::cout << "Frame Collisions: " << world.getFrameCollisions() << std::endl;
        std::cout << "Total Collisions: " << world.getTotalCollisions() << std::endl;
        
        // Show active collisions
        if (world.getFrameCollisions() > 0) {
            std::cout << "Active Collisions: ";
            for (const auto& collision : currentCollisions) {
                std::cout << "Ball" << collision.first << "<->Ball" << collision.second << " ";
            }
            std::cout << std::endl;
        }
        
        std::cout << "=============================================================" << std::endl;
        
        // DETAILED BALL-BY-BALL INFORMATION
        std::cout << "*** DETAILED BALL INFORMATION ***" << std::endl;
        std::cout << "Ball | Position (X,Y) | Velocity | Bounce | Status" << std::endl;
        std::cout << "-----|----------------|----------|--------|------------" << std::endl;
        
        int ballNumber = 0;
        for (size_t i = 0; i < bodies.size(); i++) {
            if (!bodies[i]->isStatic) {
                ballNumber++;
                std::cout << std::setw(2) << ballNumber << "   | (" 
                          << std::fixed << std::setprecision(1) << bodies[i]->position.x << "," 
                          << std::setprecision(1) << bodies[i]->position.y << ")   | "
                          << std::setprecision(1) << bodies[i]->velocity.magnitude() << " m/s | "
                          << std::setprecision(2) << bodies[i]->restitution << "    | ";
                
                // Status information
                if (ballColliding[i]) {
                    std::cout << "COLLIDING";
                } else if (bodies[i]->position.y <= bodies[i]->radius + 0.15f) {
                    std::cout << "ON GROUND";
                } else if (bodies[i]->velocity.y < 0) {
                    std::cout << "FALLING";
                } else if (bodies[i]->velocity.y > 0) {
                    std::cout << "RISING";
                } else {
                    std::cout << "RESTING";
                }
                std::cout << std::endl;
            }
        }
    }
};

int main() {
    std::cout << "*** STARTING ADVANCED PHYSICS SIMULATOR ***" << std::endl;
    std::cout << "Combining collision detection with detailed ball tracking..." << std::endl;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Create physics world
    PhysicsWorld world(Vec3(0, -9.8f, 0));
    
    std::cout << "Creating balls with different properties..." << std::endl;
    
    // Create balls with VERY different properties
    std::vector<std::shared_ptr<RigidBody>> balls;
    
    // Ball 1: High Bounce
    auto ball1 = world.createBody(Vec3(-4.0f, 5.0f, 0), 0.4f, 1.0f);
    ball1->restitution = 0.9f;
    ball1->friction = 0.1f;
    balls.push_back(ball1);
    
    // Ball 2: Medium Bounce
    auto ball2 = world.createBody(Vec3(-2.0f, 6.0f, 0), 0.4f, 1.0f);
    ball2->restitution = 0.6f;
    ball2->friction = 0.2f;
    balls.push_back(ball2);
    
    // Ball 3: Low Bounce
    auto ball3 = world.createBody(Vec3(0.0f, 4.0f, 0), 0.4f, 1.0f);
    ball3->restitution = 0.3f;
    ball3->friction = 0.3f;
    balls.push_back(ball3);
    
    // Ball 4: Heavy Ball
    auto ball4 = world.createBody(Vec3(2.0f, 7.0f, 0), 0.5f, 3.0f);
    ball4->restitution = 0.5f;
    ball4->friction = 0.2f;
    balls.push_back(ball4);
    
    // Ball 5: Light Ball
    auto ball5 = world.createBody(Vec3(4.0f, 5.0f, 0), 0.3f, 0.5f);
    ball5->restitution = 0.8f;
    ball5->friction = 0.1f;
    balls.push_back(ball5);
    
    // Give them different horizontal velocities for more interesting motion
    ball1->velocity.x = 0.3f;
    ball2->velocity.x = -0.2f;
    ball3->velocity.x = 0.4f;
    ball4->velocity.x = -0.1f;
    ball5->velocity.x = 0.5f;
    
    std::cout << "Ball Properties:" << std::endl;
    std::cout << "1: High Bounce(90%) 2: Medium(60%) 3: Low(30%) 4: Heavy 5: Light" << std::endl;
    std::cout << "Starting simulation in 3 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    // Simulation loop
    int frameCount = 0;
    const int maxFrames = 500;
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (frameCount < maxFrames) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Update physics
        world.update(1.0f / 60.0f);
        
        // Render every 3 frames for better visualization
        if (frameCount % 3 == 0) {
            ConsoleRenderer::render(world);
            std::cout << "Frame: " << frameCount << "/" << maxFrames;
            std::cout << " - Watch collisions & ball details!" << std::endl;
            
            // Show which balls are most active
            int activeBalls = 0;
            for (const auto& ball : balls) {
                if (ball->velocity.magnitude() > 0.5f) activeBalls++;
            }
            std::cout << "Active balls (moving > 0.5 m/s): " << activeBalls << "/" << balls.size() << std::endl;
        }
        
        // Add occasional new balls for variety
        if (frameCount == 150 || frameCount == 300) {
            float x = (std::rand() % 60) / 10.0f - 3.0f;
            auto newBall = world.createBody(Vec3(x, 10.0f, 0), 0.35f, 1.2f);
            newBall->restitution = 0.7f;
            newBall->velocity.x = (std::rand() % 100) / 50.0f - 1.0f;
            balls.push_back(newBall);
            std::cout << "*** New ball added! Total: " << balls.size() << " balls ***" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        frameCount++;
    }
    
    std::cout << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << "*** SIMULATION COMPLETE! ***" << std::endl;
    std::cout << "FINAL STATISTICS:" << std::endl;
    std::cout << "Total collisions detected: " << world.getTotalCollisions() << std::endl;
    std::cout << "Average collisions per frame: " << (float)world.getTotalCollisions() / maxFrames << std::endl;
    std::cout << "Different physics behaviors demonstrated:" << std::endl;
    std::cout << "- High bounce ball (90%) vs Low bounce ball (30%)" << std::endl;
    std::cout << "- Heavy vs Light ball mass effects" << std::endl;
    std::cout << "- Collision detection and response" << std::endl;
    std::cout << "- Real-time physics simulation" << std::endl;
    
    return 0;
}