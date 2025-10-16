cmake_minimum_required(VERSION 3.10)
project(PhysicsSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(PhysicsSimulator
    main.cpp
    Vec3.h
    Mat4.h
    RigidBody.h
    PhysicsWorld.h
)

# Compiler specific settings
if(MSVC)
    target_compile_options(PhysicsSimulator PRIVATE /W4)
else()
    target_compile_options(PhysicsSimulator PRIVATE -Wall -Wextra)
endif()