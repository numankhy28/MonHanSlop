#include "RenderManager.h"
#include "../world/GameMap.h"
#include "../entities/Character.h"
#include "../entities/DragonBoss.h"
#include "../entities/DamageIndicator.h"
#include "../input/VirtualJoystick.h"
#include "../core/ScreenManager.h"
#include "../assets/AssetManager.h"
#include <cmath>

RenderManager::RenderManager(int virtualWidth, int virtualHeight)
    : m_virtualWidth(virtualWidth),
      m_virtualHeight(virtualHeight) {
    m_targetTexture = LoadRenderTexture(m_virtualWidth, m_virtualHeight);
    SetTextureFilter(m_targetTexture.texture, TEXTURE_FILTER_BILINEAR);

    m_camera.target = Vector2{1200.0f, 800.0f};
    m_camera.offset = Vector2{(float)m_virtualWidth * 0.5f, (float)m_virtualHeight * 0.5f};
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.0f;
}

RenderManager::~RenderManager() {
    UnloadRenderTexture(m_targetTexture);
}

void RenderManager::UpdateCamera(Vector2 targetPos, float mapWidth, float mapHeight, float dt) {
    float halfW = (float)m_virtualWidth * 0.5f;
    float halfH = (float)m_virtualHeight * 0.5f;

    float clampedX = targetPos.x;
    float clampedY = targetPos.y;

    if (mapWidth > m_virtualWidth) {
        if (clampedX < halfW) clampedX = halfW;
        if (clampedX > mapWidth - halfW) clampedX = mapWidth - halfW;
    } else {
        clampedX = mapWidth * 0.5f;
    }

    if (mapHeight > m_virtualHeight) {
        if (clampedY < halfH) clampedY = halfH;
        if (clampedY > mapHeight - halfH) clampedY = mapHeight - halfH;
    } else {
        clampedY = mapHeight * 0.5f;
    }

    float smoothFactor = fminf(10.0f * dt, 1.0f);
    m_camera.target.x += (clampedX - m_camera.target.x) * smoothFactor;
    m_camera.target.y += (clampedY - m_camera.target.y) * smoothFactor;
}

void RenderManager::Render(const GameMap& map,
                           const Character& character,
                           const DragonBoss& boss,
                           const VirtualJoystick& joystick,
                           const ScreenManager& screenManager,
                           const AssetManager& assetManager,
                           GameState gameState) {
    // =========================================================================
    // 1. RENDER TO VIRTUAL SCREEN BUFFER (Texture Mode)
    // =========================================================================
    BeginTextureMode(m_targetTexture);
        ClearBackground(Color{25, 30, 28, 255});

        // --- World Pass (2D Camera) ---
        BeginMode2D(m_camera);
            RenderWorldPass(map, character, boss, assetManager);
        EndMode2D();

        // --- Screen-Space UI Pass (Fixed 1280x720 canvas) ---
        RenderUIPass(character, boss, joystick);

        // --- Game Over / Victory Overlay ---
        if (gameState != GameState::Playing) {
            RenderGameOverScreen(gameState);
        }

    EndTextureMode();

    // =========================================================================
    // 2. PRESENTATION PASS TO PHYSICAL WINDOW / DISPLAY (Single Render Point)
    // =========================================================================
    BeginDrawing();
        ClearBackground(Color{12, 14, 18, 255});

        DrawTexturePro(
            m_targetTexture.texture,
            screenManager.GetSourceRect(),
            screenManager.GetDestRect(),
            Vector2{0.0f, 0.0f},
            0.0f,
            WHITE
        );

        Rectangle dest = screenManager.GetDestRect();
        DrawRectangleLinesEx(dest, 2.0f, Color{60, 65, 75, 200});
    EndDrawing();
}

void RenderManager::RenderWorldPass(const GameMap& map,
                                    const Character& character,
                                    const DragonBoss& boss,
                                    const AssetManager& assetManager) {
    // 1. Map Arena Tiles & Floor
    map.Draw(assetManager.GetMapTexture());

    // 2. Boss Environmental Ground Hazards (Burning Squares)
    RenderBossAttacks(boss);

    // 3. Evade Afterimage Ghost Trails
    RenderAfterimages(character, assetManager);

    // 4. Combat Particles (dust, wind, sparks)
    RenderParticles(character);

    // 5. Character Entity
    character.Draw(assetManager.GetMaleTexture(), assetManager.GetFemaleTexture());

    // 5b. 8-Directional Aim Vector Indicator (Small Glowing Blue Orb)
    RenderAimIndicator(character);

    // 6. Dragon Wyvern Boss
    boss.Draw(assetManager.GetDragonTexture());

    // 7. Male Katana Slash Arc Wave
    RenderSlashArc(character);

    // 8. Female Hunter Projectile Arrows
    RenderArrows(character);

    // 9. Floating Damage Indicators (Damage Numbers)
    RenderDamageIndicators();
}

