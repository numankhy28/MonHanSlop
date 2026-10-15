#pragma once
#include "raylib.h"
#include <vector>

struct DamageNumber {
    Vector2 position;
    int amount;
    Color color;
    float timer;
    float maxTimer;
    float offsetY;
    float scale;
    bool isCrit;
};

class DamageSystem {
public:
    static DamageSystem& Instance() {
        static DamageSystem instance;
        return instance;
    }

    void Spawn(Vector2 pos, int amount, Color color, bool isCrit = false) {
        DamageNumber dn;
        // Add random slight X jitter so overlapping numbers don't completely hide each other
        float jitterX = (float)((GetRandomValue(-15, 15)));
        dn.position = Vector2{pos.x + jitterX, pos.y - 10.0f};
        dn.amount = amount;
        dn.color = color;
        dn.timer = 0.85f;
        dn.maxTimer = 0.85f;
        dn.offsetY = 0.0f;
        dn.scale = isCrit ? 1.5f : 1.0f;
        dn.isCrit = isCrit;
        m_numbers.push_back(dn);
    }

    void Update(float dt) {
        for (size_t i = 0; i < m_numbers.size(); i++) {
            m_numbers[i].timer -= dt;
            // Float upwards with decelerating velocity
            m_numbers[i].offsetY -= 48.0f * dt;
        }

        // Clean up expired numbers
        for (int i = (int)m_numbers.size() - 1; i >= 0; i--) {
            if (m_numbers[i].timer <= 0.0f) {
                m_numbers.erase(m_numbers.begin() + i);
            }
        }
    }

    const std::vector<DamageNumber>& GetNumbers() const {
        return m_numbers;
    }

private:
    DamageSystem() = default;
    std::vector<DamageNumber> m_numbers;
};
