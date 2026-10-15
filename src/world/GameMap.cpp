#include "GameMap.h"
#include <cmath>

GameMap::GameMap(float width, float height)
    : m_width(width), m_height(height) {
}

void GameMap::Update(float dt) {
    // Dynamic world elements (e.g. grass sway, torch flicker) can update here if needed
    (void)dt;
}

Vector2 GameMap::ClampToBounds(Vector2 position, float radius) const {
    float minX = radius + 40.0f;
    float maxX = m_width - radius - 40.0f;
    float minY = radius + 40.0f;
    float maxY = m_height - radius - 40.0f;

    Vector2 clamped = position;
    if (clamped.x < minX) clamped.x = minX;
    if (clamped.x > maxX) clamped.x = maxX;
    if (clamped.y < minY) clamped.y = minY;
    if (clamped.y > maxY) clamped.y = maxY;

    return clamped;
}

void GameMap::Draw(Texture2D mapTexture) const {
    // If map texture is valid, tile it or draw it over the arena
    if (mapTexture.id > 0) {
        Rectangle source = {0.0f, 0.0f, (float)mapTexture.width, (float)mapTexture.height};
        Rectangle dest = {0.0f, 0.0f, m_width, m_height};
        DrawTexturePro(mapTexture, source, dest, {0.0f, 0.0f}, 0.0f, WHITE);
    } else {
        // Fallback procedural hunting arena rendering
        // Forest green base
        DrawRectangle(0, 0, (int)m_width, (int)m_height, Color{38, 68, 42, 255});

        // Grid lines (subtle hunting ground tiles)
        int tileSize = 120;
        for (int x = 0; x < m_width; x += tileSize) {
            DrawLine(x, 0, x, (int)m_height, Color{30, 56, 34, 120});
        }
        for (int y = 0; y < m_height; y += tileSize) {
            DrawLine(0, y, (int)m_width, y, Color{30, 56, 34, 120});
        }

        // Center ancient monster arena ring
        Vector2 center = {m_width * 0.5f, m_height * 0.5f};
        DrawCircleV(center, 420.0f, Color{55, 52, 45, 180});
        DrawCircleLinesV(center, 420.0f, Color{190, 160, 110, 200});
        DrawCircleLinesV(center, 430.0f, Color{140, 110, 70, 150});
        DrawCircleV(center, 250.0f, Color{70, 66, 58, 200});
        DrawCircleLinesV(center, 250.0f, Color{190, 160, 110, 220});
    }

    // Outer boundary fence/stones
    DrawRectangleLinesEx(Rectangle{20.0f, 20.0f, m_width - 40.0f, m_height - 40.0f}, 12.0f, Color{40, 30, 25, 230});
    DrawRectangleLinesEx(Rectangle{32.0f, 32.0f, m_width - 64.0f, m_height - 64.0f}, 3.0f, Color{180, 140, 80, 180});
}