void RenderManager::RenderBossAttacks(const DragonBoss& boss) {
    // --- 1. Burning Square Zones ---
    const auto& burningSquares = boss.GetBurningSquares();
    for (const auto& bs : burningSquares) {
        if (!bs.active) continue;

        if (!bs.isBurning) {
            // Telegraph Warning Phase: Pulsing orange/red border and hazard cross
            float pulse = (std::sin(bs.timer * 16.0f) + 1.0f) * 0.5f;
            Color warnBorder = Fade(Color{255, 120, 20, 255}, 0.5f + pulse * 0.5f);
            Color warnFill   = Fade(Color{220, 80, 20, 100}, 0.15f + pulse * 0.20f);

            DrawRectangleRec(bs.rect, warnFill);
            DrawRectangleLinesEx(bs.rect, 3.0f, warnBorder);

            // Diagonal hazard lines inside square
            DrawLineEx(Vector2{bs.rect.x, bs.rect.y}, Vector2{bs.rect.x + bs.rect.width, bs.rect.y + bs.rect.height}, 2.0f, warnBorder);
            DrawLineEx(Vector2{bs.rect.x + bs.rect.width, bs.rect.y}, Vector2{bs.rect.x, bs.rect.y + bs.rect.height}, 2.0f, warnBorder);
            DrawText("BURNING ZONE", (int)bs.rect.x + 28, (int)bs.rect.y + (int)bs.rect.height * 0.5f - 8, 14, Color{255, 200, 100, 230});
        } else {
            // Active Blazing Fire Phase
            float flicker = (float)(GetRandomValue(0, 40)) / 255.0f;
            Color flameFill = Color{215, 60, 15, (unsigned char)(170 + flicker * 60)};
            DrawRectangleRec(bs.rect, flameFill);
            DrawRectangleLinesEx(bs.rect, 3.0f, Color{255, 210, 40, 255});

            // Animated flame pillars / embers inside square
            for (int fx = 15; fx < bs.rect.width; fx += 30) {
                float hFlicker = (float)(GetRandomValue(10, 35));
                Vector2 b1 = {bs.rect.x + (float)fx, bs.rect.y + bs.rect.height - 5.0f};
                Vector2 b2 = {bs.rect.x + (float)fx + 10.0f, bs.rect.y + bs.rect.height - 5.0f};
                Vector2 tip = {bs.rect.x + (float)fx + 5.0f, bs.rect.y + bs.rect.height - 5.0f - hFlicker};
                DrawTriangle(tip, b1, b2, Color{255, 230, 60, 220});
            }
        }
    }

    // --- 2. Claw Strikes ---
    const auto& clawStrikes = boss.GetClawStrikes();
    for (const auto& cs : clawStrikes) {
        if (!cs.active) continue;

        if (cs.isWarning) {
            // Warning Telegraph Circle
            DrawCircleV(cs.position, cs.radius, Color{200, 30, 20, 60});
            DrawCircleLines((int)cs.position.x, (int)cs.position.y, cs.radius, Color{255, 80, 50, 200});
            // Claw mark shadows
            for (int i = -1; i <= 1; i++) {
                DrawLineEx(Vector2{cs.position.x + (float)i * 18.0f - 15.0f, cs.position.y - 20.0f},
                           Vector2{cs.position.x + (float)i * 18.0f + 15.0f, cs.position.y + 20.0f},
                           2.0f, Color{255, 100, 70, 150});
            }
        } else {
            // Active Fierce Claw Slash marks (3 deep glowing red lacerations)
            for (int i = -1; i <= 1; i++) {
                Vector2 start = {cs.position.x + (float)i * 22.0f - 28.0f, cs.position.y - 32.0f};
                Vector2 end   = {cs.position.x + (float)i * 22.0f + 28.0f, cs.position.y + 32.0f};
                // Outer red claw slash
                DrawLineEx(start, end, 6.0f, Color{240, 40, 30, 240});
                // Inner bright orange/white slash core
                DrawLineEx(start, end, 2.5f, Color{255, 240, 180, 255});
            }
            DrawCircleV(cs.position, 16.0f, Color{255, 150, 40, 160});
        }
    }

    // --- 3. Fireballs ---
    const auto& fireballs = boss.GetFireballs();
    for (const auto& fb : fireballs) {
        if (!fb.active) continue;

        // Fiery smoke trail behind fireball
        Vector2 trailPos = {fb.position.x - fb.velocity.x * 0.04f, fb.position.y - fb.velocity.y * 0.04f};
        DrawCircleV(trailPos, fb.radius * 0.85f, Color{220, 60, 20, 140});

        // Fireball outer corona
        DrawCircleV(fb.position, fb.radius, Color{240, 90, 20, 230});
        // Fireball inner core
        DrawCircleV(fb.position, fb.radius * 0.55f, Color{255, 235, 90, 255});
        DrawCircleV(fb.position, fb.radius * 0.25f, WHITE);
    }
}

