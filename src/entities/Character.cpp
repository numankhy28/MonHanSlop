#include "Character.h"
#include "DamageIndicator.h"
#include "../world/GameMap.h"
#include <cmath>
#include <cstdlib>

Character::Character(Vector2 startPosition, CharacterGender gender)
    : m_position(startPosition),
      m_velocity{0.0f, 0.0f},
      m_speed(360.0f),
      m_radius(28.0f),
      m_gender(gender),
      m_isMoving(false),
      m_walkTimer(0.0f),
      m_facingX(1.0f),
      m_aimDirection{1.0f, 0.0f},
      m_maxHp(100),
      m_currentHp(100),
      m_invulnTimer(0.0f),
      m_hitFlashTimer(0.0f),
      m_isEvading(false),
      m_evadeTimer(0.0f),
      m_evadeDuration(0.20f),
      m_evadeCooldown(0.0f),
      m_evadeMaxCooldown(3.0f),
      m_evadeDirection{1.0f, 0.0f},
      m_afterimageTimer(0.0f),
      m_isAttacking(false),
      m_attackTimer(0.0f),
      m_attackDuration(0.30f),
      m_attackCooldown(0.0f),
      m_slashArc{Vector2{0, 0}, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, false} {
}

void Character::ToggleGender() {
    m_gender = (m_gender == CharacterGender::Female) ? CharacterGender::Male : CharacterGender::Female;
    m_isAttacking = false;
    m_attackTimer = 0.0f;
    m_slashArc.active = false;
}

float Character::GetAimAngle() const {
    return std::atan2(m_aimDirection.y, m_aimDirection.x) * RAD2DEG;
}

bool Character::TriggerEvade(Vector2 moveInput) {
    if (m_isEvading || m_evadeCooldown > 0.0f) return false;

    // Determine evade dash direction: joystick direction, aim direction or facing direction
    float inputLenSq = moveInput.x * moveInput.x + moveInput.y * moveInput.y;
    if (inputLenSq > 0.05f) {
        float len = std::sqrt(inputLenSq);
        m_evadeDirection = Vector2{moveInput.x / len, moveInput.y / len};
    } else {
        m_evadeDirection = m_aimDirection;
    }

    m_isEvading = true;
    m_evadeTimer = m_evadeDuration;
    m_evadeCooldown = m_evadeMaxCooldown; // 3.0 seconds reload cooldown
    m_afterimageTimer = 0.0f;

    // Interrupt attack if evading (Monster Hunter cancel window)
    m_isAttacking = false;
    m_slashArc.active = false;

    // Spawn evade dust puff at feet
    SpawnParticles(Vector2{m_position.x, m_position.y + 20.0f}, Color{210, 200, 180, 180}, 8, 80.0f);

    return true;
}

bool Character::TriggerAttack() {
    if (m_isAttacking || m_isEvading || m_attackCooldown > 0.0f) return false;

    m_isAttacking = true;
    m_attackTimer = m_attackDuration;
    m_attackCooldown = 0.32f;

    if (m_gender == CharacterGender::Female) {
        // Female Hunter: Release Great Bow Arrow in the 8-directional aim vector
        Vector2 arrowPos = {
            m_position.x + m_aimDirection.x * 35.0f,
            m_position.y - 18.0f + m_aimDirection.y * 35.0f
        };
        Vector2 arrowVel = {
            m_aimDirection.x * 1200.0f,
            m_aimDirection.y * 1200.0f
        };
        float rot = std::atan2(m_aimDirection.y, m_aimDirection.x) * RAD2DEG;

        m_arrows.push_back(Arrow{arrowPos, arrowVel, rot, 0.0f, true});

        // Bowstring release flash particles
        SpawnParticles(arrowPos, Color{255, 230, 160, 255}, 6, 130.0f);
    } else {
        // Male Hunter: Katana Arc Slash oriented to the 8-directional vector!
        // Arc radius significantly extended to 150.0f (high reach & impact)
        Vector2 arcOrigin = {
            m_position.x + m_aimDirection.x * 25.0f,
            m_position.y - 18.0f + m_aimDirection.y * 25.0f
        };
        m_slashArc.origin = arcOrigin;
        m_slashArc.radius = 150.0f; // Extended size as requested
        m_slashArc.facingX = m_facingX;
        m_slashArc.progress = 0.0f;
        m_slashArc.active = true;

        float centerAngle = std::atan2(m_aimDirection.y, m_aimDirection.x) * RAD2DEG;
        m_slashArc.startAngle = centerAngle - 68.0f;
        m_slashArc.endAngle   = centerAngle + 68.0f;

        // Forward sword lunge in the aim direction
        m_position.x += m_aimDirection.x * 28.0f;
        m_position.y += m_aimDirection.y * 28.0f;

        // Katana swing gleam sparks
        SpawnParticles(m_slashArc.origin, Color{160, 220, 255, 255}, 10, 160.0f);
    }

    return true;
}

float Character::GetAttackProgress() const {
    if (!m_isAttacking || m_attackDuration <= 0.0f) return 0.0f;
    return 1.0f - (m_attackTimer / m_attackDuration);
}

