#include "DragonBoss.h"
#include "Character.h"
#include "DamageIndicator.h"
#include "../world/GameMap.h"
#include <cmath>
#include <cstdlib>

DragonBoss::DragonBoss(Vector2 startPos)
    : m_position(startPos),
      m_velocity{0.0f, 0.0f},
      m_radius(68.0f),
      m_maxHp(2000),
      m_currentHp(2000),
      m_hitFlashTimer(0.0f),
      m_attackTimer(1.5f),
      m_attackInterval(2.2f),
      m_attackCycle(0),
      m_facingX(-1.0f) {
}

bool DragonBoss::TakeDamage(int amount, bool isCrit, Vector2 hitPos) {
    if (m_currentHp <= 0) return false;

    m_currentHp -= amount;
    if (m_currentHp < 0) m_currentHp = 0;

    m_hitFlashTimer = 0.20f;

    // Spawn floating damage text on boss
    Color dmgColor = isCrit ? Color{255, 215, 30, 255} : Color{255, 130, 45, 255};
    DamageSystem::Instance().Spawn(hitPos, amount, dmgColor, isCrit);

    return true;
}

void DragonBoss::Reset(Vector2 startPos) {
    m_position = startPos;
    m_velocity = {0.0f, 0.0f};
    m_currentHp = m_maxHp;
    m_hitFlashTimer = 0.0f;
    m_attackTimer = 1.5f;
    m_attackCycle = 0;
    m_facingX = -1.0f;
    m_fireballs.clear();
    m_clawStrikes.clear();
    m_burningSquares.clear();
}

void DragonBoss::CastFireball(Vector2 targetPos) {
    Vector2 mouthPos = {m_position.x + m_facingX * 55.0f, m_position.y - 12.0f};
    float dx = targetPos.x - mouthPos.x;
    float dy = targetPos.y - mouthPos.y;
    float len = std::sqrt(dx * dx + dy * dy);

    if (len > 0.001f) {
        float speed = 540.0f;
        Vector2 vel = {(dx / len) * speed, (dy / len) * speed};
        m_fireballs.push_back(Fireball{mouthPos, vel, 20.0f, 0.0f, true});
    }
}

void DragonBoss::CastClawStrike(Vector2 targetPos) {
    // Spawns a claw attack hazard at the target area near player
    ClawStrike cs;
    cs.position = targetPos;
    cs.timer = 0.0f;
    cs.warningDuration = 0.45f;
    cs.totalDuration = 0.75f;
    cs.radius = 65.0f;
    cs.isWarning = true;
    cs.hasDamaged = false;
    cs.active = true;
    m_clawStrikes.push_back(cs);
}

void DragonBoss::CastBurningSquare(Vector2 targetPos) {
    // Targets a 180x180 square zone on the ground
    BurningSquare bs;
    float size = 180.0f;
    bs.rect = Rectangle{targetPos.x - size * 0.5f, targetPos.y - size * 0.5f, size, size};
    bs.timer = 0.0f;
    bs.warningDuration = 0.70f;
    bs.totalDuration = 4.20f; // burns for 3.5 seconds after warning
    bs.isBurning = false;
    bs.damageTickTimer = 0.0f;
    bs.active = true;
    m_burningSquares.push_back(bs);
}