void RenderManager::RenderDamageIndicators() {
    const auto& numbers = DamageSystem::Instance().GetNumbers();

    for (const auto& dn : numbers) {
        float alpha = dn.timer / dn.maxTimer;
        Color textColor = Fade(dn.color, alpha);
        Color shadowColor = Fade(BLACK, alpha * 0.8f);

        int fontSize = dn.isCrit ? 22 : 16;
        const char* text = dn.isCrit ? TextFormat("%d CRIT!", dn.amount) : TextFormat("%d", dn.amount);

        int posX = (int)dn.position.x;
        int posY = (int)(dn.position.y + dn.offsetY);

        // Shadow / Outline for readability
        DrawText(text, posX + 1, posY + 1, fontSize, shadowColor);
        DrawText(text, posX, posY, fontSize, textColor);
    }
}

void RenderManager::RenderAfterimages(const Character& character, const AssetManager& assetManager) {
    const auto& afterimages = character.GetAfterimages();
    Texture2D femaleTex = assetManager.GetFemaleTexture();
    Texture2D maleTex = assetManager.GetMaleTexture();

    for (const auto& ghost : afterimages) {
        Texture2D tex = (ghost.gender == CharacterGender::Female) ? femaleTex : maleTex;
        if (tex.id > 0) {
            float spriteW = (float)tex.width;
            float spriteH = (float)tex.height;

            float targetH = 125.0f;
            float targetW = (spriteW / spriteH) * targetH;

            Rectangle src = {0.0f, 0.0f, spriteW * ghost.facingX, spriteH};
            Rectangle dest = {ghost.position.x, ghost.position.y - (targetH * 0.42f) + 10.0f, targetW, targetH};
            Vector2 origin = {targetW * 0.5f, targetH * 0.5f};

            Color ghostColor = Fade(Color{140, 200, 255, 255}, ghost.alpha * 0.55f);
            DrawTexturePro(tex, src, dest, origin, 0.0f, ghostColor);
        }
    }
}

void RenderManager::RenderParticles(const Character& character) {
    const auto& particles = character.GetParticles();
    for (const auto& p : particles) {
        if (!p.active) continue;
        float alphaRatio = p.life / p.maxLife;
        Color col = Fade(p.color, alphaRatio);
        DrawCircleV(p.position, p.size * alphaRatio, col);
    }
}

void RenderManager::RenderArrows(const Character& character) {
    const auto& arrows = character.GetArrows();

    for (const auto& arrow : arrows) {
        if (!arrow.active) continue;

        Vector2 pos = arrow.position;
        float spd = std::sqrt(arrow.velocity.x * arrow.velocity.x + arrow.velocity.y * arrow.velocity.y);
        Vector2 dir = (spd > 0.001f) ? Vector2{arrow.velocity.x / spd, arrow.velocity.y / spd} : Vector2{1.0f, 0.0f};
        Vector2 norm = {-dir.y, dir.x}; // Perpendicular vector for 8-way rotation

        // 1. Arrow Speed Trail (Golden Wind streak)
        Vector2 trailStart = {pos.x - dir.x * 42.0f, pos.y - dir.y * 42.0f};
        DrawLineEx(trailStart, pos, 2.5f, Color{255, 235, 150, 160});
        DrawLineEx(Vector2{pos.x - dir.x * 22.0f, pos.y - dir.y * 22.0f}, pos, 1.5f, Color{255, 255, 220, 220});

        // 2. Arrow Shaft (Heavy wood)
        Vector2 shaftStart = {pos.x - dir.x * 28.0f, pos.y - dir.y * 28.0f};
        DrawLineEx(shaftStart, pos, 3.0f, Color{130, 85, 45, 255});

        // 3. Arrow Fletchings (Hunter Feathers at the back)
        Vector2 featherBack = {pos.x - dir.x * 24.0f, pos.y - dir.y * 24.0f};
        DrawLineEx(featherBack, Vector2{featherBack.x - dir.x * 4.0f + norm.x * 5.0f, featherBack.y - dir.y * 4.0f + norm.y * 5.0f}, 2.0f, Color{220, 50, 50, 255});
        DrawLineEx(featherBack, Vector2{featherBack.x - dir.x * 4.0f - norm.x * 5.0f, featherBack.y - dir.y * 4.0f - norm.y * 5.0f}, 2.0f, Color{220, 50, 50, 255});

        // 4. Arrow Head (Carved Wyvern Bone Tip)
        Vector2 tipEnd = {pos.x + dir.x * 6.0f, pos.y + dir.y * 6.0f};
        Vector2 tipTop = {pos.x - dir.x * 4.0f + norm.x * 5.0f, pos.y - dir.y * 4.0f + norm.y * 5.0f};
        Vector2 tipBottom = {pos.x - dir.x * 4.0f - norm.x * 5.0f, pos.y - dir.y * 4.0f - norm.y * 5.0f};
        DrawTriangle(tipEnd, tipTop, tipBottom, Color{235, 230, 215, 255});
        DrawCircleV(tipEnd, 2.5f, Color{255, 255, 240, 255});
    }
}