void Character::SpawnParticles(Vector2 pos, Color col, int count, float speed) {
    for (int i = 0; i < count; i++) {
        float angle = (float)(rand() % 360) * DEG2RAD;
        float spd = ((float)(rand() % 100) / 100.0f) * speed;
        Vector2 vel = {std::cos(angle) * spd, std::sin(angle) * spd};
        float size = 3.0f + (float)(rand() % 4);
        float life = 0.25f + ((float)(rand() % 20) / 100.0f);
        m_particles.push_back(Particle{pos, vel, col, size, life, life, true});
    }
}

bool Character::TakeDamage(int amount, Vector2 hitPos) {
    (void)hitPos;
    // Evade grants invulnerability frames (i-frames)!
    if (m_isEvading || m_invulnTimer > 0.0f || m_currentHp <= 0) {
        return false;
    }

    m_currentHp -= amount;
    if (m_currentHp < 0) m_currentHp = 0;

    m_invulnTimer = 0.65f;
    m_hitFlashTimer = 0.22f;

    // Floating damage indicator
    DamageSystem::Instance().Spawn(m_position, amount, Color{255, 65, 65, 255}, false);

    // Blood / hit particles
    SpawnParticles(m_position, Color{220, 45, 45, 255}, 14, 120.0f);

    return true;
}

void Character::Heal(int amount) {
    m_currentHp += amount;
    if (m_currentHp > m_maxHp) m_currentHp = m_maxHp;
    DamageSystem::Instance().Spawn(m_position, amount, Color{60, 240, 90, 255}, false);
}

void Character::Reset(Vector2 startPosition) {
    m_position = startPosition;
    m_velocity = {0.0f, 0.0f};
    m_currentHp = m_maxHp;
    m_invulnTimer = 0.0f;
    m_hitFlashTimer = 0.0f;
    m_isEvading = false;
    m_evadeTimer = 0.0f;
    m_evadeCooldown = 0.0f;
    m_isAttacking = false;
    m_attackTimer = 0.0f;
    m_attackCooldown = 0.0f;
    m_slashArc.active = false;
    m_arrows.clear();
    m_afterimages.clear();
    m_particles.clear();
    m_facingX = 1.0f;
    m_aimDirection = Vector2{1.0f, 0.0f};
}

void Character::Update(Vector2 inputDirection, float dt, const GameMap& map) {
    // Timers
    if (m_invulnTimer > 0.0f) m_invulnTimer -= dt;
    if (m_hitFlashTimer > 0.0f) m_hitFlashTimer -= dt;
    if (m_evadeCooldown > 0.0f) m_evadeCooldown -= dt;
    if (m_attackCooldown > 0.0f) m_attackCooldown -= dt;

    // Update 8-directional attack aim vector whenever joystick or movement key is used
    float inLenSq = inputDirection.x * inputDirection.x + inputDirection.y * inputDirection.y;
    if (inLenSq > 0.04f) {
        // Snap angle to 8 cardinal / intercardinal directions (45-degree / PI/4 sectors)
        float angle = std::atan2(inputDirection.y, inputDirection.x);
        float sector = std::round(angle / (PI * 0.25f));
        float snappedAngle = sector * (PI * 0.25f);
        m_aimDirection = Vector2{ std::cos(snappedAngle), std::sin(snappedAngle) };

        // Update facing direction (East / West)
        if (m_aimDirection.x > 0.1f) {
            m_facingX = 1.0f;
        } else if (m_aimDirection.x < -0.1f) {
            m_facingX = -1.0f;
        }
    }

    // 2. Evade Dash State Handling (Sprite displacement)
    if (m_isEvading) {
        float evadeSpeed = 850.0f; // rapid swift displacement
        m_position.x += m_evadeDirection.x * evadeSpeed * dt;
        m_position.y += m_evadeDirection.y * evadeSpeed * dt;

        m_evadeTimer -= dt;
        m_afterimageTimer += dt;

        // Leave ghost trail afterimages during dash displacement
        if (m_afterimageTimer >= 0.035f) {
            m_afterimageTimer = 0.0f;
            m_afterimages.push_back(Afterimage{m_position, 0.7f, m_gender, m_facingX});
        }

        if (m_evadeTimer <= 0.0f) {
            m_isEvading = false;
        }
    }
    // 3. Attack State Handling
    else if (m_isAttacking) {
        m_attackTimer -= dt;
        if (m_gender == CharacterGender::Male && m_slashArc.active) {
            m_slashArc.progress = GetAttackProgress();
            m_slashArc.origin = Vector2{
                m_position.x + m_aimDirection.x * 25.0f,
                m_position.y - 18.0f + m_aimDirection.y * 25.0f
            };
        }

        if (m_attackTimer <= 0.0f) {
            m_isAttacking = false;
            m_slashArc.active = false;
        }
    }
    // 4. Normal Movement Handling
    else {
        m_velocity.x = inputDirection.x * m_speed;
        m_velocity.y = inputDirection.y * m_speed;

        float speedSq = m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y;
        m_isMoving = (speedSq > 1.0f);

        if (m_isMoving) {
            m_position.x += m_velocity.x * dt;
            m_position.y += m_velocity.y * dt;
            m_walkTimer += dt;
        } else {
            m_walkTimer = 0.0f;
        }
    }

    // Keep character inside arena boundary
    m_position = map.ClampToBounds(m_position, m_radius);

    // 5. Update Arrows
    for (size_t i = 0; i < m_arrows.size(); i++) {
        if (!m_arrows[i].active) continue;
        m_arrows[i].position.x += m_arrows[i].velocity.x * dt;
        m_arrows[i].position.y += m_arrows[i].velocity.y * dt;
        m_arrows[i].lifeTimer += dt;

        // Spawn subtle arrow air-trail spark
        if (rand() % 3 == 0) {
            m_particles.push_back(Particle{
                m_arrows[i].position,
                Vector2{0.0f, (float)(rand() % 40 - 20)},
                Color{255, 230, 150, 180},
                2.5f,
                0.2f,
                0.2f,
                true
            });
        }

        // Deactivate arrow after 1.8 seconds or out of arena bounds
        if (m_arrows[i].lifeTimer > 1.8f ||
            m_arrows[i].position.x < 0.0f || m_arrows[i].position.x > map.GetWidth()) {
            // Impact spark on arena border
            SpawnParticles(m_arrows[i].position, Color{220, 180, 100, 220}, 6, 90.0f);
            m_arrows[i].active = false;
        }
    }

    // Clean up inactive arrows
    for (int i = (int)m_arrows.size() - 1; i >= 0; i--) {
        if (!m_arrows[i].active) {
            m_arrows.erase(m_arrows.begin() + i);
        }
    }

    // 6. Update Afterimages (fade out)
    for (size_t i = 0; i < m_afterimages.size(); i++) {
        m_afterimages[i].alpha -= dt * 3.5f;
    }
    for (int i = (int)m_afterimages.size() - 1; i >= 0; i--) {
        if (m_afterimages[i].alpha <= 0.0f) {
            m_afterimages.erase(m_afterimages.begin() + i);
        }
    }

    // 7. Update Particles
    for (size_t i = 0; i < m_particles.size(); i++) {
        if (!m_particles[i].active) continue;
        m_particles[i].position.x += m_particles[i].velocity.x * dt;
        m_particles[i].position.y += m_particles[i].velocity.y * dt;
        m_particles[i].life -= dt;
        if (m_particles[i].life <= 0.0f) {
            m_particles[i].active = false;
        }
    }
    for (int i = (int)m_particles.size() - 1; i >= 0; i--) {
        if (!m_particles[i].active) {
            m_particles.erase(m_particles.begin() + i);
        }
    }
}

