#include "level.h"
#include <math.h>
#include <stdlib.h>

#define ADD_PLATFORM(level, X, Y, W, H) \
    (level)->platforms[(level)->platformCount++] = (Rectangle){ (X), (Y), (W), (H) }

void InitLevel(Level* level) {
    level->platformCount = 0;
    level->backgroundColor = (Color){ 30, 20, 50, 255 };
    level->bgTime = 0.0f;

    level->playerStart = (Vector2){ 100, 1480 };
    level->worldWidth = 1600.0f;
    level->worldHeight = 1600.0f;

    // Boundary walls (first 4 platforms)
    // index 0 = floor (drawn)
    // index 1 = ceiling (not drawn)
    // index 2 = left wall (not drawn)
    // index 3 = right wall (not drawn)
    ADD_PLATFORM(level, 0, 1550, 1600, 50);   // 0: floor
    ADD_PLATFORM(level, 0, 0, 1600, 20);   // 1: ceiling
    ADD_PLATFORM(level, 0, 0, 20, 1600); // 2: left wall
    ADD_PLATFORM(level, 1580, 0, 20, 1600); // 3: right wall

    // Row 1
    ADD_PLATFORM(level, 150, 1420, 250, 20);
    ADD_PLATFORM(level, 500, 1350, 250, 20);
    ADD_PLATFORM(level, 850, 1420, 250, 20);
    ADD_PLATFORM(level, 1200, 1350, 250, 20);

    // Row 2
    ADD_PLATFORM(level, 200, 1280, 250, 20);
    ADD_PLATFORM(level, 550, 1210, 250, 20);
    ADD_PLATFORM(level, 900, 1280, 250, 20);
    ADD_PLATFORM(level, 1250, 1210, 250, 20);

    // Row 3
    ADD_PLATFORM(level, 150, 1140, 250, 20);
    ADD_PLATFORM(level, 500, 1070, 250, 20);
    ADD_PLATFORM(level, 850, 1140, 250, 20);
    ADD_PLATFORM(level, 1200, 1070, 250, 20);

    // Row 4
    ADD_PLATFORM(level, 200, 1000, 250, 20);
    ADD_PLATFORM(level, 550, 930, 250, 20);
    ADD_PLATFORM(level, 900, 1000, 250, 20);
    ADD_PLATFORM(level, 1250, 930, 250, 20);

    // Row 5
    ADD_PLATFORM(level, 150, 860, 250, 20);
    ADD_PLATFORM(level, 500, 790, 250, 20);
    ADD_PLATFORM(level, 850, 860, 250, 20);
    ADD_PLATFORM(level, 1200, 790, 250, 20);

    // Row 6
    ADD_PLATFORM(level, 200, 720, 250, 20);
    ADD_PLATFORM(level, 550, 650, 250, 20);
    ADD_PLATFORM(level, 900, 720, 250, 20);
    ADD_PLATFORM(level, 1250, 650, 250, 20);

    // Row 7
    ADD_PLATFORM(level, 150, 580, 250, 20);
    ADD_PLATFORM(level, 500, 510, 250, 20);
    ADD_PLATFORM(level, 850, 580, 250, 20);
    ADD_PLATFORM(level, 1200, 510, 250, 20);

    // Top platform
    ADD_PLATFORM(level, 1200, 380, 320, 20);
}

void UpdateLevelBackground(Level* level, float deltaTime) {
    level->bgTime += deltaTime;
}

// ---------------------------------------------------------------------------
// Background art
// ---------------------------------------------------------------------------

static unsigned int Hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

static float Rand01(unsigned int seed) {
    return (float)(Hash(seed) & 0xFFFF) / 65535.0f;
}

static void GetViewRect(Camera2D* camera, float* x, float* y,
    float* w, float* h) {
    *w = GetScreenWidth() / camera->zoom;
    *h = GetScreenHeight() / camera->zoom;
    *x = camera->target.x - *w / 2.0f;
    *y = camera->target.y - *h / 2.0f;
}