void RenderManager::RenderSlashArc(const Character& character) {
    const SlashArc& arc = character.GetSlashArc();
    if (!arc.active || arc.progress <= 0.001f) return;

    Vector2 center = arc.origin;
    float rOuter = arc.radius;       // Extended radius: 150.0f
    float rInner = arc.radius - 38.0f; // Broad, powerful blade wave

    float currentAngle = arc.startAngle + (arc.endAngle - arc.startAngle) * arc.progress;
    float trailLength = (arc.endAngle > arc.startAngle) ? 58.0f : -58.0f;
    float trailStartAngle = currentAngle - trailLength;

    if (arc.endAngle > arc.startAngle) {
        if (trailStartAngle < arc.startAngle) trailStartAngle = arc.startAngle;
    } else {
        if (trailStartAngle > arc.startAngle) trailStartAngle = arc.startAngle;
    }

    const int segments = 32;
    float angleStep = (currentAngle - trailStartAngle) / (float)segments;

    for (int i = 0; i < segments; i++) {
        float a1 = (trailStartAngle + (float)i * angleStep) * DEG2RAD;
        float a2 = (trailStartAngle + (float)(i + 1) * angleStep) * DEG2RAD;

        float fadeProg = (float)(i + 1) / (float)segments;
        Color cOuter = Fade(Color{90, 190, 255, 255}, fadeProg * 0.85f);
        Color cCore  = Fade(Color{235, 248, 255, 255}, fadeProg * 0.98f);
        Color cInner = Fade(Color{40, 110, 230, 255}, fadeProg * 0.45f);

        Vector2 p1Outer = {center.x + std::cos(a1) * rOuter, center.y + std::sin(a1) * rOuter};
        Vector2 p2Outer = {center.x + std::cos(a2) * rOuter, center.y + std::sin(a2) * rOuter};
        Vector2 p1Mid   = {center.x + std::cos(a1) * (rOuter - 15.0f), center.y + std::sin(a1) * (rOuter - 15.0f)};
        Vector2 p2Mid   = {center.x + std::cos(a2) * (rOuter - 15.0f), center.y + std::sin(a2) * (rOuter - 15.0f)};
        Vector2 p1Inner = {center.x + std::cos(a1) * rInner, center.y + std::sin(a1) * rInner};
        Vector2 p2Inner = {center.x + std::cos(a2) * rInner, center.y + std::sin(a2) * rInner};

        // Outer electric blue aura band
        DrawTriangle(p1Outer, p2Outer, p2Mid, cOuter);
        DrawTriangle(p1Outer, p2Mid, p1Mid, cOuter);

        // Core razor-sharp blade light
        DrawTriangle(p1Mid, p2Mid, p2Inner, cCore);
        DrawTriangle(p1Mid, p2Inner, p1Inner, cInner);
    }

    // Leading edge blade flash spark (Tip of Katana swing)
    float leadRad = currentAngle * DEG2RAD;
    Vector2 tipPos = {center.x + std::cos(leadRad) * (rOuter - 8.0f), center.y + std::sin(leadRad) * (rOuter - 8.0f)};
    DrawCircleV(tipPos, 9.0f, Color{255, 255, 255, 255});
    DrawCircleV(tipPos, 16.0f, Color{160, 220, 255, 200});
}