void DragonBoss::Update(float dt, Character& player, const GameMap& map) {
    if (m_hitFlashTimer > 0.0f) m_hitFlashTimer -= dt;

    if (m_currentHp <= 0) return; // Boss defeated

    Vector2 playerPos = player.GetPosition();

    // 1. Facing Direction (Face towards the player)
    m_facingX = (playerPos.x < m_position.x) ? -1.0f : 1.0f;

    // 2. Gentle hovering movement to maintain combat range
    float dx = playerPos.x - m_position.x;
    float dy = playerPos.y - m_position.y;
    float distToPlayer = std::sqrt(dx * dx + dy * dy);

    // Ideal range: 380 to 560 px (keeps boss further away on the right side)
    if (distToPlayer > 540.0f) {
        // Move slightly closer
        m_position.x += (dx / distToPlayer) * 80.0f * dt;
        m_position.y += (dy / distToPlayer) * 80.0f * dt;
    } else if (distToPlayer < 360.0f) {
        // Back off slightly
        m_position.x -= (dx / distToPlayer) * 85.0f * dt;
        m_position.y -= (dy / distToPlayer) * 85.0f * dt;
    }
    m_position = map.ClampToBounds(m_position, m_radius + 40.0f);

    // 3. Attack Cycling
    m_attackTimer -= dt;
    if (m_attackTimer <= 0.0f) {
        m_attackTimer = m_attackInterval;

        // Rotate through the 3 requested attacks:
        // 0: Fireball
        // 1: Claw Strike Spawn
        // 2: Burning Square Zone
        switch (m_attackCycle % 3) {
            case 0:
                CastFireball(playerPos);
                break;
            case 1:
                CastClawStrike(playerPos);
                break;
            case 2:
                CastBurningSquare(playerPos);
                break;
        }
        m_attackCycle++;
    }

    // 4. Update Fireballs
    for (size_t i = 0; i < m_fireballs.size(); i++) {
        if (!m_fireballs[i].active) continue;

        m_fireballs[i].position.x += m_fireballs[i].velocity.x * dt;
        m_fireballs[i].position.y += m_fireballs[i].velocity.y * dt;
        m_fireballs[i].life += dt;

        // Collision with player
        float fdx = playerPos.x - m_fireballs[i].position.x;
        float fdy = playerPos.y - m_fireballs[i].position.y;
        float fdist = std::sqrt(fdx * fdx + fdy * fdy);

        if (fdist < m_fireballs[i].radius + player.GetRadius()) {
            player.TakeDamage(26, m_fireballs[i].position);
            m_fireballs[i].active = false;
        }

        // Expire after 3 seconds or out of bounds
        if (m_fireballs[i].life > 3.0f ||
            m_fireballs[i].position.x < 0 || m_fireballs[i].position.x > map.GetWidth() ||
            m_fireballs[i].position.y < 0 || m_fireballs[i].position.y > map.GetHeight()) {
            m_fireballs[i].active = false;
        }
    }
    for (int i = (int)m_fireballs.size() - 1; i >= 0; i--) {
        if (!m_fireballs[i].active) m_fireballs.erase(m_fireballs.begin() + i);
    }

    // 5. Update Claw Strikes
    for (size_t i = 0; i < m_clawStrikes.size(); i++) {
        if (!m_clawStrikes[i].active) continue;

        m_clawStrikes[i].timer += dt;
        if (m_clawStrikes[i].timer >= m_clawStrikes[i].warningDuration) {
            m_clawStrikes[i].isWarning = false;

            if (!m_clawStrikes[i].hasDamaged) {
                float cdx = playerPos.x - m_clawStrikes[i].position.x;
                float cdy = playerPos.y - m_clawStrikes[i].position.y;
                float cdist = std::sqrt(cdx * cdx + cdy * cdy);

                if (cdist < m_clawStrikes[i].radius + player.GetRadius()) {
                    player.TakeDamage(34, m_clawStrikes[i].position);
                    m_clawStrikes[i].hasDamaged = true;
                }
            }
        }

        if (m_clawStrikes[i].timer >= m_clawStrikes[i].totalDuration) {
            m_clawStrikes[i].active = false;
        }
    }
    for (int i = (int)m_clawStrikes.size() - 1; i >= 0; i--) {
        if (!m_clawStrikes[i].active) m_clawStrikes.erase(m_clawStrikes.begin() + i);
    }

    // 6. Update Burning Squares
    for (size_t i = 0; i < m_burningSquares.size(); i++) {
        if (!m_burningSquares[i].active) continue;

        m_burningSquares[i].timer += dt;
        if (m_burningSquares[i].timer >= m_burningSquares[i].warningDuration) {
            m_burningSquares[i].isBurning = true;

            // Damage tick every 0.35s while player stands inside
            m_burningSquares[i].damageTickTimer += dt;
            if (m_burningSquares[i].damageTickTimer >= 0.35f) {
                m_burningSquares[i].damageTickTimer = 0.0f;
                if (CheckCollisionPointRec(playerPos, m_burningSquares[i].rect)) {
                    player.TakeDamage(12, playerPos);
                }
            }
        }

        if (m_burningSquares[i].timer >= m_burningSquares[i].totalDuration) {
            m_burningSquares[i].active = false;
        }
    }
    for (int i = (int)m_burningSquares.size() - 1; i >= 0; i--) {
        if (!m_burningSquares[i].active) m_burningSquares.erase(m_burningSquares.begin() + i);
    }

    // 7. Check Player Weapons Hitting Boss
    // (a) Arrows from Great Bow
    const auto& arrows = player.GetArrows();
    for (size_t i = 0; i < arrows.size(); i++) {
        if (!arrows[i].active) continue;

        float adx = arrows[i].position.x - m_position.x;
        float ady = arrows[i].position.y - m_position.y;
        float adist = std::sqrt(adx * adx + ady * ady);

        if (adist < m_radius + 20.0f) {
            // Hit! 25% critical hit chance
            bool isCrit = (rand() % 4 == 0);
            int dmg = isCrit ? (75 + rand() % 20) : (48 + rand() % 15);
            TakeDamage(dmg, isCrit, arrows[i].position);

            // Deactivate arrow by casting away
            const_cast<Arrow&>(arrows[i]).active = false;
        }
    }

    // (b) Katana Slash Arc from Male Hunter
    const auto& slashArc = player.GetSlashArc();
    if (player.GetGender() == CharacterGender::Male && slashArc.active && slashArc.progress > 0.3f && slashArc.progress < 0.65f) {
        float sdx = slashArc.origin.x - m_position.x;
        float sdy = slashArc.origin.y - m_position.y;
        float sdist = std::sqrt(sdx * sdx + sdy * sdy);

        if (sdist < slashArc.radius + m_radius) {
            // Katana hits with extended arc and high damage!
            static float katanaHitCooldown = 0.0f;
            katanaHitCooldown -= dt;
            if (katanaHitCooldown <= 0.0f) {
                katanaHitCooldown = 0.28f;
                bool isCrit = (rand() % 3 == 0);
                int dmg = isCrit ? (165 + rand() % 35) : (115 + rand() % 25);
                TakeDamage(dmg, isCrit, Vector2{m_position.x - m_facingX * 20.0f, m_position.y});
            }
        }
    }
}

