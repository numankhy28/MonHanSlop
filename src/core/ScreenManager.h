#pragma once
#include "raylib.h"

class ScreenManager {
public:
    ScreenManager(int virtualWidth = 1280, int virtualHeight = 720);

    // Call whenever window size changes or at start of frame
    void Update();

    // Coordinate conversions between physical window/touch screen and virtual game canvas
    Vector2 ScreenToVirtual(Vector2 screenPos) const;
    Vector2 VirtualToScreen(Vector2 virtualPos) const;
    Vector2 GetVirtualMousePosition() const;

    bool IsInsideViewport(Vector2 screenPos) const;

    // Getters
    int GetVirtualWidth() const { return m_virtualWidth; }
    int GetVirtualHeight() const { return m_virtualHeight; }
    float GetScale() const { return m_scale; }
    Vector2 GetOffset() const { return m_offset; }
    Rectangle GetSourceRect() const;
    Rectangle GetDestRect() const;

private:
    int m_virtualWidth;
    int m_virtualHeight;
    float m_scale;
    Vector2 m_offset;
};
