/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>
#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <include/shader_s.h>
//#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
//#include "shader.h"
#include "texture.h"
//#include "game_object.h"


// Represents a single particle and its state
struct Particle {
    glm::vec3 Position, Velocity;
    glm::vec4 Color;
    GLfloat Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // Constructor
    ParticleGenerator(Shader shader, GLuint amount);
    // Update all particles
    void Update(GLfloat dt, glm::vec3 pos, glm::vec3 v, GLuint newParticles, bool cut, glm::vec3 offset = glm::vec3(0.0f, 0.0f,0.0f));
    // Render all particles
    void Draw();
    Shader shader;
private:
    // State
    std::vector<Particle> particles;
    GLuint amount;
    // Render state
    GLuint VAO;
    // Initializes buffer and vertex attributes
    void init();
    // Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    GLuint firstUnusedParticle();
    // Respawns particle
    void respawnParticle(Particle& particle, glm::vec3 pos, glm::vec3 v, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
};

#endif