void RenderManager::RenderUIPass(const Character& character, const DragonBoss& boss, const VirtualJoystick& joystick) {
    // 1. Hunter HUD with Functional HP (Top-Left)
    RenderHUD(character);

    // 2. Dragon Boss Epic Health Bar (Top Center)
    RenderBossHUD(boss);

    // 3. Virtual Joystick (Bottom-Left)
    RenderVirtualJoystickUI(joystick);

    // 4. Character Switcher Button & Profile (Top-Right)
    Rectangle btnRect = GetCharacterSwitchButtonRect();
    bool isFemale = (character.GetGender() == CharacterGender::Female);

    Color btnBg = isFemale ? Color{160, 60, 90, 220} : Color{45, 85, 150, 220};
    Color btnBorder = isFemale ? Color{240, 160, 190, 255} : Color{140, 190, 240, 255};

    DrawRectangleRounded(btnRect, 0.25f, 6, btnBg);
    DrawRectangleRoundedLinesEx(btnRect, 0.25f, 6, 2.0f, btnBorder);

    DrawText("CHANGE HUNTER", (int)btnRect.x + 40, (int)btnRect.y + 12, 16, WHITE);
    DrawText("[TAP / PRESS 'C']", (int)btnRect.x + 48, (int)btnRect.y + 34, 12, Color{220, 220, 220, 200});

    // Hunter Profile Card
    Rectangle cardRect = {btnRect.x - 20.0f, btnRect.y + 70.0f, 250.0f, 95.0f};
    DrawRectangleRounded(cardRect, 0.2f, 6, Color{20, 22, 28, 200});
    DrawRectangleRoundedLinesEx(cardRect, 0.2f, 6, 1.5f, Color{80, 85, 100, 150});

    if (isFemale) {
        DrawText("FEMALE HUNTER", (int)cardRect.x + 16, (int)cardRect.y + 10, 16, Color{250, 180, 200, 255});
        DrawText("Armor: Crop-top Armor (No Helmet)", (int)cardRect.x + 16, (int)cardRect.y + 32, 12, Color{210, 210, 210, 255});
        DrawText("Weapon: Great Hunter Bow (East)", (int)cardRect.x + 16, (int)cardRect.y + 50, 12, Color{255, 210, 110, 255});
        DrawText("Action: Shoot Arrow [J / Tap]", (int)cardRect.x + 16, (int)cardRect.y + 68, 12, Color{120, 230, 200, 255});
    } else {
        DrawText("MALE HUNTER", (int)cardRect.x + 16, (int)cardRect.y + 10, 16, Color{160, 200, 255, 255});
        DrawText("Armor: Ronin Plate Coat (No Helmet)", (int)cardRect.x + 16, (int)cardRect.y + 32, 12, Color{210, 210, 210, 255});
        DrawText("Weapon: Steel Katana (East)", (int)cardRect.x + 16, (int)cardRect.y + 50, 12, Color{255, 210, 110, 255});
        DrawText("Action: Arc Slash [J / Tap]", (int)cardRect.x + 16, (int)cardRect.y + 68, 12, Color{140, 200, 255, 255});
    }

    // 5. Interactive Mobile Action Buttons (Bottom-Right)
    Vector2 btnAttackCenter = {1170.0f, 590.0f};
    Vector2 btnDodgeCenter = {1060.0f, 630.0f};
    bool isAttacking = character.IsAttacking();
    bool isEvading = character.IsEvading();

    // ATTACK BUTTON
    Color atkBg = isAttacking ? Color{235, 90, 60, 255} : Color{180, 55, 40, 200};
    Color atkBorder = isAttacking ? Color{255, 240, 200, 255} : Color{255, 170, 150, 230};
    DrawCircleV(btnAttackCenter, 46.0f, atkBg);
    DrawCircleLines((int)btnAttackCenter.x, (int)btnAttackCenter.y, 46.0f, atkBorder);
    DrawCircleLines((int)btnAttackCenter.x, (int)btnAttackCenter.y, 42.0f, Color{atkBorder.r, atkBorder.g, atkBorder.b, 80});

    const char* atkText = isFemale ? "SHOOT" : "SLASH";
    DrawText(atkText, (int)btnAttackCenter.x - 22, (int)btnAttackCenter.y - 12, 15, WHITE);
    DrawText("[J / Click]", (int)btnAttackCenter.x - 26, (int)btnAttackCenter.y + 8, 10, Color{240, 220, 220, 220});

    // EVADE BUTTON with 3s reload cooldown indicator
    float evadeCd = character.GetEvadeCooldown();
    float evadeMaxCd = character.GetEvadeMaxCooldown();
    bool onCooldown = (evadeCd > 0.0f && !isEvading);

    Color dodgeBg = isEvading ? Color{70, 160, 240, 255} : (onCooldown ? Color{30, 40, 55, 220} : Color{40, 105, 160, 200});
    Color dodgeBorder = isEvading ? Color{220, 245, 255, 255} : (onCooldown ? Color{80, 90, 110, 200} : Color{150, 200, 250, 230});
    DrawCircleV(btnDodgeCenter, 38.0f, dodgeBg);
    DrawCircleLines((int)btnDodgeCenter.x, (int)btnDodgeCenter.y, 38.0f, dodgeBorder);

    if (onCooldown) {
        // Radial cooldown sweep: clockwise arc fills as cooldown progresses
        float cdRatio = evadeCd / evadeMaxCd; // 1.0 = just used, 0.0 = ready
        float readyRatio = 1.0f - cdRatio;
        int arcSegments = 32;

        // Draw filled arc portion (ready part) in bright blue
        for (int seg = 0; seg < (int)(readyRatio * arcSegments); seg++) {
            float a1 = (-90.0f + (float)seg * (360.0f / (float)arcSegments)) * DEG2RAD;
            float a2 = (-90.0f + (float)(seg + 1) * (360.0f / (float)arcSegments)) * DEG2RAD;

            Vector2 center = btnDodgeCenter;
            Vector2 p1 = {center.x + std::cos(a1) * 36.0f, center.y + std::sin(a1) * 36.0f};
            Vector2 p2 = {center.x + std::cos(a2) * 36.0f, center.y + std::sin(a2) * 36.0f};

            DrawTriangle(center, p1, p2, Color{60, 150, 220, 120});
        }

        // Cooldown timer text
        DrawText(TextFormat("%.1f", evadeCd), (int)btnDodgeCenter.x - 10, (int)btnDodgeCenter.y - 7, 15, Color{200, 220, 255, 255});
    } else {
        DrawText("EVADE", (int)btnDodgeCenter.x - 22, (int)btnDodgeCenter.y - 11, 14, WHITE);
        DrawText("[Space]", (int)btnDodgeCenter.x - 18, (int)btnDodgeCenter.y + 7, 10, Color{220, 235, 255, 220});
    }

    // 6. Dodge Cooldown bar under the buttons
    RenderDodgeCooldown(character);

    // 7. System Controls Hint (Bottom Center)
    DrawText("Touch / Drag Joystick | Attack: [J] | Evade: [Space] (3s CD) | Switch: [C] | Restart: [R]", 220, 688, 14, Color{180, 190, 205, 180});
}

