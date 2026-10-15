#include "AssetManager.h"
#include <cmath>
#include <cstdio>

AssetManager::AssetManager()
    : m_femaleTexture{0, 0, 0, 0, 0},
      m_maleTexture{0, 0, 0, 0, 0},
      m_mapTexture{0, 0, 0, 0, 0},
      m_dragonTexture{0, 0, 0, 0, 0},
      m_isLoaded(false) {
}

AssetManager::~AssetManager() {
    UnloadAssets();
}

void AssetManager::LoadAssets() {
    if (m_isLoaded) return;

    const char* femalePath = "assets/female_hunter_east.png";
    const char* malePath = "assets/male_hunter_east.png";
    const char* mapPath = "assets/map_placeholder.png";
    const char* dragonPath = "assets/dragon_boss.png";

    // 1. Female Hunter Sprite
    if (FileExists(femalePath)) {
        m_femaleTexture = LoadTexture(femalePath);
    } else {
        Image img = GenerateFemaleHunterSprite();
        ExportImage(img, femalePath);
        m_femaleTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    SetTextureFilter(m_femaleTexture, TEXTURE_FILTER_BILINEAR);

    // 2. Male Hunter Sprite
    if (FileExists(malePath)) {
        m_maleTexture = LoadTexture(malePath);
    } else {
        Image img = GenerateMaleHunterSprite();
        ExportImage(img, malePath);
        m_maleTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    SetTextureFilter(m_maleTexture, TEXTURE_FILTER_BILINEAR);

    // 3. Map Arena Placeholder
    if (FileExists(mapPath)) {
        m_mapTexture = LoadTexture(mapPath);
    } else {
        Image img = GenerateMapArenaPlaceholder();
        ExportImage(img, mapPath);
        m_mapTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    SetTextureFilter(m_mapTexture, TEXTURE_FILTER_BILINEAR);

    // 4. Dragon Boss Sprite
    if (FileExists(dragonPath)) {
        m_dragonTexture = LoadTexture(dragonPath);
    } else {
        Image img = GenerateDragonBossSprite();
        ExportImage(img, dragonPath);
        m_dragonTexture = LoadTextureFromImage(img);
        UnloadImage(img);
    }
    SetTextureFilter(m_dragonTexture, TEXTURE_FILTER_BILINEAR);

    m_isLoaded = true;
}

void AssetManager::UnloadAssets() {
    if (m_femaleTexture.id > 0) UnloadTexture(m_femaleTexture);
    if (m_maleTexture.id > 0)   UnloadTexture(m_maleTexture);
    if (m_mapTexture.id > 0)    UnloadTexture(m_mapTexture);
    if (m_dragonTexture.id > 0) UnloadTexture(m_dragonTexture);
    m_femaleTexture = {0, 0, 0, 0, 0};
    m_maleTexture = {0, 0, 0, 0, 0};
    m_mapTexture = {0, 0, 0, 0, 0};
    m_dragonTexture = {0, 0, 0, 0, 0};
    m_isLoaded = false;
}

// -------------------------------------------------------------
// Procedural Sprite Generator: Female Hunter (East-facing, Crop-top Armor, Big Bow, No Helmet)
// -------------------------------------------------------------
Image AssetManager::GenerateFemaleHunterSprite() {
    const int W = 128;
    const int H = 128;
    Image img = GenImageColor(W, H, Color{0, 0, 0, 0});

    Color cSkin = {255, 222, 195, 255};
    Color cSkinShadow = {228, 188, 160, 255};
    Color cHair = {235, 175, 75, 255};
    Color cHairDark = {185, 125, 45, 255};
    Color cHairLight = {255, 210, 115, 255};
    Color cArmorDark = {45, 40, 48, 255};
    Color cArmorPlate = {85, 78, 92, 255};
    Color cBronze = {195, 140, 60, 255};
    Color cLeggings = {35, 32, 40, 255};
    Color cBowBone = {215, 205, 185, 255};
    Color cBowWood = {130, 80, 45, 255};
    Color cBowString = {240, 240, 250, 210};
    Color cEyeTeal = {30, 195, 205, 255};

    // --- Quiver on back (West side) ---
    ImageDrawRectangle(&img, 38, 54, 10, 26, cArmorDark);
    ImageDrawRectangle(&img, 36, 46, 4, 10, Color{220, 50, 50, 255}); // arrow fletching
    ImageDrawRectangle(&img, 42, 44, 4, 12, Color{220, 180, 50, 255});

    // --- Long Ponytail flowing back (West direction) ---
    ImageDrawCircle(&img, 42, 34, 14, cHairDark);
    ImageDrawCircle(&img, 34, 38, 12, cHair);
    ImageDrawCircle(&img, 24, 46, 10, cHair);
    ImageDrawCircle(&img, 18, 56, 7, cHairDark);
    ImageDrawCircle(&img, 14, 66, 5, cHair);
    ImageDrawCircle(&img, 35, 36, 6, cHairLight); // ponytail highlight

    // --- Legs & Armored Greaves ---
    // Left leg
    ImageDrawRectangle(&img, 52, 80, 10, 24, cLeggings);
    ImageDrawRectangle(&img, 50, 96, 12, 24, cArmorPlate); // Shin guard
    ImageDrawRectangle(&img, 50, 94, 12, 4, cBronze);     // Knee trim
    ImageDrawRectangle(&img, 52, 118, 14, 6, cArmorDark);  // Boot foot
    // Right leg (forward, stepped East)
    ImageDrawRectangle(&img, 66, 80, 11, 24, cLeggings);
    ImageDrawRectangle(&img, 65, 96, 13, 24, cArmorPlate);
    ImageDrawRectangle(&img, 65, 94, 13, 4, cBronze);
    ImageDrawRectangle(&img, 67, 118, 16, 6, cArmorDark);

    // --- Armored Belt & Faulds ---
    ImageDrawRectangle(&img, 50, 72, 28, 9, cArmorDark);
    ImageDrawRectangle(&img, 58, 73, 10, 7, cBronze); // Wyvern buckle
    ImageDrawRectangle(&img, 72, 74, 7, 10, cArmorPlate); // Side hip armor

    // --- Exposed Midriff (Crop-top Design) ---
    ImageDrawRectangle(&img, 53, 62, 22, 10, cSkin);
    ImageDrawRectangle(&img, 51, 64, 4, 8, cSkinShadow);
    ImageDrawPixel(&img, 64, 67, Color{190, 150, 120, 255}); // Navel detail

    // --- Crop-top Armor Bustier ---
    ImageDrawRectangle(&img, 50, 48, 26, 14, cArmorDark);
    ImageDrawRectangle(&img, 52, 50, 22, 10, cArmorPlate);
    ImageDrawRectangle(&img, 50, 48, 26, 3, cBronze); // Gold trim top
    ImageDrawRectangle(&img, 52, 59, 22, 3, cBronze); // Gold trim bottom

    // --- Neck & Collar ---
    ImageDrawRectangle(&img, 58, 38, 10, 10, cSkin);
    ImageDrawRectangle(&img, 58, 44, 10, 3, cArmorDark); // Choker band

    // --- Left Arm & Shoulder Pauldron ---
    ImageDrawCircle(&img, 48, 48, 7, cBronze);     // Left pauldron
    ImageDrawCircle(&img, 48, 48, 5, cArmorPlate);
    ImageDrawRectangle(&img, 46, 54, 7, 16, cSkin);
    ImageDrawRectangle(&img, 46, 64, 8, 10, cArmorPlate); // Vambrace

    // --- Head (Anime Profile looking EAST) ---
    ImageDrawCircle(&img, 64, 30, 15, cSkin); // Head base
    ImageDrawRectangle(&img, 64, 28, 10, 10, cSkin); // East jaw/chin contour
    ImageDrawPixel(&img, 75, 34, cSkinShadow); // Nose tip pointing East

    // Anime Eye (Looking East)
    ImageDrawRectangle(&img, 68, 26, 6, 8, Color{20, 20, 25, 255}); // Eye contour
    ImageDrawRectangle(&img, 69, 27, 4, 6, cEyeTeal);               // Iris
    ImageDrawRectangle(&img, 70, 29, 2, 4, Color{10, 40, 50, 255}); // Pupil
    ImageDrawPixel(&img, 71, 27, WHITE);                             // Eye specular highlight
    ImageDrawLine(&img, 67, 24, 75, 24, Color{30, 25, 20, 255});    // Eyelash

    // Mouth
    ImageDrawPixel(&img, 73, 37, Color{210, 120, 120, 255});

    // Anime Hair (Front bangs, sides, no helmet!)
    ImageDrawCircle(&img, 59, 22, 13, cHair);
    ImageDrawCircle(&img, 66, 18, 11, cHair);
    ImageDrawCircle(&img, 72, 20, 8, cHair); // East front fringe
    ImageDrawLine(&img, 58, 14, 74, 17, cHairLight); // Shine band
    // Side locks framing face
    ImageDrawRectangle(&img, 56, 26, 6, 16, cHair);
    ImageDrawRectangle(&img, 64, 20, 5, 12, cHair);

    // --- Right Arm (Holding Bow Grip, facing East) ---
    ImageDrawRectangle(&img, 68, 50, 14, 7, cSkin);
    ImageDrawRectangle(&img, 78, 48, 12, 9, cArmorPlate); // Vambrace
    ImageDrawRectangle(&img, 78, 48, 12, 2, cBronze);
    ImageDrawCircle(&img, 90, 53, 5, cSkin);              // Hand on bow

    // --- GIANT GREAT HUNTER BOW (BÜYÜK YAY) ---
    // Massive curved recurve bow on East side, taller than character
    int bowX = 98;
    // Bow upper limb curving forward then back
    for (int t = 0; t <= 45; t++) {
        float rad = (float)t / 45.0f * 1.57f;
        int bx = bowX + (int)(std::sin(rad) * 16.0f);
        int by = 52 - (int)(std::cos(rad) * 44.0f);
        ImageDrawCircle(&img, bx, by, 4, cBowWood);
        ImageDrawCircle(&img, bx - 1, by, 3, cBowBone);
    }
    // Bow lower limb curving forward then back
    for (int t = 0; t <= 48; t++) {
        float rad = (float)t / 48.0f * 1.57f;
        int bx = bowX + (int)(std::sin(rad) * 16.0f);
        int by = 54 + (int)(std::cos(rad) * 46.0f);
        ImageDrawCircle(&img, bx, by, 4, cBowWood);
        ImageDrawCircle(&img, bx - 1, by, 3, cBowBone);
    }
    // Bow central riser / grip
    ImageDrawRectangle(&img, bowX - 6, 44, 12, 18, cBronze);
    ImageDrawRectangle(&img, bowX - 4, 46, 8, 14, cArmorDark);
    // Upper & lower bow tips (dragon horn carved)
    ImageDrawCircle(&img, bowX + 16, 7, 5, cBronze);
    ImageDrawCircle(&img, bowX + 16, 101, 5, cBronze);
    // Bowstring drawn taut
    ImageDrawLine(&img, bowX + 15, 8, 90, 53, cBowString);
    ImageDrawLine(&img, 90, 53, bowX + 15, 100, cBowString);

    return img;
}

// -------------------------------------------------------------
// Procedural Sprite Generator: Male Hunter (East-facing, Ronin/Wyvern Plate Coat, Katana, No Helmet)
// -------------------------------------------------------------
Image AssetManager::GenerateMaleHunterSprite() {
    const int W = 128;
    const int H = 128;
    Image img = GenImageColor(W, H, Color{0, 0, 0, 0});

    Color cSkin = {252, 218, 192, 255};
    Color cSkinShadow = {222, 180, 150, 255};
    Color cHair = {32, 38, 52, 255};
    Color cHairHighlight = {65, 80, 105, 255};
    Color cCoatDark = {38, 48, 64, 255};
    Color cCoatTrim = {210, 170, 70, 255};
    Color cSteel = {175, 185, 200, 255};
    Color cSteelLight = {230, 238, 245, 255};
    Color cLeather = {80, 50, 32, 255};
    Color cPants = {30, 32, 38, 255};
    Color cEyeBlue = {70, 160, 235, 255};

    // --- Legs & Heavy Hunter Greaves ---
    // Left leg
    ImageDrawRectangle(&img, 50, 82, 12, 22, cPants);
    ImageDrawRectangle(&img, 48, 96, 14, 24, cSteel);
    ImageDrawRectangle(&img, 48, 94, 14, 4, cCoatTrim);
    ImageDrawRectangle(&img, 48, 118, 16, 6, cLeather);
    // Right leg (braced stance forward East)
    ImageDrawRectangle(&img, 66, 82, 13, 22, cPants);
    ImageDrawRectangle(&img, 65, 96, 15, 24, cSteel);
    ImageDrawRectangle(&img, 65, 94, 15, 4, cCoatTrim);
    ImageDrawRectangle(&img, 67, 118, 18, 6, cLeather);

    // --- Armor Coat / Tassets & Waist Sash ---
    ImageDrawRectangle(&img, 46, 68, 34, 16, cCoatDark);
    ImageDrawRectangle(&img, 46, 68, 34, 4, cLeather);
    ImageDrawRectangle(&img, 58, 68, 10, 14, cCoatTrim); // Wyvern belt buckle
    // Split coat tails
    ImageDrawRectangle(&img, 44, 76, 10, 14, cCoatDark);
    ImageDrawRectangle(&img, 72, 76, 10, 12, cCoatDark);

    // --- Torso (Ronin Wyvern Plate Armor Coat) ---
    ImageDrawRectangle(&img, 48, 46, 30, 24, cCoatDark);
    ImageDrawRectangle(&img, 52, 48, 22, 18, cSteel); // Steel cuirass center
    ImageDrawRectangle(&img, 48, 46, 4, 24, cCoatTrim);
    ImageDrawRectangle(&img, 74, 46, 4, 24, cCoatTrim);

    // --- Neck & Collar ---
    ImageDrawRectangle(&img, 60, 36, 10, 12, cSkin);
    ImageDrawRectangle(&img, 56, 42, 18, 4, cCoatDark); // High coat collar

    // --- Left Shoulder Guard (Heavy Plate Pauldron) ---
    ImageDrawCircle(&img, 46, 46, 9, cSteel);
    ImageDrawCircle(&img, 46, 46, 7, cCoatTrim);
    ImageDrawCircle(&img, 46, 46, 5, cSteelLight);
    // Left arm
    ImageDrawRectangle(&img, 42, 54, 8, 16, cLeather);

    // --- Head (Anime Profile looking EAST) ---
    ImageDrawCircle(&img, 66, 28, 15, cSkin);
    ImageDrawRectangle(&img, 66, 26, 11, 11, cSkin); // Chiseled anime jaw East
    ImageDrawPixel(&img, 77, 33, cSkinShadow); // Nose tip

    // Anime Eye (Focused hunter eye East)
    ImageDrawRectangle(&img, 70, 25, 6, 6, Color{15, 15, 20, 255});
    ImageDrawRectangle(&img, 71, 26, 4, 5, cEyeBlue);
    ImageDrawRectangle(&img, 72, 27, 2, 3, Color{10, 25, 45, 255});
    ImageDrawPixel(&img, 73, 26, WHITE); // Highlight
    ImageDrawLine(&img, 69, 23, 77, 23, Color{20, 25, 35, 255}); // Brow

    // Mouth
    ImageDrawLine(&img, 72, 36, 76, 36, Color{160, 100, 90, 255});

    // Anime Spiky Hair (Charcoal/Dark Blue, stylish spikes, NO HELMET!)
    // Back spikes
    ImageDrawTriangle(&img, Vector2{56, 24}, Vector2{36, 16}, Vector2{52, 34}, cHair);
    ImageDrawTriangle(&img, Vector2{52, 32}, Vector2{34, 30}, Vector2{50, 40}, cHair);
    ImageDrawTriangle(&img, Vector2{58, 18}, Vector2{44, 8},  Vector2{66, 16}, cHair);
    // Top head volume
    ImageDrawCircle(&img, 64, 18, 13, cHair);
    ImageDrawCircle(&img, 72, 17, 9, cHair);
    // Front spikes & bangs over forehead
    ImageDrawTriangle(&img, Vector2{68, 16}, Vector2{78, 14}, Vector2{74, 24}, cHair);
    ImageDrawTriangle(&img, Vector2{74, 20}, Vector2{80, 22}, Vector2{74, 28}, cHair);
    // Hair highlights
    ImageDrawLine(&img, 58, 12, 72, 12, cHairHighlight);
    ImageDrawLine(&img, 62, 14, 76, 15, cHairHighlight);

    // --- Katana Saya (Scabbard at hip) ---
    for (int i = 0; i < 30; i++) {
        ImageDrawPixel(&img, 36 + i, 74 + (i / 5), Color{20, 20, 25, 255});
        ImageDrawPixel(&img, 36 + i, 75 + (i / 5), cCoatTrim);
    }

    // --- Right Arm (Griping Katana in ready stance, facing East) ---
    ImageDrawRectangle(&img, 70, 48, 16, 8, cLeather);
    ImageDrawRectangle(&img, 82, 46, 12, 11, cSteel);    // Armored vambrace
    ImageDrawRectangle(&img, 82, 46, 12, 2, cCoatTrim);
    ImageDrawCircle(&img, 94, 52, 6, cSkinShadow);        // Hand gripping hilt

    // --- KATANA (SWORD) FACING EAST ---
    // Tsuka (Hilt)
    ImageDrawRectangle(&img, 88, 50, 14, 5, Color{25, 25, 30, 255}); // Wrapped hilt
    ImageDrawPixel(&img, 90, 52, cCoatTrim);
    ImageDrawPixel(&img, 94, 52, cCoatTrim);
    ImageDrawPixel(&img, 98, 52, cCoatTrim);
    // Tsuba (Golden circular crossguard)
    ImageDrawRectangle(&img, 102, 45, 3, 15, cCoatTrim);
    ImageDrawCircle(&img, 103, 52, 4, cCoatTrim);
    // Habaki (Blade collar)
    ImageDrawRectangle(&img, 105, 50, 3, 5, cCoatTrim);

    // Katana Blade (Curved steel blade gleaming East)
    // Blade extends from x=108 to x=127 with subtle elegant upward curve
    for (int x = 108; x < 127; x++) {
        float prog = (float)(x - 108) / 19.0f;
        int y = 51 - (int)(prog * prog * 4.0f);
        // Blade back (mune)
        ImageDrawPixel(&img, x, y - 1, cSteel);
        // Blade core (shinogi)
        ImageDrawPixel(&img, x, y, cSteelLight);
        // Sharp edge (ha) & hamon
        ImageDrawPixel(&img, x, y + 1, WHITE);
    }
    // Katana Kissaki (Sharp pointed tip at East end)
    ImageDrawPixel(&img, 126, 47, WHITE);
    ImageDrawPixel(&img, 127, 46, WHITE);

    return img;
}

// -------------------------------------------------------------
// Procedural Map Placeholder: Monster Hunter Ancient Arena Hunting Ground
// -------------------------------------------------------------
Image AssetManager::GenerateMapArenaPlaceholder() {
    const int W = 512;
    const int H = 512;
    Image img = GenImageColor(W, H, Color{36, 62, 38, 255});

    Color cGrassLush = {44, 76, 46, 255};
    Color cGrassDark = {28, 50, 30, 255};
    Color cDirt = {78, 62, 44, 255};
    Color cStoneSlab = {92, 90, 82, 255};
    Color cStoneDark = {60, 58, 52, 255};
    Color cAncientGold = {175, 145, 80, 255};

    // 1. Tiled natural terrain patches
    for (int y = 0; y < H; y += 32) {
        for (int x = 0; x < W; x += 32) {
            if ((x + y) % 64 == 0) {
                ImageDrawRectangle(&img, x, y, 32, 32, cGrassLush);
            } else if ((x * 3 + y * 7) % 5 == 0) {
                ImageDrawRectangle(&img, x, y, 32, 32, cGrassDark);
            }
        }
    }

    // 2. Dirt paths across the hunting arena
    for (int i = 0; i < W; i += 4) {
        int py = (int)(256.0f + std::sin((float)i * 0.02f) * 40.0f);
        ImageDrawCircle(&img, i, py, 26, cDirt);
        int px = (int)(256.0f + std::cos((float)i * 0.02f) * 40.0f);
        ImageDrawCircle(&img, px, i, 26, cDirt);
    }

    // 3. Ancient Wyvern Stone Arena Ring (Center)
    Vector2 center = {256.0f, 256.0f};
    ImageDrawCircle(&img, (int)center.x, (int)center.y, 160, cStoneDark);
    ImageDrawCircle(&img, (int)center.x, (int)center.y, 150, cStoneSlab);
    ImageDrawCircle(&img, (int)center.x, (int)center.y, 90, cStoneDark);
    ImageDrawCircle(&img, (int)center.x, (int)center.y, 82, cStoneSlab);

    // Ancient Arena Carved Markings
    for (int angle = 0; angle < 360; angle += 30) {
        float rad = (float)angle * DEG2RAD;
        int sx = (int)(center.x + std::cos(rad) * 90.0f);
        int sy = (int)(center.y + std::sin(rad) * 90.0f);
        int ex = (int)(center.x + std::cos(rad) * 150.0f);
        int ey = (int)(center.y + std::sin(rad) * 150.0f);
        ImageDrawLine(&img, sx, sy, ex, ey, cAncientGold);
    }

    // Arena Center Wyvern Crest
    ImageDrawCircle(&img, (int)center.x, (int)center.y, 35, cStoneDark);
    ImageDrawCircle(&img, (int)center.x, (int)center.y, 28, cAncientGold);
    ImageDrawCircle(&img, (int)center.x, (int)center.y, 18, cStoneSlab);

    return img;
}

// -------------------------------------------------------------
// Procedural Sprite Generator: Dragon Boss (Wyvern, Facing West)
// -------------------------------------------------------------
Image AssetManager::GenerateDragonBossSprite() {
    const int W = 192;
    const int H = 192;
    Image img = GenImageColor(W, H, Color{0, 0, 0, 0});

    Color cScaleRed = {175, 32, 28, 255};
    Color cScaleDark = {115, 18, 18, 255};
    Color cPlateCharcoal = {42, 38, 44, 255};
    Color cPlateSpikes = {25, 22, 28, 255};
    Color cUnderbelly = {215, 160, 75, 255};
    Color cUnderbellyDark = {175, 120, 50, 255};
    Color cWingMembrane = {145, 28, 25, 235};
    Color cWingDark = {95, 16, 16, 255};
    Color cEyeGold = {255, 215, 20, 255};
    Color cTeeth = {245, 240, 225, 255};
    Color cClaws = {30, 28, 32, 255};

    // --- 1. Wyvern Spiked Tail (Right / East side) ---
    for (int t = 0; t <= 35; t++) {
        float rad = (float)t / 35.0f * 1.57f;
        int tx = 135 + (int)(std::sin(rad) * 42.0f);
        int ty = 120 - (int)(std::cos(rad) * 22.0f);
        int radius = 10 - (t / 4);
        if (radius < 3) radius = 3;
        ImageDrawCircle(&img, tx, ty, radius, cScaleRed);
        // Spikes along tail
        if (t % 6 == 0) {
            ImageDrawTriangle(&img, Vector2{(float)tx, (float)ty - radius},
                                   Vector2{(float)tx + 5.0f, (float)ty - radius - 8.0f},
                                   Vector2{(float)tx - 4.0f, (float)ty - radius}, cPlateSpikes);
        }
    }
    // Tail spade / blade
    ImageDrawTriangle(&img, Vector2{176, 100}, Vector2{190, 92}, Vector2{184, 110}, cPlateCharcoal);

    // --- 2. Wyvern Massive Wings (Up & Back) ---
    // Right wing (Background wing)
    Vector2 w1 = {115, 75};
    Vector2 w2 = {140, 18};
    Vector2 w3 = {180, 40};
    Vector2 w4 = {150, 80};
    ImageDrawTriangle(&img, w1, w2, w4, cWingDark);
    ImageDrawTriangle(&img, w2, w3, w4, cWingMembrane);
    ImageDrawLine(&img, (int)w1.x, (int)w1.y, (int)w2.x, (int)w2.y, cPlateCharcoal);
    ImageDrawLine(&img, (int)w2.x, (int)w2.y, (int)w3.x, (int)w3.y, cPlateCharcoal);

    // Left wing (Foreground dominant wing)
    Vector2 fw1 = {95, 85};
    Vector2 fw2 = {110, 12};
    Vector2 fw3 = {165, 30};
    Vector2 fw4 = {130, 95};
    ImageDrawTriangle(&img, fw1, fw2, fw4, cWingDark);
    ImageDrawTriangle(&img, fw2, fw3, fw4, cWingMembrane);
    ImageDrawLineEx(&img, fw1, fw2, 4.0f, cPlateCharcoal);
    ImageDrawLineEx(&img, fw2, fw3, 3.5f, cPlateCharcoal);
    // Wing claws / thumbs
    ImageDrawCircle(&img, (int)fw2.x, (int)fw2.y, 4, cClaws);
    ImageDrawTriangle(&img, Vector2{fw2.x - 2, fw2.y}, Vector2{fw2.x - 7, fw2.y - 6}, Vector2{fw2.x + 3, fw2.y - 1}, cClaws);

    // --- 3. Muscular Wyvern Torso & Back Plates ---
    ImageDrawCircle(&img, 105, 110, 32, cScaleDark);
    ImageDrawCircle(&img, 102, 108, 30, cScaleRed);
    ImageDrawCircle(&img, 90, 115, 26, cScaleRed);

    // Golden Underbelly Plating
    ImageDrawCircle(&img, 82, 122, 18, cUnderbellyDark);
    ImageDrawCircle(&img, 82, 122, 16, cUnderbelly);
    ImageDrawLine(&img, 72, 115, 88, 115, cUnderbellyDark);
    ImageDrawLine(&img, 70, 122, 90, 122, cUnderbellyDark);
    ImageDrawLine(&img, 72, 129, 88, 129, cUnderbellyDark);

    // Dorsal Spikes along spine
    ImageDrawTriangle(&img, Vector2{92, 85}, Vector2{96, 70}, Vector2{104, 86}, cPlateSpikes);
    ImageDrawTriangle(&img, Vector2{106, 82}, Vector2{112, 66}, Vector2{120, 84}, cPlateSpikes);
    ImageDrawTriangle(&img, Vector2{122, 86}, Vector2{128, 72}, Vector2{134, 90}, cPlateSpikes);

    // --- 4. Muscular Legs & Claws (Digging into ground) ---
    // Back leg
    ImageDrawRectangle(&img, 112, 125, 18, 28, cScaleDark);
    ImageDrawRectangle(&img, 110, 148, 22, 8, cClaws);
    // Front leg
    ImageDrawRectangle(&img, 86, 128, 20, 28, cScaleRed);
    ImageDrawCircle(&img, 96, 134, 13, cScaleDark); // Muscular thigh
    // Razor talons (pointing West)
    ImageDrawRectangle(&img, 76, 152, 26, 9, cClaws);
    ImageDrawTriangle(&img, Vector2{76, 154}, Vector2{64, 159}, Vector2{76, 161}, cClaws);
    ImageDrawTriangle(&img, Vector2{82, 154}, Vector2{72, 162}, Vector2{82, 161}, cClaws);
    ImageDrawTriangle(&img, Vector2{90, 154}, Vector2{82, 162}, Vector2{90, 161}, cClaws);

    // --- 5. Dragon Neck & Head (Facing WEST) ---
    // Neck arching forward
    ImageDrawCircle(&img, 72, 96, 20, cScaleRed);
    ImageDrawCircle(&img, 62, 86, 18, cScaleRed);
    ImageDrawCircle(&img, 52, 78, 16, cScaleDark);

    // Head base (Facing West)
    ImageDrawRectangle(&img, 32, 68, 30, 20, cScaleRed);
    ImageDrawTriangle(&img, Vector2{32, 68}, Vector2{12, 76}, Vector2{32, 88}, cScaleRed); // Snout pointing West
    ImageDrawRectangle(&img, 16, 75, 20, 10, cScaleDark); // Lower jaw

    // Sharp Razor Teeth
    for (int px = 18; px <= 32; px += 4) {
        ImageDrawTriangle(&img, Vector2{(float)px, 75.0f}, Vector2{(float)px + 2.0f, 79.0f}, Vector2{(float)px + 4.0f, 75.0f}, cTeeth);
    }

    // Horns (Large curved wyvern horns arching back/up)
    Vector2 hBase1 = {52, 68};
    Vector2 hTip1  = {84, 38};
    ImageDrawLineEx(&img, hBase1, hTip1, 6.0f, cPlateCharcoal);
    ImageDrawTriangle(&img, Vector2{80, 36}, Vector2{92, 30}, Vector2{86, 44}, cPlateSpikes);

    Vector2 hBase2 = {45, 66};
    Vector2 hTip2  = {74, 42};
    ImageDrawLineEx(&img, hBase2, hTip2, 4.5f, cPlateCharcoal);

    // Reptilian Blazing Eye (Looking West)
    ImageDrawRectangle(&img, 38, 70, 7, 6, Color{20, 20, 20, 255});
    ImageDrawRectangle(&img, 39, 71, 5, 4, cEyeGold);
    ImageDrawLine(&img, 41, 71, 41, 74, Color{10, 10, 10, 255}); // Slit pupil
    ImageDrawPixel(&img, 42, 71, WHITE); // Specular glint

    // Smoke embers from nostrils
    ImageDrawPixel(&img, 14, 73, Color{255, 120, 30, 255});
    ImageDrawPixel(&img, 12, 72, Color{255, 200, 50, 200});

    return img;
}