void DragonBoss::Draw(Texture2D bossTexture) const {
    if (m_currentHp <= 0) {
        // Defeated indicator
        DrawText("DRAGON WYVERN HUNTED!", (int)m_position.x - 120, (int)m_position.y - 20, 20, Color{255, 215, 50, 255});
        return;
    }

    // 1. Drop shadow under boss
    Vector2 shadowPos = {m_position.x, m_position.y + 70.0f};
    DrawEllipse((int)shadowPos.x, (int)shadowPos.y, 110.0f, 32.0f, Color{10, 10, 10, 130});

    // 2. Boss Sprite
    if (bossTexture.id > 0) {
        float spriteW = (float)bossTexture.width;
        float spriteH = (float)bossTexture.height;

        // Target boss height: 260px (imposing Rathalos wyvern scale)
        float targetH = 260.0f;
        float targetW = (spriteW / spriteH) * targetH;

        // Facing direction: m_facingX is -1.0f (facing West) or 1.0f (facing East)
        Rectangle source = {
            0.0f,
            0.0f,
            spriteW * (-m_facingX), // Base texture faces West
            spriteH
        };

        Rectangle dest = {
            m_position.x,
            m_position.y - 20.0f,
            targetW,
            targetH
        };

        Vector2 origin = {targetW * 0.5f, targetH * 0.5f};
        Color tint = (m_hitFlashTimer > 0.0f) ? Color{255, 120, 120, 255} : WHITE;
        DrawTexturePro(bossTexture, source, dest, origin, 0.0f, tint);
    } else {
        // Fallback procedural circle
        Color bodyColor = (m_hitFlashTimer > 0.0f) ? Color{255, 120, 120, 255} : Color{180, 40, 40, 255};
        DrawCircleV(m_position, m_radius, bodyColor);
        DrawCircleLines((int)m_position.x, (int)m_position.y, m_radius, Color{255, 200, 50, 255});
    }
}
