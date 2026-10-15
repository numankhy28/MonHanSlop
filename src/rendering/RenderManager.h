#pragma once
#include "raylib.h"

class GameMap;
class Character;
class DragonBoss;
class VirtualJoystick;
class ScreenManager;
class AssetManager;

enum class GameState {
    Playing,
    Victory,
    Defeat
};

class RenderManager {
public:
    RenderManager(int virtualWidth = 1280, int virtualHeight = 720);
    ~RenderManager();

    // The SINGLE centralized render pipeline for the entire game
    void Render(const GameMap& map,
                const Character& character,
                const DragonBoss& boss,
                const VirtualJoystick& joystick,
                const ScreenManager& screenManager,
                const AssetManager& assetManager,
                GameState gameState);

    // Camera follow configuration
    void UpdateCamera(Vector2 targetPos, float mapWidth, float mapHeight, float dt);

    // UI interactive regions in virtual screen space
    Rectangle GetCharacterSwitchButtonRect() const { return Rectangle{1020.0f, 20.0f, 230.0f, 60.0f}; }
    Rectangle GetAttackButtonRect() const { return Rectangle{1126.0f, 546.0f, 88.0f, 88.0f}; }
    Rectangle GetDodgeButtonRect() const { return Rectangle{1024.0f, 594.0f, 72.0f, 72.0f}; }
    Rectangle GetRestartButtonRect() const { return Rectangle{490.0f, 440.0f, 300.0f, 60.0f}; }

private:
    int m_virtualWidth;
    int m_virtualHeight;
    RenderTexture2D m_targetTexture;
    Camera2D m_camera;

    // Sub-render passes executed strictly within the centralized Render() method
    void RenderWorldPass(const GameMap& map, const Character& character, const DragonBoss& boss, const AssetManager& assetManager);
    void RenderAfterimages(const Character& character, const AssetManager& assetManager);
    void RenderArrows(const Character& character);
    void RenderSlashArc(const Character& character);
    void RenderParticles(const Character& character);
    void RenderAimIndicator(const Character& character);

    // Boss attack visuals
    void RenderBossAttacks(const DragonBoss& boss);

    // Damage numbers
    void RenderDamageIndicators();

    // UI & HUD
    void RenderUIPass(const Character& character, const DragonBoss& boss, const VirtualJoystick& joystick);
    void RenderHUD(const Character& character);
    void RenderBossHUD(const DragonBoss& boss);
    void RenderDodgeCooldown(const Character& character);
    void RenderVirtualJoystickUI(const VirtualJoystick& joystick);

    // Game Over / Victory screens
    void RenderGameOverScreen(GameState state);
};