// ---------------------------------------------------------------------------
// SKY - banded vertical gradient, anchored to world so it doesn't scroll.
// ---------------------------------------------------------------------------
static void DrawSky(Camera2D* camera) {
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);

    float padX = 400.0f;
    float padY = 400.0f;
    float top = vy - padY;
    float height = vh + padY * 2.0f;

    Color c1 = (Color){ 20,  10,  40, 255 };
    Color c2 = (Color){ 60,  30,  90, 255 };
    Color c3 = (Color){ 150,  70, 100, 255 };
    Color c4 = (Color){ 240, 130,  90, 255 };
    Color c5 = (Color){ 255, 200, 120, 255 };

    float bandH = height / 4.0f;

    DrawRectangleGradientV((int)(vx - padX), (int)top,
        (int)(vw + padX * 2), (int)bandH, c1, c2);
    DrawRectangleGradientV((int)(vx - padX), (int)(top + bandH),
        (int)(vw + padX * 2), (int)bandH, c2, c3);
    DrawRectangleGradientV((int)(vx - padX), (int)(top + bandH * 2),
        (int)(vw + padX * 2), (int)bandH, c3, c4);
    DrawRectangleGradientV((int)(vx - padX), (int)(top + bandH * 3),
        (int)(vw + padX * 2), (int)bandH, c4, c5);
}