void RenderManager::RenderHUD(const Character& character) {
    DrawRectangleRounded(Rectangle{20.0f, 20.0f, 320.0f, 85.0f}, 0.2f, 6, Color{18, 20, 24, 210});
    DrawRectangleRoundedLinesEx(Rectangle{20.0f, 20.0f, 320.0f, 85.0f}, 0.2f, 6, 1.5f, Color{90, 85, 75, 180});

    DrawText("HUNTER", 36, 28, 14, Color{220, 200, 150, 255});
    DrawText("ZONE 1 : ANCIENT ARENA", 150, 28, 12, Color{140, 170, 160, 220});

    // FUNCTIONAL HEALTH BAR
    float hpRatio = (float)character.GetCurrentHp() / (float)character.GetMaxHp();
    if (hpRatio < 0.0f) hpRatio = 0.0f;
    if (hpRatio > 1.0f) hpRatio = 1.0f;

    DrawRectangle(36, 48, 280, 14, Color{60, 20, 20, 230});
    DrawRectangle(36, 48, (int)(280.0f * hpRatio), 14, Color{45, 195, 60, 255});
    DrawRectangleLines(36, 48, 280, 14, Color{180, 170, 140, 200});
    DrawText(TextFormat("HP %d/%d", character.GetCurrentHp(), character.GetMaxHp()), 38, 50, 10, WHITE);

    // Stamina Bar
    DrawRectangle(36, 68, 230, 12, Color{60, 50, 15, 230});
    DrawRectangle(36, 68, 210, 12, Color{225, 190, 40, 255});
    DrawRectangleLines(36, 68, 230, 12, Color{180, 170, 140, 200});
    DrawText("ST", 38, 70, 9, WHITE);

    Vector2 pos = character.GetPosition();
    const char* status = character.IsDead() ? "FAINTED!" : (character.IsEvading() ? "EVADING (I-FRAMES)" : (character.IsAttacking() ? "ATTACKING" : (character.IsMoving() ? "WALKING" : "IDLE")));
    DrawText(TextFormat("Pos: (%.0f, %.0f) | Stance: EAST | State: %s", pos.x, pos.y, status), 36, 112, 12, Color{160, 175, 190, 190});
}

