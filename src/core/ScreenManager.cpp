#include "ScreenManager.h"
#include <cmath>

ScreenManager::ScreenManager(int virtualWidth, int virtualHeight)
    : m_virtualWidth(virtualWidth),
      m_virtualHeight(virtualHeight),
      m_scale(1.0f),
      m_offset{0.0f, 0.0f} {
    Update();
}

void ScreenManager::Update() {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    if (screenW <= 0 || screenH <= 0) return;

    // Calculate maximum uniform scale that fits virtual resolution inside current screen
    float scaleX = (float)screenW / (float)m_virtualWidth;
    float scaleY = (float)screenH / (float)m_virtualHeight;
    m_scale = fminf(scaleX, scaleY);

    // Center virtual viewport inside physical screen (letterboxing / pillarboxing)
    m_offset.x = (screenW - ((float)m_virtualWidth * m_scale)) * 0.5f;
    m_offset.y = (screenH - ((float)m_virtualHeight * m_scale)) * 0.5f;
}

Vector2 ScreenManager::ScreenToVirtual(Vector2 screenPos) const {
    if (m_scale <= 0.0f) return screenPos;
    return Vector2{
        (screenPos.x - m_offset.x) / m_scale,
        (screenPos.y - m_offset.y) / m_scale
    };
}

Vector2 ScreenManager::VirtualToScreen(Vector2 virtualPos) const {
    return Vector2{
        virtualPos.x * m_scale + m_offset.x,
        virtualPos.y * m_scale + m_offset.y
    };
}

Vector2 ScreenManager::GetVirtualMousePosition() const {
    Vector2 rawMouse = GetMousePosition();
    return ScreenToVirtual(rawMouse);
}

bool ScreenManager::IsInsideViewport(Vector2 screenPos) const {
    float vpW = (float)m_virtualWidth * m_scale;
    float vpH = (float)m_virtualHeight * m_scale;
    return (screenPos.x >= m_offset.x && screenPos.x <= m_offset.x + vpW &&
            screenPos.y >= m_offset.y && screenPos.y <= m_offset.y + vpH);
}

Rectangle ScreenManager::GetSourceRect() const {
    // Negative height in raylib source rect flips the OpenGL render texture right-side up
    return Rectangle{0.0f, 0.0f, (float)m_virtualWidth, -(float)m_virtualHeight};
}

Rectangle ScreenManager::GetDestRect() const {
    return Rectangle{m_offset.x, m_offset.y, (float)m_virtualWidth * m_scale, (float)m_virtualHeight * m_scale};
}
