#pragma once
#include "raylib.h"
#include <vector>

struct Fireball {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float life;
    bool active;
};

struct ClawStrike {
    Vector2 position;
    float timer;
    float warningDuration;
    float totalDuration;
    float radius;
    bool isWarning;
    bool hasDamaged;
    bool active;
};

struct BurningSquare {
    Rectangle rect;
    float timer;
    float warningDuration;
    float totalDuration;
    bool isBurning;
    float damageTickTimer;
    bool active;
};

class Character;
class GameMap;

class DragonBoss {
public:
    DragonBoss(Vector2 startPos = {1750.0f, 750.0f});

    void Update(float dt, Character& player, const GameMap& map);

    // Hit reception
    bool TakeDamage(int amount, bool isCrit, Vector2 hitPos);

    // Getters
    Vector2 GetPosition() const { return m_position; }
    float GetRadius() const { return m_radius; }
    int GetCurrentHp() const { return m_currentHp; }
    int GetMaxHp() const { return m_maxHp; }
    bool IsDead() const { return m_currentHp <= 0; }
    float GetHitFlashTimer() const { return m_hitFlashTimer; }
    float GetFacingX() const { return m_facingX; }

    const std::vector<Fireball>& GetFireballs() const { return m_fireballs; }
    const std::vector<ClawStrike>& GetClawStrikes() const { return m_clawStrikes; }
    const std::vector<BurningSquare>& GetBurningSquares() const { return m_burningSquares; }

    // Pure draw method - CALLED ONLY by RenderManager
    void Draw(Texture2D bossTexture) const;

    void Reset(Vector2 startPos = {1750.0f, 750.0f});

private:
    Vector2 m_position;
    Vector2 m_velocity;
    float m_radius;
    int m_maxHp;
    int m_currentHp;
    float m_hitFlashTimer;

    // AI & Attack Timers
    float m_attackTimer;
    float m_attackInterval;
    int m_attackCycle;
    float m_facingX; // Default -1.0f (Facing West towards player)

    // Attacks
    std::vector<Fireball> m_fireballs;
    std::vector<ClawStrike> m_clawStrikes;
    std::vector<BurningSquare> m_burningSquares;

    void CastFireball(Vector2 targetPos);
    void CastClawStrike(Vector2 targetPos);
    void CastBurningSquare(Vector2 targetPos);
};