void RenderManager::RenderBossHUD(const DragonBoss& boss) {
    if (boss.IsDead()) return;

    // Center boss health bar panel at top of screen
    float barWidth = 460.0f;
    float startX = ((float)m_virtualWidth - barWidth) * 0.5f;
    float startY = 18.0f;

    DrawRectangleRounded(Rectangle{startX - 10.0f, startY - 4.0f, barWidth + 20.0f, 44.0f}, 0.25f, 6, Color{20, 16, 22, 220});
    DrawRectangleRoundedLinesEx(Rectangle{startX - 10.0f, startY - 4.0f, barWidth + 20.0f, 44.0f}, 0.25f, 6, 1.5f, Color{195, 150, 60, 220});

    // Dragon Boss Crest / Name
    DrawText("RATHALOS - FIRE WYVERN [BOSS]", (int)startX + 4, (int)startY, 13, Color{255, 210, 80, 255});
    DrawText(TextFormat("%d / %d", boss.GetCurrentHp(), boss.GetMaxHp()), (int)startX + (int)barWidth - 100, (int)startY, 12, Color{220, 220, 220, 220});

    // Boss Health Bar Gauge
    float bossHpRatio = (float)boss.GetCurrentHp() / (float)boss.GetMaxHp();
    if (bossHpRatio < 0.0f) bossHpRatio = 0.0f;
    if (bossHpRatio > 1.0f) bossHpRatio = 1.0f;

    DrawRectangle((int)startX, (int)startY + 18, (int)barWidth, 14, Color{60, 20, 20, 240});
    DrawRectangle((int)startX, (int)startY + 18, (int)(barWidth * bossHpRatio), 14, Color{220, 45, 35, 255});
    DrawRectangleLines((int)startX, (int)startY + 18, (int)barWidth, 14, Color{215, 175, 80, 220});
}

void RenderManager::RenderVirtualJoystickUI(const VirtualJoystick& joystick) {
    Vector2 center = joystick.GetCenter();
    Vector2 knob = joystick.GetKnobPos();
    float baseRadius = joystick.GetBaseRadius();
    float knobRadius = joystick.GetKnobRadius();
    bool active = joystick.IsActive();

    Color baseBg = active ? Color{35, 45, 65, 150} : Color{25, 30, 42, 100};
    Color baseLine = active ? Color{100, 180, 240, 220} : Color{120, 130, 150, 130};

    DrawCircleV(center, baseRadius, baseBg);
    DrawCircleLines((int)center.x, (int)center.y, baseRadius, baseLine);
    DrawCircleLines((int)center.x, (int)center.y, baseRadius - 8.0f, Color{baseLine.r, baseLine.g, baseLine.b, 60});

    DrawLine((int)(center.x - baseRadius + 8), (int)center.y, (int)(center.x - baseRadius + 22), (int)center.y, baseLine);
    DrawLine((int)(center.x + baseRadius - 22), (int)center.y, (int)(center.x + baseRadius - 8), (int)center.y, baseLine);
    DrawLine((int)center.x, (int)(center.y - baseRadius + 8), (int)center.x, (int)(center.y - baseRadius + 22), baseLine);
    DrawLine((int)center.x, (int)(center.y + baseRadius - 22), (int)center.x, (int)(center.y + baseRadius - 8), baseLine);

    if (active) {
        DrawLineEx(center, knob, 3.0f, Color{120, 200, 255, 180});
    }

    Color knobBg = active ? Color{70, 140, 220, 230} : Color{50, 60, 75, 190};
    Color knobLine = active ? Color{200, 235, 255, 255} : Color{150, 160, 180, 200};

    DrawCircleV(knob, knobRadius, knobBg);
    DrawCircleLines((int)knob.x, (int)knob.y, knobRadius, knobLine);
    DrawCircleV(knob, knobRadius * 0.4f, knobLine);
}

void RenderManager::RenderDodgeCooldown(const Character& character) {
    float evadeCd = character.GetEvadeCooldown();
    float evadeMaxCd = character.GetEvadeMaxCooldown();

    if (evadeCd <= 0.0f) return;

    // Horizontal cooldown bar below action buttons
    float barX = 1010.0f;
    float barY = 680.0f;
    float barW = 210.0f;
    float barH = 8.0f;
    float readyRatio = 1.0f - (evadeCd / evadeMaxCd);

    DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, Color{25, 30, 40, 180});
    DrawRectangle((int)barX, (int)barY, (int)(barW * readyRatio), (int)barH, Color{60, 150, 220, 230});
    DrawRectangleLines((int)barX, (int)barY, (int)barW, (int)barH, Color{100, 130, 170, 180});

    DrawText(TextFormat("EVADE CD: %.1fs", evadeCd), (int)barX, (int)barY - 14, 11, Color{170, 200, 230, 220});
}

