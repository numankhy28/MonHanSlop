#pragma once
#include "raylib.h"

class GameMap {
public:
    GameMap(float width = 2400.0f, float height = 1600.0f);

    void Update(float dt);

    // Enforces character stays inside arena bounds
    Vector2 ClampToBounds(Vector2 position, float radius) const;

    float GetWidth() const { return m_width; }
    float GetHeight() const { return m_height; }
    Rectangle GetBounds() const { return Rectangle{0.0f, 0.0f, m_width, m_height}; }

    // Pure draw method - CALLED ONLY by RenderManager during scene pass
    void Draw(Texture2D mapTexture) const;

private:
    float m_width;
    float m_height;
};
