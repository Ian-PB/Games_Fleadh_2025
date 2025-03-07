#pragma once

#include "raylib.h"
#include "raymath.h"
#include "stdio.h"
#include <algorithm>

#include "../include/ParticleSpawner.h"
#include "../include/Projectile.h"

class Planet
{
public:
    void init(Vector3 t_pos = {0.0f, 0.0f, -9.0f}, int t_maxHealth = 100, Color t_color = WHITE);

    void update(Vector3 t_playerPos3D, Player& t_player);
    void draw();
    void drawParticles();

    Vector3 getPos() { return position; }
    void setPos(Vector3 t_pos) { position = t_pos; }
    bool isCoreConsumed() { return coreConsumed; }
    bool checkIfParticlesActive() { return deathParticles.checkIfParticalsActive(); }

    bool isDefeated() { return defeated; }
    void takeDmg(int t_damage);
    
    void turnOnProjectiles() { fireProjectiles = true; }

    void changeColor(Color t_color);

    void setAlive(); 
    void setOriginalPos(Vector3 t_pos) { originalPos = t_pos; }

    ParticleSpawner& getParticles() { return deathParticles; }

private:    
    Model model;

    Model planet1;
    Model planet2;
    Model planet3;
    Model planet4;

    Model core;
    Model shield;

    Model projectileModel;
    
    Vector3 position;
    Vector3 originalPos = {0, 0, 0};
    
    void updateRotation();
    float tilt = 45.0f;
    float pitch = 0.0f;
    float roll = 0.0f;
    float yaw = 0.0f;
    
    void whileDead(Vector3 t_playerPos3D, Vector2 t_playerPos2D);
    void deathAnimation(Vector2 t_playerPos);
    void moveToPos(Vector3 t_targetPos);
    int maxHealth = 100;
    int health = 0;
    bool defeated = false;
    bool coreConsumed = false;

    Color color;
    Color coreTint = {25, 25, 25, 255};

    void explosion();
    void genExplosionTexture();

    Shader explosionShader;
    // shader locations
    int displacementIntensityLocationInShader;
    int perlinNoiseTextureLocationInShader;
    int explosionTimerLocationInShader;

    Texture2D explosion_driver;
    float displacementIntensity = 0.0f;
    bool explosionActive = false;
    float explosionTimer = 0.0f;
    float mult = 1.0f;
    // Change this number for more dramatic effect
    bool maxDistHit = false;

    ParticleSpawner deathParticles;
    bool maxRadiusHit = false;
    const float MAX_PARTICAL_RADIUS = 150.0f;
    float particleRadius = 30.0f;

    const float SPEED = 0.15f;

    // Projectiles
    void shoot(Vector3 t_playerPos, Player& t_player);
    void shotClock(Vector3 t_playerPos, Player& t_player);
    std::vector<Projectile> projectiles;
    float shootingTimer = 0.0f;

    Sound damageSound;
    Sound destructionSound;
    Sound coreCollectingSound;
    bool coreStart = false;

    bool fireProjectiles = false;
    bool canShoot = false;
    const int COOLDOWN_DUR = 3;
    float cooldownTimer = 0.0f;
};

// Overload the + operator for Raylib's Color type
inline Color operator+(const Color &lhs, const Color &rhs) 
{
    Color result;
    result.r = static_cast<unsigned char>(std::min(static_cast<int>(lhs.r) + static_cast<int>(rhs.r), 255));
    result.g = static_cast<unsigned char>(std::min(static_cast<int>(lhs.g) + static_cast<int>(rhs.g), 255));
    result.b = static_cast<unsigned char>(std::min(static_cast<int>(lhs.b) + static_cast<int>(rhs.b), 255));
    result.a = static_cast<unsigned char>(std::min(static_cast<int>(lhs.a) + static_cast<int>(rhs.a), 255));
    return result;
}