void RenderManager::RenderGameOverScreen(GameState state) {
    // Full screen dark overlay
    DrawRectangle(0, 0, m_virtualWidth, m_virtualHeight, Color{0, 0, 0, 180});

    if (state == GameState::Victory) {
        // Victory Banner
        const char* title = "QUEST COMPLETE!";
        int titleW = MeasureText(title, 48);
        int titleX = (m_virtualWidth - titleW) / 2;

        // Glow behind text
        DrawRectangleRounded(Rectangle{(float)titleX - 30.0f, 230.0f, (float)titleW + 60.0f, 70.0f}, 0.3f, 6, Color{25, 60, 25, 220});
        DrawRectangleRoundedLinesEx(Rectangle{(float)titleX - 30.0f, 230.0f, (float)titleW + 60.0f, 70.0f}, 0.3f, 6, 2.0f, Color{80, 220, 80, 255});

        DrawText(title, titleX, 242, 48, Color{80, 255, 80, 255});

        const char* sub = "The Dragon Wyvern Has Been Slain!";
        int subW = MeasureText(sub, 22);
        DrawText(sub, (m_virtualWidth - subW) / 2, 320, 22, Color{200, 255, 200, 255});

        // Wyvern Crest Icon
        DrawCircleV(Vector2{(float)(m_virtualWidth / 2), 380.0f}, 28.0f, Color{200, 160, 50, 255});
        DrawCircleV(Vector2{(float)(m_virtualWidth / 2), 380.0f}, 20.0f, Color{50, 50, 45, 255});
        DrawText("W", m_virtualWidth / 2 - 7, 372, 18, Color{200, 160, 50, 255});

    } else {
        // Defeat Banner
        const char* title = "QUEST FAILED";
        int titleW = MeasureText(title, 48);
        int titleX = (m_virtualWidth - titleW) / 2;

        DrawRectangleRounded(Rectangle{(float)titleX - 30.0f, 230.0f, (float)titleW + 60.0f, 70.0f}, 0.3f, 6, Color{60, 18, 18, 220});
        DrawRectangleRoundedLinesEx(Rectangle{(float)titleX - 30.0f, 230.0f, (float)titleW + 60.0f, 70.0f}, 0.3f, 6, 2.0f, Color{220, 60, 50, 255});

        DrawText(title, titleX, 242, 48, Color{255, 70, 60, 255});

        const char* sub = "You Have Fainted...";
        int subW = MeasureText(sub, 22);
        DrawText(sub, (m_virtualWidth - subW) / 2, 320, 22, Color{255, 180, 170, 255});

        // Skull Icon
        DrawCircleV(Vector2{(float)(m_virtualWidth / 2), 380.0f}, 24.0f, Color{200, 50, 40, 255});
        DrawText("X", m_virtualWidth / 2 - 6, 372, 18, Color{255, 220, 200, 255});
    }

    // Restart Button
    Rectangle restartBtn = GetRestartButtonRect();
    DrawRectangleRounded(restartBtn, 0.3f, 6, Color{45, 55, 80, 240});
    DrawRectangleRoundedLinesEx(restartBtn, 0.3f, 6, 2.0f, Color{160, 180, 220, 255});

    const char* restartText = "RESTART HUNT [R / TAP]";
    int rtW = MeasureText(restartText, 22);
    DrawText(restartText, (int)restartBtn.x + ((int)restartBtn.width - rtW) / 2, (int)restartBtn.y + 18, 22, WHITE);
}

void RenderManager::RenderAimIndicator(const Character& character) {
    if (character.IsDead()) return;

    Vector2 playerPos = character.GetPosition();
    Vector2 aimDir = character.GetAimDirection();
    float dist = 68.0f;

    Vector2 playerCenter = {playerPos.x, playerPos.y - 18.0f};
    Vector2 orbPos = {playerCenter.x + aimDir.x * dist, playerCenter.y + aimDir.y * dist};

    // 1. Subtle glowing aim projection line from hunter to the orb
    DrawLineEx(playerCenter, orbPos, 1.5f, Color{80, 180, 255, 90});

    // 2. Aim guide notch along the vector
    Vector2 midPoint = {playerCenter.x + aimDir.x * (dist * 0.55f), playerCenter.y + aimDir.y * (dist * 0.55f)};
    DrawCircleV(midPoint, 2.0f, Color{100, 200, 255, 140});

    // 3. Small Blue Aim Indicator Orb
    static float pulseTimer = 0.0f;
    pulseTimer += GetFrameTime();
    float pulse = (std::sin(pulseTimer * 10.0f) + 1.0f) * 0.5f;

    // Outer soft blue halo
    DrawCircleV(orbPos, 9.5f + pulse * 2.0f, Color{30, 140, 255, (unsigned char)(70 + pulse * 40)});
    // Middle cyan aura ring
    DrawCircleV(orbPos, 6.0f, Color{70, 195, 255, 200});
    DrawCircleLines((int)orbPos.x, (int)orbPos.y, 6.0f, Color{180, 240, 255, 240});
    // Inner bright white-blue core
    DrawCircleV(orbPos, 3.0f, Color{240, 250, 255, 255});
}
