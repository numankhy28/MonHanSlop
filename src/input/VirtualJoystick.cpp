#include "VirtualJoystick.h"
#include <cmath>

VirtualJoystick::VirtualJoystick(Vector2 defaultCenter, float baseRadius, float knobRadius)
    : m_defaultCenter(defaultCenter),
      m_center(defaultCenter),
      m_knobPos(defaultCenter),
      m_baseRadius(baseRadius),
      m_knobRadius(knobRadius),
      m_direction{0.0f, 0.0f},
      m_isActive(false),
      m_isDragging(false),
      m_activeTouchId(-1) {
}

void VirtualJoystick::Update(Vector2 virtualMousePos, bool isMouseButtonDown, bool isTouchDown, Vector2 virtualTouchPos) {
    // 1. Touch / Mouse pointer input
    Vector2 pointerPos = isTouchDown ? virtualTouchPos : virtualMousePos;
    bool pointerDown = isTouchDown || isMouseButtonDown;

    if (pointerDown) {
        float dx = pointerPos.x - m_center.x;
        float dy = pointerPos.y - m_center.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        // Start drag if clicking/touching inside activation area (e.g. left side of virtual screen or near joystick)
        if (!m_isDragging) {
            // Activate if clicked within 1.5x base radius or left-bottom screen zone
            if (dist <= m_baseRadius * 1.5f || (pointerPos.x < 450.0f && pointerPos.y > 350.0f)) {
                m_isDragging = true;
                // Dynamically re-anchor center for touch comfort if touched inside touch zone
                if (dist > m_baseRadius * 1.5f) {
                    m_center = pointerPos;
                }
            }
        }

        if (m_isDragging) {
            dx = pointerPos.x - m_center.x;
            dy = pointerPos.y - m_center.y;
            dist = std::sqrt(dx * dx + dy * dy);

            if (dist > 0.001f) {
                // Vector normalization using linear algebra
                float nx = dx / dist;
                float ny = dy / dist;

                // Clamp knob to boundary
                float clampedDist = fminf(dist, m_baseRadius);
                m_knobPos.x = m_center.x + nx * clampedDist;
                m_knobPos.y = m_center.y + ny * clampedDist;

                // Deadzone check (5% threshold to avoid jitter)
                float deadzone = m_baseRadius * 0.05f;
                if (clampedDist < deadzone) {
                    m_direction = {0.0f, 0.0f};
                    m_isActive = false;
                } else {
                    // Ratio between 0.0 and 1.0
                    float magnitude = (clampedDist - deadzone) / (m_baseRadius - deadzone);
                    m_direction.x = nx * magnitude;
                    m_direction.y = ny * magnitude;
                    m_isActive = true;
                }
            } else {
                m_knobPos = m_center;
                m_direction = {0.0f, 0.0f};
                m_isActive = false;
            }
        }
    } else {
        // Released
        m_isDragging = false;
        m_center = m_defaultCenter;
        m_knobPos = m_defaultCenter;
        m_direction = {0.0f, 0.0f};
        m_isActive = false;
    }

    // 2. Keyboard fallback for PC testing (WASD / Arrow Keys)
    // We strictly apply vector normalization to eliminate diagonal speedboost:
    // Without normalization: diagonal magnitude = sqrt(1^2 + 1^2) = ~1.414 (41.4% faster)
    // With normalization: diagonal magnitude = 1.0f
    float keyX = 0.0f;
    float keyY = 0.0f;

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) keyX += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  keyX -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  keyY += 1.0f;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    keyY -= 1.0f;

    if (keyX != 0.0f || keyY != 0.0f) {
        float keyLen = std::sqrt(keyX * keyX + keyY * keyY);
        // Linear algebra normalization: v_norm = v / ||v||
        float normX = keyX / keyLen;
        float normY = keyY / keyLen;

        m_direction.x = normX;
        m_direction.y = normY;
        m_isActive = true;

        // Animate knob visually towards direction
        m_knobPos.x = m_center.x + normX * (m_baseRadius * 0.75f);
        m_knobPos.y = m_center.y + normY * (m_baseRadius * 0.75f);
    }
}
