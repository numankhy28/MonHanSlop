#include "raylib.h"
#include "core/ScreenManager.h"
#include "assets/AssetManager.h"
#include "world/GameMap.h"
#include "entities/Character.h"
#include "entities/DragonBoss.h"
#include "entities/DamageIndicator.h"
#include "input/VirtualJoystick.h"
#include "rendering/RenderManager.h"

int main() {
    // Enable resizable window so mobile landscape aspect ratios and resolutions can be simulated
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    // Initial window size (16:9 mobile landscape preview)
    const int initialWindowWidth = 1280;
    const int initialWindowHeight = 720;
    InitWindow(initialWindowWidth, initialWindowHeight, "Monster Hunter 2D Mobile Prototype [Raylib + C++]");
    SetTargetFPS(60);

    // 1. Mobile Screen Auto-Scale Manager (Target Virtual Resolution: 1280x720)
    ScreenManager screenManager(1280, 720);

    // 2. Asset Manager (Generates high quality placeholder sprites on first run & loads textures)
    AssetManager assetManager;
    assetManager.LoadAssets();

    // 3. World & Hunting Arena Map
    GameMap gameMap(2400.0f, 1600.0f);

    // 4. Player Character (Default: Female Hunter with Great Bow, facing East)
    Character player(Vector2{1100.0f, 800.0f}, CharacterGender::Female);

    // 5. Dragon Wyvern Boss (Spawns across the arena)
    DragonBoss boss(Vector2{1550.0f, 750.0f});

    // 6. Mobile Virtual Joystick (Tailored for mobile landscape thumb controls)
    VirtualJoystick joystick(Vector2{180.0f, 540.0f}, 85.0f, 38.0f);

    // 7. Centralized Render Pipeline Manager
    RenderManager renderManager(1280, 720);

    // 8. Game State Management (Playing, Victory, Defeat)
    GameState gameState = GameState::Playing;

    // =========================================================================
    // MAIN GAME LOOP
    // =========================================================================
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f; // Clamp delta time to avoid physics tunneling

        // 1. Update virtual screen scaling for any window resize / orientation change
        screenManager.Update();

        // 2. Coordinate conversions
        Vector2 virtualMousePos = screenManager.GetVirtualMousePosition();
        bool isMouseDown = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool isMouseClick = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        int touchCount = GetTouchPointCount();
        bool isTouchDown = (touchCount > 0);
        Vector2 virtualTouchPos = isTouchDown ? screenManager.ScreenToVirtual(GetTouchPosition(0)) : virtualMousePos;

        // 3. Game Flow Logic based on GameState
        if (gameState == GameState::Playing) {
            // Check victory / defeat conditions
            if (boss.IsDead()) {
                gameState = GameState::Victory;
            } else if (player.IsDead()) {
                gameState = GameState::Defeat;
            }

            // Multi-touch & UI hit detection for mobile buttons
            Rectangle switchBtn = renderManager.GetCharacterSwitchButtonRect();
            Rectangle attackBtn = renderManager.GetAttackButtonRect();
            Rectangle dodgeBtn  = renderManager.GetDodgeButtonRect();

            bool triggerSwitch = IsKeyPressed(KEY_C);
            bool triggerAttack = IsKeyPressed(KEY_J) || IsKeyPressed(KEY_ENTER);
            bool triggerEvade  = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_K);

            // Check mouse click on UI buttons
            if (isMouseClick) {
                if (CheckCollisionPointRec(virtualMousePos, switchBtn)) triggerSwitch = true;
                if (CheckCollisionPointRec(virtualMousePos, attackBtn)) triggerAttack = true;
                if (CheckCollisionPointRec(virtualMousePos, dodgeBtn))  triggerEvade  = true;
            }

            // Check multi-touch points on mobile screen
            for (int i = 0; i < touchCount; i++) {
                Vector2 tPos = screenManager.ScreenToVirtual(GetTouchPosition(i));
                if (CheckCollisionPointRec(tPos, switchBtn)) triggerSwitch = true;
                if (CheckCollisionPointRec(tPos, attackBtn)) triggerAttack = true;
                if (CheckCollisionPointRec(tPos, dodgeBtn))  triggerEvade  = true;
            }

            // Update Virtual Joystick (linear algebra normalization ensures no diagonal speedup)
            joystick.Update(virtualMousePos, isMouseDown, isTouchDown, virtualTouchPos);
            Vector2 inputDir = joystick.GetDirection();

            // Combat & Action Triggers
            if (triggerSwitch) {
                player.ToggleGender();
            }
            if (triggerEvade) {
                player.TriggerEvade(inputDir); // 3-second reload cooldown enforced
            }
            if (triggerAttack) {
                player.TriggerAttack();
            }

            // Update Game Entities
            player.Update(inputDir, dt, gameMap);
            boss.Update(dt, player, gameMap);
            gameMap.Update(dt);

        } else {
            // Game Over / Victory Screen: Listen for Restart
            Rectangle restartBtn = renderManager.GetRestartButtonRect();
            bool triggerRestart = IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);

            if (isMouseClick && CheckCollisionPointRec(virtualMousePos, restartBtn)) {
                triggerRestart = true;
            }
            for (int i = 0; i < touchCount; i++) {
                Vector2 tPos = screenManager.ScreenToVirtual(GetTouchPosition(i));
                if (CheckCollisionPointRec(tPos, restartBtn)) {
                    triggerRestart = true;
                }
            }

            if (triggerRestart) {
                player.Reset(Vector2{1100.0f, 800.0f});
                boss.Reset(Vector2{1550.0f, 750.0f});
                gameState = GameState::Playing;
            }
        }

        // 4. Update Floating Damage Indicators
        DamageSystem::Instance().Update(dt);

        // 5. Camera Follows Player
        renderManager.UpdateCamera(player.GetPosition(), gameMap.GetWidth(), gameMap.GetHeight(), dt);

        // 6. CENTRALIZED RENDERING: Single render function call managing the entire frame
        renderManager.Render(gameMap, player, boss, joystick, screenManager, assetManager, gameState);
    }

    // Cleanup resources
    assetManager.UnloadAssets();
    CloseWindow();

    return 0;
}