// ---------------------------------------------------------------------------
// STARS
// ---------------------------------------------------------------------------
static void DrawStars(Camera2D* camera) {
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);

    float tileW = 800.0f;
    float tileH = 800.0f;

    int minTX = (int)floorf(vx / tileW) - 1;
    int maxTX = (int)floorf((vx + vw) / tileW) + 1;
    int minTY = (int)floorf(vy / tileH) - 1;
    int maxTY = (int)floorf((vy + vh) / tileH) + 1;

    for (int ty = minTY; ty <= maxTY; ty++) {
        for (int tx = minTX; tx <= maxTX; tx++) {
            unsigned int base = Hash((unsigned int)(tx * 92837111) ^ (unsigned int)(ty * 689287499));
            for (int i = 0; i < 40; i++) {
                unsigned int s = base + (unsigned int)(i * 2654435761u);
                float sx = Rand01(s) * tileW + tx * tileW;
                float sy = Rand01(s ^ 0xABCD1234u) * tileH + ty * tileH;
                if (sy > vy + vh * 0.65f) continue;
                float twinkle = 0.5f + 0.5f * sinf((float)GetTime() * 2.0f + (float)i);
                float alpha = Rand01(s ^ 0x9E3779B9u) * 0.7f + 0.3f;
                Color col = Fade(WHITE, alpha * twinkle * 0.9f);
                int size = (Rand01(s ^ 0x1234567u) > 0.85f) ? 2 : 1;
                DrawRectangle((int)sx, (int)sy, size, size, col);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// SUN - halos and body, no light streaks.
// ---------------------------------------------------------------------------
static void DrawSun(Level* level, Camera2D* camera) {
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);
    (void)vh;

    float sx = fmodf(level->bgTime * 6.0f, 3000.0f) + vx - 300.0f;
    float sy = vy + 180.0f;

    for (int k = -1; k <= 1; k++) {
        float x = sx + k * 3000.0f;
        for (int i = 6; i >= 0; i--) {
            float r = 60.0f + i * 40.0f;
            float a = 0.09f - i * 0.012f;
            if (a < 0.0f) a = 0.0f;
            DrawCircle((int)x, (int)sy, (int)r,
                Fade((Color) { 255, 180, 100, 255 }, a));
        }
        DrawCircle((int)x, (int)sy, 70, (Color) { 255, 230, 180, 255 });
        DrawCircle((int)x, (int)sy, 55, (Color) { 255, 245, 220, 255 });
    }
}

// ---------------------------------------------------------------------------
// MOUNTAIN RIDGE
// ---------------------------------------------------------------------------
static void DrawMountainRidge(Camera2D* camera,
    float scrollSpeed,
    float baselineY,
    float peakHeight,
    float tileW,
    Color bodyColor)
{
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);
    (void)vh;

    float offset = -(float)GetTime() * scrollSpeed;

    int minTX = (int)floorf((vx - offset) / tileW) - 1;
    int maxTX = (int)floorf((vx - offset + vw) / tileW) + 1;

    for (int tx = minTX; tx <= maxTX; tx++) {
        unsigned int seed = Hash((unsigned int)tx ^ 0xDEADBEEFu);
        int peakCount = 5 + (int)(Rand01(seed) * 3.0f);
        float tileLeft = tx * tileW + offset;
        float step = tileW / (float)peakCount;

        float prevX = tileLeft;
        float prevY = baselineY + (Rand01(seed ^ 0x11111111u) * 40.0f);

        for (int i = 1; i <= peakCount; i++) {
            float px = tileLeft + step * i;
            float t = Rand01(Hash(seed ^ (unsigned int)(i * 0x9E3779B9u)));
            float py = baselineY - peakHeight * (0.5f + 0.5f * t);

            Vector2 a = { floorf(prevX),    floorf(baselineY + 200.0f) };
            Vector2 b = { floorf(prevX),    floorf(prevY) };
            Vector2 c = { ceilf(px) + 1.0f, floorf(py) };
            Vector2 d = { ceilf(px) + 1.0f, floorf(baselineY + 200.0f) };
            DrawTriangle(a, b, c, bodyColor);
            DrawTriangle(a, c, d, bodyColor);

            prevX = px;
            prevY = py;
        }
    }
}

// ---------------------------------------------------------------------------
// HILLS
// ---------------------------------------------------------------------------
static void DrawHillBand(Camera2D* camera, float scrollSpeed,
    float baselineY, float radius,
    Color hillColor, Color treeColor,
    bool trees)
{
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);
    (void)vh;

    float offset = -(float)GetTime() * scrollSpeed;
    float spacing = radius * 1.4f;

    int minTX = (int)floorf((vx - offset) / spacing) - 2;
    int maxTX = (int)floorf((vx - offset + vw) / spacing) + 2;

    for (int tx = minTX; tx <= maxTX; tx++) {
        unsigned int seed = Hash((unsigned int)tx ^ 0xBEEFCAFEu);
        float t = Rand01(seed);
        float r = radius * (0.85f + 0.3f * t);
        float x = tx * spacing + offset;
        float y = baselineY + r * 0.15f;

        DrawCircle((int)floorf(x), (int)floorf(y), (int)r, hillColor);

        if (trees && (Rand01(seed ^ 0x87654321u) > 0.5f)) {
            float treeX = x + (Rand01(seed ^ 0x11112222u) - 0.5f) * r * 1.2f;
            float treeBaseY = y - r * 0.6f;
            float treeH = 40.0f + Rand01(seed ^ 0x33334444u) * 20.0f;
            float treeW = treeH * 0.55f;

            DrawRectangle((int)floorf(treeX - 2),
                (int)floorf(treeBaseY),
                4, (int)(treeH * 0.3f), treeColor);
            for (int i = 0; i < 3; i++) {
                float fY = treeBaseY - i * treeH * 0.22f;
                float fW = treeW * (1.0f - i * 0.2f);
                Vector2 a = { floorf(treeX - fW * 0.5f),       floorf(fY) };
                Vector2 b = { ceilf(treeX + fW * 0.5f) + 1.0f, floorf(fY) };
                Vector2 c = { floorf(treeX),                    floorf(fY - treeH * 0.4f) };
                DrawTriangle(a, b, c, treeColor);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CLOUDS
// ---------------------------------------------------------------------------
static void DrawSoftCloud(float x, float y, float scale, float alpha) {
    Color c = Fade((Color) { 255, 220, 210, 255 }, alpha);
    DrawCircle((int)(x - 40 * scale), (int)y, (int)(26 * scale), c);
    DrawCircle((int)(x - 10 * scale), (int)(y - 12 * scale), (int)(34 * scale), c);
    DrawCircle((int)(x + 25 * scale), (int)(y - 6 * scale), (int)(30 * scale), c);
    DrawCircle((int)(x + 55 * scale), (int)(y + 4 * scale), (int)(22 * scale), c);
    DrawCircle((int)(x - 20 * scale), (int)(y + 10 * scale), (int)(22 * scale), c);
    DrawCircle((int)(x + 20 * scale), (int)(y + 12 * scale), (int)(24 * scale), c);
}

static void DrawCloudBand(Camera2D* camera, float scrollSpeed,
    float baseY, float spreadY,
    int count, float scale, float alpha)
{
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);
    (void)vh;

    float offset = -(float)GetTime() * scrollSpeed;
    float tileW = 900.0f;
    int minTX = (int)floorf((vx - offset) / tileW) - 1;
    int maxTX = (int)floorf((vx - offset + vw) / tileW) + 1;

    for (int tx = minTX; tx <= maxTX; tx++) {
        for (int i = 0; i < count; i++) {
            unsigned int seed = Hash((unsigned int)(tx * 92821 + i * 68917));
            float ox = Rand01(seed) * tileW;
            float oy = (Rand01(seed ^ 0x11111111u) - 0.5f) * spreadY;
            float sc = scale * (0.8f + 0.5f * Rand01(seed ^ 0x22222222u));

            float x = tx * tileW + ox + offset;
            float y = baseY + oy;

            DrawSoftCloud(floorf(x), floorf(y), sc, alpha);
        }
    }
}

// ---------------------------------------------------------------------------
// FOREGROUND ROCKS
// ---------------------------------------------------------------------------
static void DrawForegroundRocks(Camera2D* camera, float scrollSpeed) {
    float offset = -(float)GetTime() * scrollSpeed;

    float baseY = 1620.0f;

    Color rockColor = (Color){ 25, 20, 35, 255 };
    Color rockTop = (Color){ 40, 32, 55, 255 };

    float tileW = 700.0f;
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);
    (void)vh;

    int minTX = (int)floorf((vx - offset) / tileW) - 1;
    int maxTX = (int)floorf((vx - offset + vw) / tileW) + 1;

    for (int tx = minTX; tx <= maxTX; tx++) {
        unsigned int seed = Hash((unsigned int)tx ^ 0xC0FFEEu);
        float ox = Rand01(seed) * tileW;
        float r = 80.0f + Rand01(seed ^ 0x55AA55AAu) * 60.0f;
        float x = tx * tileW + ox + offset;

        DrawCircle((int)floorf(x), (int)floorf(baseY + r * 0.5f), (int)r, rockColor);
        DrawCircle((int)floorf(x), (int)floorf(baseY + r * 0.35f), (int)(r * 0.75f), rockTop);
    }
}

// ---------------------------------------------------------------------------
// Master background draw.
// ---------------------------------------------------------------------------
void DrawLevelBackground(Level* level, Camera2D* camera) {
    (void)level;

    DrawSky(camera);
    DrawStars(camera);
    DrawSun(level, camera);

    DrawCloudBand(camera, 8.0f, 620.0f, 120.0f, 3, 1.6f, 0.90f);

    DrawMountainRidge(camera, 15.0f, 1180.0f, 220.0f, 900.0f,
        (Color) {
        110, 90, 140, 255
    });

    DrawMountainRidge(camera, 30.0f, 1280.0f, 260.0f, 750.0f,
        (Color) {
        60, 50, 90, 255
    });

    DrawCloudBand(camera, 45.0f, 700.0f, 90.0f, 2, 1.9f, 0.95f);

    DrawHillBand(camera, 60.0f, 1380.0f, 150.0f,
        (Color) {
        55, 80, 90, 255
    },
        (Color) {
        30, 45, 55, 255
    },
        true);

    DrawHillBand(camera, 90.0f, 1470.0f, 170.0f,
        (Color) {
        30, 45, 60, 255
    },
        (Color) {
        15, 25, 35, 255
    },
        false);

    DrawCloudBand(camera, 110.0f, 900.0f, 80.0f, 2, 2.2f, 0.85f);

    DrawForegroundRocks(camera, 150.0f);
}

// Draw every platform EXCEPT the ceiling (index 1), left wall (index 2),
// and right wall (index 3). The floor (index 0) stays visible because
// that's what the player stands on.
void DrawLevel(Level* level) {
    for (int i = 0; i < level->platformCount; i++) {
        if (i == 1 || i == 2 || i == 3) continue;

        DrawRectangleRec(level->platforms[i], DARKGREEN);
        DrawRectangleLinesEx(level->platforms[i], 2, DARKGRAY);
    }
}