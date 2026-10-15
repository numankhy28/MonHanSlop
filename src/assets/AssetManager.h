#pragma once
#include "raylib.h"

class AssetManager {
public:
    AssetManager();
    ~AssetManager();

    // Loads textures from disk or generates & exports them if missing
    void LoadAssets();
    void UnloadAssets();

    Texture2D GetFemaleTexture() const { return m_femaleTexture; }
    Texture2D GetMaleTexture() const { return m_maleTexture; }
    Texture2D GetMapTexture() const { return m_mapTexture; }
    Texture2D GetDragonTexture() const { return m_dragonTexture; }

private:
    Texture2D m_femaleTexture;
    Texture2D m_maleTexture;
    Texture2D m_mapTexture;
    Texture2D m_dragonTexture;
    bool m_isLoaded;

    // Procedural placeholder generators (saves high-detail East-facing sprites as PNGs)
    Image GenerateFemaleHunterSprite();
    Image GenerateMaleHunterSprite();
    Image GenerateMapArenaPlaceholder();
    Image GenerateDragonBossSprite();
};
