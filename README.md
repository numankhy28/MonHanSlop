# 2D Monster Hunter Inspired Mobile? Vibe-Coded Project (Raylib + C++)

## About The Project

This project was developed using **Gemini 3.8 Flash** as a showcase of current AI agent capabilities. 

The primary goal is to demonstrate the raw engineering and problem-solving capacity of an autonomous AI agent — generating a complete, functional game architecture entirely natively, without using external boilerplates, pre-made frameworks, or third-party code templates.
 
Unfortunately the goals cannot accomplished. Generated sprites has a lot of flaws and game mechanics doesn't feel good at all. In the beginning this project was planned to be use Paper-Doll system for customizable armor & weapon sprites. Agent doesn't understand any references or instructions at all , so this feature is gone forever. Suprisingly agent doesn't consider Trademarks as a threat and has no problem to use freely. So here we are , our Red Wyvern "King of the Skies" boss Rathalos 
<p align="center">
  <img src="assets/gameplay.png" alt="Gameplay Screenshot" width="300">
</p>


---

## Classes & Architecture

1. **`ScreenManager` (Mobile Automatic Screen Scaling)**
   - Virtual Resolution: **1280x720 (16:9 Landscape)**.
   - Calculates an automatic letterbox using `scale = min(screenW/virtualW, screenH/virtualH)` to handle various mobile screen sizes, notches, and window dimensions.
   - Converts touch and mouse coordinates from physical screen space to the virtual game canvas (`ScreenToVirtual`).

2. **`RenderManager` (Centralized Render Management)**
   - **All rendering is handled within a single function (`RenderManager::Render`).**
   - Individual classes (Character, DragonBoss, Map, etc.) never manage their own render loops (`BeginDrawing`/`EndDrawing`).
   - Draws the 8-directional blue aim indicator, high-quality 2D anime character sprites, dragon boss, extended katana arc wave, 8-directional flying arrows, and the UI from a unified center.

3. **`VirtualJoystick` & Linear Algebra Normalization**
   - Touchscreen / draggable virtual joystick in the bottom-left corner tailored for mobile landscape orientation.
   - **Linear Algebra Normalization:** Prevents movement speed from multiplying by $\sqrt{2} \approx 1.414$ during diagonal movement ($\vec{v} / \Vert{}\vec{v}\Vert{}$), keeping speed consistent across all directions.
   - Includes WASD / Arrow keys input support for PC testing.

4. **High-Quality 2D Anime Character & Boss Sprites (`assets/`)**
   - **Female Hunter:**
     - Full-body 2D anime character art facing East.
     - Helmetless design with flowing golden/blonde ponytail hair.
     - Crop-top dragon-scale hunter armor (exposed midriff), pauldrons, quiver, and a massive carved **Great Bow**.
     - High-resolution PNG format with a transparent background (`assets/female_hunter_east.png`).
   - **Male Hunter:**
     - Full-body 2D anime warrior art facing East.
     - Helmetless design with stylized messy dark hair and an intense gaze.
     - Ronin / Wyvern plate coat armor, steel chestplate, and greaves.
     - Gripping a glowing steel **Katana** in a combat stance.
     - High-resolution PNG format with a transparent background (`assets/male_hunter_east.png`).
   - **Dragon Boss (Rathalos-Style Fire Wyvern):**
     - Majestic and threatening 2D anime boss art facing West.
     - Large curved horns, sharp jaw, and smoking nostrils.
     - Massive leathery wings, muscular talons, and a spiked heavy tail.
     - High-resolution PNG format with a transparent background (`assets/dragon_boss.png`).

5. **8-Directional Attack Vector & Blue Aim Indicator**
   - A glowing blue energy orb appears in front of the character to clearly display the attack trajectory.
   - Arrows and Katana arcs align dynamically to this 8-directional vector.
   - Katana arc attack range extended to **150 pixels** with increased damage.
   - Evade Dash features a **3-second reload/cooldown period** accompanied by a visual reload counter.

6. **Game Over & Restart System (Game Over / Victory)**
   - Triggers a Victory screen upon slaying the dragon and a Defeat screen if character HP reaches zero.
   - Instant reset available via the **"RESTART HUNT"** button or pressing **`R` / `Space` / `Enter`** on the keyboard.
---

## Controls
| Action                      | Mobile (Touch)                                            | PC (Keyboard) |
| **Movement**                | Bottom-left Virtual Joystick                              | **W, A, S, D** / **Arrow Keys** |
| **Attack (Shoot / Slash)**  | Bottom-right red **SHOOT / SLASH** button                 | **`J`** / **`Enter`** |
| **Evade (Dash)**            | Bottom-right blue **EVADE** button *(3s Cooldown)*        | **`Space`** / **`K`** |
| **Change Hunter**           | Top-right **CHANGE HUNTER** button                        | **`C`** |
| **Restart Hunt**            | On-screen **RESTART HUNT** button *(Game Over / Victory)* | **`R`** |

---

## Build & Run
You can double-click the build.bat file in the folder or run it via the terminal:
```cmd
build.bat --run
```
or directly:
```cmd
run.bat
```