void Character::Draw(Texture2D maleTexture, Texture2D femaleTexture) const {
    Texture2D activeTex = (m_gender == CharacterGender::Female) ? femaleTexture : maleTexture;

    // 1. Drop shadow under character's feet
    Vector2 shadowPos = {m_position.x, m_position.y + 22.0f};
    DrawEllipse((int)shadowPos.x, (int)shadowPos.y, 28.0f, 12.0f, Color{10, 10, 10, 110});

    // 2. Walk animation bounce / Evade tilt
    float bounce = (m_isMoving && !m_isEvading) ? std::sin(m_walkTimer * 14.0f) * 4.0f : 0.0f;
    float rotation = 0.0f;

    if (m_isEvading) {
        // Dynamic evade dash tilt
        rotation = m_evadeDirection.x * 15.0f;
    } else if (m_isAttacking) {
        // Attack pose tilt: female leans back slightly for shot, male lunges forward
        float attackProg = GetAttackProgress();
        if (m_gender == CharacterGender::Female) {
            rotation = -m_facingX * (1.0f - attackProg) * 8.0f;
        } else {
            rotation = m_facingX * (1.0f - attackProg) * 12.0f;
        }
    }

    if (activeTex.id > 0) {
        float spriteW = (float)activeTex.width;
        float spriteH = (float)activeTex.height;

        // Target character height in 720p virtual screen is 125px
        float targetH = 125.0f;
        float targetW = (spriteW / spriteH) * targetH;

        Rectangle source = {
            0.0f,
            0.0f,
            spriteW * m_facingX,
            spriteH
        };

        Rectangle dest = {
            m_position.x,
            m_position.y - (targetH * 0.42f) + 10.0f + bounce,
            targetW,
            targetH
        };

        Vector2 origin = {targetW * 0.5f, targetH * 0.5f};
        Color tint = WHITE;
        if (m_hitFlashTimer > 0.0f) {
            tint = Color{255, 110, 110, 255}; // Red hit flash
        } else if (m_isEvading) {
            tint = Color{200, 230, 255, 220}; // Evade blue tint
        } else if (m_invulnTimer > 0.0f) {
            int flicker = (int)(m_invulnTimer * 20.0f) % 2;
            tint = (flicker == 0) ? Color{255, 255, 255, 120} : WHITE;
        }
        DrawTexturePro(activeTex, source, dest, origin, rotation, tint);
    }
}
