#pragma once
#include "raylib.h"
#include <vector>

class GameMap;

enum class CharacterGender {
    Male,
    Female
};

struct Arrow {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float lifeTimer;
    bool active;
};

struct Afterimage {
    Vector2 position;
    float alpha;
    CharacterGender gender;
    float facingX;
};

struct SlashArc {
    Vector2 origin;
    float radius;
    float startAngle;
    float endAngle;
    float progress; // 0.0 to 1.0
    float facingX;
    bool active;
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float size;
    float life;
    float maxLife;
    bool active;
};

class Character {
public:
    Character(Vector2 startPosition = {1200.0f, 800.0f}, CharacterGender gender = CharacterGender::Female);

    void Update(Vector2 inputDirection, float dt, const GameMap& map);

    // Actions
    bool TriggerEvade(Vector2 moveInput);
    bool TriggerAttack();

    void SetGender(CharacterGender gender) { m_gender = gender; }
    void ToggleGender();

    CharacterGender GetGender() const { return m_gender; }
    const char* GetGenderName() const {
        return (m_gender == CharacterGender::Female) ? "Female Hunter (Great Bow)" : "Male Hunter (Katana)";
    }

    Vector2 GetPosition() const { return m_position; }
    Vector2 GetVelocity() const { return m_velocity; }
    float GetSpeed() const { return m_speed; }
    float GetRadius() const { return m_radius; }
    bool IsMoving() const { return m_isMoving; }
    float GetFacingX() const { return m_facingX; }
    Vector2 GetAimDirection() const { return m_aimDirection; }
    float GetAimAngle() const;

    bool IsEvading() const { return m_isEvading; }
    bool IsAttacking() const { return m_isAttacking; }
    float GetEvadeTimer() const { return m_evadeTimer; }
    float GetEvadeCooldown() const { return m_evadeCooldown; }
    float GetEvadeMaxCooldown() const { return m_evadeMaxCooldown; }
    float GetAttackProgress() const;

    // Health & Damage
    bool TakeDamage(int amount, Vector2 hitPos);
    int GetCurrentHp() const { return m_currentHp; }
    int GetMaxHp() const { return m_maxHp; }
    bool IsDead() const { return m_currentHp <= 0; }
    bool IsInvulnerable() const { return m_isEvading || m_invulnTimer > 0.0f; }
    float GetHitFlashTimer() const { return m_hitFlashTimer; }
    void Heal(int amount);
    void Reset(Vector2 startPosition = {1100.0f, 800.0f});

    const std::vector<Arrow>& GetArrows() const { return m_arrows; }
    const std::vector<Afterimage>& GetAfterimages() const { return m_afterimages; }
    const SlashArc& GetSlashArc() const { return m_slashArc; }
    const std::vector<Particle>& GetParticles() const { return m_particles; }

    // Pure draw method - CALLED ONLY by RenderManager during scene pass
    void Draw(Texture2D maleTexture, Texture2D femaleTexture) const;

private:
    Vector2 m_position;
    Vector2 m_velocity;
    float m_speed;
    float m_radius;
    CharacterGender m_gender;
    bool m_isMoving;
    float m_walkTimer;
    float m_facingX; // 1.0f = East (Default), -1.0f = West
    Vector2 m_aimDirection; // 8-directional attack aim vector

    // Health & Invulnerability
    int m_maxHp;
    int m_currentHp;
    float m_invulnTimer;
    float m_hitFlashTimer;

    // Evade / Dodge Dash state
    bool m_isEvading;
    float m_evadeTimer;
    float m_evadeDuration;
    float m_evadeCooldown;
    float m_evadeMaxCooldown;
    Vector2 m_evadeDirection;
    float m_afterimageTimer;

    // Attack state
    bool m_isAttacking;
    float m_attackTimer;
    float m_attackDuration;
    float m_attackCooldown;

    // Combat visual elements
    std::vector<Arrow> m_arrows;
    std::vector<Afterimage> m_afterimages;
    SlashArc m_slashArc;
    std::vector<Particle> m_particles;

    void SpawnParticles(Vector2 pos, Color col, int count, float speed);
};
