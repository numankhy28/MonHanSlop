#pragma once
#include "raylib.h"

class VirtualJoystick {
public:
    VirtualJoystick(Vector2 defaultCenter = {180.0f, 540.0f}, float baseRadius = 85.0f, float knobRadius = 38.0f);

    // Updates joystick state using virtual mouse/touch coordinates and keyboard fallback
    void Update(Vector2 virtualMousePos, bool isMouseButtonDown, bool isTouchDown, Vector2 virtualTouchPos);

    // Get normalized direction vector (length is guaranteed to be <= 1.0f)
    Vector2 GetDirection() const { return m_direction; }

    bool IsActive() const { return m_isActive; }
    Vector2 GetCenter() const { return m_center; }
    Vector2 GetKnobPos() const { return m_knobPos; }
    float GetBaseRadius() const { return m_baseRadius; }
    float GetKnobRadius() const { return m_knobRadius; }

private:
    Vector2 m_defaultCenter;
    Vector2 m_center;
    Vector2 m_knobPos;
    float m_baseRadius;
    float m_knobRadius;
    Vector2 m_direction; // Normalized movement vector (length <= 1.0f)
    bool m_isActive;
    bool m_isDragging;
    int m_activeTouchId;
};
