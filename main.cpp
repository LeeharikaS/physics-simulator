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

// Improved console renderer
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
        
        std::cout << "🎯 BOUNCING BALLS PHYSICS - ALL BALLS VISIBLE 🎯" << std::endl;
        std::cout << "==================================================" << std::endl;
        
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
            // Add height markers
            if (y % 5 == 0) {
                float height = (groundY - y) / 2.0f;
                std::string marker = std::to_string(static_cast<int>(height)) + "m";
                for (size_t i = 0; i < marker.length() && i < 4; i++) {
                    if (1 + i < WIDTH) grid[y][1 + i] = marker[i];
                }
            }
        }
        
        // Draw physics bodies
        std::vector<std::shared_ptr<RigidBody>> dynamicBalls;
        for (const auto& body : world.getBodies()) {
            if (!body->isStatic) {
                dynamicBalls.push_back(body);
                
                // Convert to screen coordinates
                int screenX = static_cast<int>((body->position.x + 8.0f) / 16.0f * WIDTH);
                int screenY = groundY - static_cast<int>(body->position.y * 1.8f);
                
                // Clamp to screen
                screenX = std::max(1, std::min(WIDTH-1, screenX));
                screenY = std::max(0, std::min(HEIGHT-1, screenY));
                
                if (screenY <= groundY) {
                    grid[screenY][screenX] = 'O';
                    
                    // Draw a small line to ground if ball is on ground
                    if (body->position.y <= body->radius + 0.15f) {
                        for (int y = screenY + 1; y <= groundY; y++) {
                            if (y < HEIGHT) grid[y][screenX] = '.';
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
        
        std::cout << "==================================================" << std::endl;
        
        // Show ALL balls information
        std::cout << "ALL " << dynamicBalls.size() << " BALLS:" << std::endl;
        std::cout << "Pos (X,Y)  | Velocity | Bounce | On Ground?" << std::endl;
        std::cout << "-----------|----------|--------|------------" << std::endl;
        
        for (size_t i = 0; i < dynamicBalls.size(); i++) {
            const auto& ball = dynamicBalls[i];
            std::cout << "Ball " << (i+1) << ": (" 
                      << std::fixed << std::setprecision(1) << ball->position.x << "," 
                      << std::setprecision(1) << ball->position.y << ") | "
                      << std::setprecision(1) << ball->velocity.magnitude() << " m/s | "
                      << std::setprecision(2) << ball->restitution << "    | ";
            
            if (ball->position.y <= ball->radius + 0.15f) {
                std::cout << "YES - Settled";
            } else if (ball->velocity.y < 0) {
                std::cout << "No - Falling ↓";
            } else {
                std::cout << "No - Rising ↑";
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    std::cout << "🚀 STARTING ADVANCED BOUNCING PHYSICS DEMO 🚀" << std::endl;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Create physics world
    PhysicsWorld world(Vec3(0, -9.8f, 0));
    
    std::cout << "Creating 8 balls with different properties..." << std::endl;
    
    // Create balls with VERY different properties
    std::vector<std::shared_ptr<RigidBody>> balls;
    
    // Ball 1: Super Bouncy Rubber Ball
    auto ball1 = world.createBody(Vec3(-6.0f, 8.0f, 0), 0.4f, 1.0f);
    ball1->restitution = 0.95f;
    ball1->friction = 0.05f;
    balls.push_back(ball1);
    
    // Ball 2: Tennis Ball
    auto ball2 = world.createBody(Vec3(-4.0f, 7.0f, 0), 0.4f, 1.0f);
    ball2->restitution = 0.75f;
    ball2->friction = 0.1f;
    balls.push_back(ball2);
    
    // Ball 3: Basketball
    auto ball3 = world.createBody(Vec3(-2.0f, 6.0f, 0), 0.5f, 2.0f);
    ball3->restitution = 0.65f;
    ball3->friction = 0.15f;
    balls.push_back(ball3);
    
    // Ball 4: Baseball
    auto ball4 = world.createBody(Vec3(0.0f, 5.0f, 0), 0.3f, 0.8f);
    ball4->restitution = 0.45f;
    ball4->friction = 0.2f;
    balls.push_back(ball4);
    
    // Ball 5: Bowling Ball (less bouncy)
    auto ball5 = world.createBody(Vec3(2.0f, 4.0f, 0), 0.6f, 5.0f);
    ball5->restitution = 0.25f;
    ball5->friction = 0.25f;
    balls.push_back(ball5);
    
    // Ball 6: Ping Pong Ball
    auto ball6 = world.createBody(Vec3(4.0f, 9.0f, 0), 0.2f, 0.3f);
    ball6->restitution = 0.85f;
    ball6->friction = 0.08f;
    balls.push_back(ball6);
    
    // Ball 7: Medicine Ball (very heavy, low bounce)
    auto ball7 = world.createBody(Vec3(6.0f, 3.0f, 0), 0.5f, 8.0f);
    ball7->restitution = 0.15f;
    ball7->friction = 0.3f;
    balls.push_back(ball7);
    
    // Give them different horizontal velocities for more interesting motion
    ball1->velocity.x = 0.5f;
    ball2->velocity.x = -0.3f;
    ball3->velocity.x = 0.2f;
    ball4->velocity.x = -0.4f;
    ball5->velocity.x = 0.1f;
    ball6->velocity.x = 0.6f;
    ball7->velocity.x = -0.2f;
    
    std::cout << "Ball Properties:" << std::endl;
    std::cout << "1: Rubber(95%) 2: Tennis(75%) 3: Basketball(65%)" << std::endl;
    std::cout << "4: Baseball(45%) 5: Bowling(25%) 6: PingPong(85%) 7: Medicine(15%)" << std::endl;
    std::cout << "Starting simulation in 3 seconds..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    // Simulation loop
    int frameCount = 0;
    const int maxFrames = 400;
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (frameCount < maxFrames) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        // Update physics
        world.update(1.0f / 60.0f);
        
        // Render every 2 frames
        if (frameCount % 2 == 0) {
            ConsoleRenderer::render(world);
            std::cout << "Frame: " << frameCount << "/" << maxFrames;
            std::cout << " - Watch the different bounce behaviors!" << std::endl;
            
            // Show which balls are most active
            int activeBalls = 0;
            for (const auto& ball : balls) {
                if (ball->velocity.magnitude() > 0.5f) activeBalls++;
            }
            std::cout << "Active balls (moving > 0.5 m/s): " << activeBalls << "/7" << std::endl;
        }
        
        // Add occasional new balls for variety
        if (frameCount == 150) {
            auto newBall = world.createBody(Vec3(-3.0f, 10.0f, 0), 0.35f, 1.2f);
            newBall->restitution = 0.8f;
            newBall->velocity.x = 0.7f;
            balls.push_back(newBall);
            std::cout << "✨ Added bonus ball! Total: " << balls.size() << " balls" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
        frameCount++;
    }
    
    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "🎯 PHYSICS DEMONSTRATION COMPLETE! 🎯" << std::endl;
    std::cout << "What you observed:" << std::endl;
    std::cout << "• 7+ balls with different physical properties" << std::endl;
    std::cout << "• Rubber ball (95% bounce) - bounced highest/longest" << std::endl;
    std::cout << "• Medicine ball (15% bounce) - barely bounced at all" << std::endl;
    std::cout << "• Different masses affecting motion" << std::endl;
    std::cout << "• Horizontal movement and collisions" << std::endl;
    std::cout << "• Energy dissipation over time" << std::endl;
    
    return 0;
}