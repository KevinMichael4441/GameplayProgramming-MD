#include "level.h"
#include <math.h>
#include <stdlib.h>

#define ADD_PLATFORM(level, X, Y, W, H) \
    (level)->platforms[(level)->platformCount++] = (Rectangle){ (X), (Y), (W), (H) }

void InitLevel(Level* level, MapTheme theme) {
    level->platformCount = 0;
    level->bgTime = 0.0f;
    level->theme = theme;
    level->backgroundColor = (Color){ 30, 20, 50, 255 };

    level->playerStart = (Vector2){ 100, 880 };
    level->worldWidth = 1600.0f;
    level->worldHeight = 1000.0f;

    // Boundary walls (indices 0-3)
    ADD_PLATFORM(level, 0, 950, 1600, 50);    // floor
    ADD_PLATFORM(level, 0, 0, 1600, 20);    // ceiling
    ADD_PLATFORM(level, 0, 0, 20, 1000);  // left wall
    ADD_PLATFORM(level, 1580, 0, 20, 1000);  // right wall

    // Row 1
    ADD_PLATFORM(level, 200, 850, 220, 20);
    ADD_PLATFORM(level, 700, 850, 220, 20);
    ADD_PLATFORM(level, 1200, 850, 220, 20);

    // Row 2
    ADD_PLATFORM(level, 400, 720, 220, 20);
    ADD_PLATFORM(level, 900, 720, 220, 20);
    ADD_PLATFORM(level, 1350, 720, 200, 20);

    // Row 3
    ADD_PLATFORM(level, 150, 590, 220, 20);
    ADD_PLATFORM(level, 650, 590, 220, 20);
    ADD_PLATFORM(level, 1150, 590, 220, 20);

    // Row 4
    ADD_PLATFORM(level, 350, 460, 220, 20);
    ADD_PLATFORM(level, 850, 460, 220, 20);
    ADD_PLATFORM(level, 1300, 460, 220, 20);

    // Row 5
    ADD_PLATFORM(level, 600, 330, 220, 20);
    ADD_PLATFORM(level, 1050, 330, 220, 20);

    // Top platform
    ADD_PLATFORM(level, 700, 200, 260, 20);
}

void UpdateLevelBackground(Level* level, float deltaTime) {
    level->bgTime += deltaTime;
}

const char* GetMapThemeName(MapTheme theme) {
    switch (theme) {
    case MAP_NORMAL: return "ACID";
    case MAP_SNOW:   return "SNOW";
    case MAP_JUNGLE: return "JUNGLE";
    case MAP_TIME:   return "TIME";
    default:         return "?";
    }
}

// ---------------------------------------------------------------------------
// Helpers
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

static Color HSV(float h, float s, float v, unsigned char a) {
    h = h - floorf(h);
    float i = floorf(h * 6.0f);
    float f = h * 6.0f - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - f * s);
    float t = v * (1.0f - (1.0f - f) * s);

    float r = 0, g = 0, b = 0;
    switch ((int)i % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default:r = v; g = p; b = q; break;
    }
    return (Color) {
        (unsigned char)(r * 255.0f),
            (unsigned char)(g * 255.0f),
            (unsigned char)(b * 255.0f),
            a
    };
}

static float HueShift(float base) {
    return base + (float)GetTime() * 0.08f;
}

// ---------------------------------------------------------------------------
// Sky
// ---------------------------------------------------------------------------

static void GetSkyPalette(MapTheme theme, Color out[5]) {
    if (theme == MAP_NORMAL) {
        float h = HueShift(0.0f);
        out[0] = HSV(h + 0.00f, 0.85f, 0.55f, 255);
        out[1] = HSV(h + 0.10f, 0.85f, 0.75f, 255);
        out[2] = HSV(h + 0.20f, 0.85f, 0.95f, 255);
        out[3] = HSV(h + 0.35f, 0.80f, 1.00f, 255);
        out[4] = HSV(h + 0.50f, 0.70f, 1.00f, 255);
        return;
    }

    switch (theme) {
    case MAP_SNOW:
        out[0] = (Color){ 40,  70, 120, 255 };
        out[1] = (Color){ 90, 130, 180, 255 };
        out[2] = (Color){ 160, 200, 230, 255 };
        out[3] = (Color){ 220, 235, 245, 255 };
        out[4] = (Color){ 255, 250, 245, 255 };
        break;
    case MAP_JUNGLE:
        out[0] = (Color){ 15,  40,  30, 255 };
        out[1] = (Color){ 40,  90,  60, 255 };
        out[2] = (Color){ 110, 160,  90, 255 };
        out[3] = (Color){ 200, 210, 130, 255 };
        out[4] = (Color){ 250, 230, 150, 255 };
        break;
    case MAP_TIME:
        out[0] = (Color){ 10,   5,  30, 255 };
        out[1] = (Color){ 40,  20,  90, 255 };
        out[2] = (Color){ 110,  60, 170, 255 };
        out[3] = (Color){ 220, 120, 200, 255 };
        out[4] = (Color){ 255, 200, 230, 255 };
        break;
    default:
        break;
    }
}

static void DrawSky(Camera2D* camera, MapTheme theme) {
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);

    float padX = 400.0f;
    float padY = 400.0f;
    float top = vy - padY;
    float height = vh + padY * 2.0f;

    Color pal[5];
    GetSkyPalette(theme, pal);

    float bandH = height / 4.0f;

    DrawRectangleGradientV((int)(vx - padX), (int)top,
        (int)(vw + padX * 2), (int)bandH, pal[0], pal[1]);
    DrawRectangleGradientV((int)(vx - padX), (int)(top + bandH),
        (int)(vw + padX * 2), (int)bandH, pal[1], pal[2]);
    DrawRectangleGradientV((int)(vx - padX), (int)(top + bandH * 2),
        (int)(vw + padX * 2), (int)bandH, pal[2], pal[3]);
    DrawRectangleGradientV((int)(vx - padX), (int)(top + bandH * 3),
        (int)(vw + padX * 2), (int)bandH, pal[3], pal[4]);
}

// ---------------------------------------------------------------------------
// Aurora (TIME only)
// ---------------------------------------------------------------------------

static void DrawAurora(Camera2D* camera, MapTheme theme) {
    if (theme != MAP_TIME) return;

    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);

    float baseY = vy + vh * 0.26f;
    float time = (float)GetTime();

    float w = vw + 800.0f;
    float x = vx - 400.0f;

    Color c1Base = (Color){ 200, 120, 255, 255 };
    Color c2Base = (Color){ 255, 160, 240, 255 };

    for (int i = 0; i < 5; i++) {
        float phase = time * 0.5f + i * 1.7f;
        float y = baseY + i * 22.0f + sinf(phase) * 14.0f;

        float centerBias = 1.0f - fabsf((float)i - 2.0f) / 2.0f;
        float alpha = 0.18f + 0.14f * centerBias
            + 0.06f * sinf(phase * 0.7f);

        Color c1 = Fade(c1Base, alpha);
        Color c2 = Fade(c2Base, alpha);

        DrawRectangleGradientH((int)x, (int)(y + 6),
            (int)w, 14,
            Fade(c1, alpha * 0.35f),
            Fade(c2, alpha * 0.35f));
        DrawRectangleGradientH((int)x, (int)y, (int)w, 8, c1, c2);
        DrawRectangleGradientH((int)x, (int)y,
            (int)w, 2,
            Fade(WHITE, alpha * 0.55f),
            Fade(WHITE, alpha * 0.55f));
    }
}

// ---------------------------------------------------------------------------
// Stars (TIME only)
// ---------------------------------------------------------------------------

static void DrawStars(Camera2D* camera, MapTheme theme) {
    if (theme != MAP_TIME) return;

    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);

    float tileW = 800.0f;
    float tileH = 800.0f;

    int minTX = (int)floorf(vx / tileW) - 1;
    int maxTX = (int)floorf((vx + vw) / tileW) + 1;
    int minTY = (int)floorf(vy / tileH) - 1;
    int maxTY = (int)floorf((vy + vh) / tileH) + 1;

    Color starColor = (Color){ 230, 200, 255, 255 };

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
                Color col = Fade(starColor, alpha * twinkle * 0.9f);
                int size = (Rand01(s ^ 0x1234567u) > 0.85f) ? 2 : 1;
                DrawRectangle((int)sx, (int)sy, size, size, col);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Acid overlay (MAP_NORMAL only)
// ---------------------------------------------------------------------------

static void DrawAcidOverlay(Camera2D* camera) {
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);

    float t = (float)GetTime();

    int ribbons = 7;
    for (int r = 0; r < ribbons; r++) {
        float phase = t * 0.6f + r * 1.3f;
        float baseY = vy + vh * (0.15f + 0.1f * r) + sinf(phase) * 30.0f;

        float alpha = 0.08f + 0.05f * sinf(phase * 0.7f);

        float step = 40.0f;
        int steps = (int)(vw / step) + 2;
        for (int i = 0; i < steps; i++) {
            float x0 = vx + i * step;
            float x1 = x0 + step;

            float y0 = baseY + sinf(t * 1.5f + x0 * 0.005f + r) * 20.0f;
            float y1 = baseY + sinf(t * 1.5f + x1 * 0.005f + r) * 20.0f;

            float hue = HueShift(0.0f) + (float)r / (float)ribbons + x0 * 0.0002f;
            Color c = HSV(hue, 0.9f, 1.0f, (unsigned char)(alpha * 255.0f));
            Color c2 = HSV(hue + 0.15f, 0.9f, 1.0f, (unsigned char)(alpha * 255.0f));

            DrawTriangle((Vector2) { x0, y0 },
                (Vector2) {
                x1, y1
            },
                (Vector2) {
                x1, y1 + 40.0f
            },
                c);
            DrawTriangle((Vector2) { x0, y0 },
                (Vector2) {
                x1, y1 + 40.0f
            },
                (Vector2) {
                x0, y0 + 40.0f
            },
                c2);
        }
    }

    int rings = 3;
    for (int r = 0; r < rings; r++) {
        float phase = t * 0.4f + r * 2.0f;
        float rad = fmodf(phase, 2.0f) * 260.0f;
        float alpha = 0.20f * (1.0f - rad / 520.0f);
        if (alpha < 0.0f) alpha = 0.0f;

        float cx = vx + vw * (0.4f + 0.2f * sinf(t * 0.3f + r));
        float cy = vy + vh * (0.4f + 0.2f * cosf(t * 0.35f + r));

        float hue = HueShift(0.0f) + r * 0.3f;
        Color c = HSV(hue, 1.0f, 1.0f, (unsigned char)(alpha * 255.0f));

        DrawCircleLines((int)cx, (int)cy, rad, c);
        DrawCircleLines((int)cx, (int)cy, rad + 2, c);
    }
}

// ---------------------------------------------------------------------------
// Sun
// ---------------------------------------------------------------------------

static void DrawSun(Level* level, Camera2D* camera) {
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);
    (void)vh;

    if (level->theme == MAP_SNOW) return;

    float sx = fmodf(level->bgTime * 6.0f, 3000.0f) + vx - 300.0f;
    float sy = vy + 180.0f;

    Color glow, body1, body2;
    if (level->theme == MAP_NORMAL) {
        float h = HueShift(0.0f);
        glow = HSV(h + 0.10f, 0.85f, 1.00f, 255);
        body1 = HSV(h + 0.20f, 0.60f, 1.00f, 255);
        body2 = HSV(h + 0.30f, 0.30f, 1.00f, 255);
    }
    else if (level->theme == MAP_JUNGLE) {
        glow = (Color){ 255, 240, 130, 255 };
        body1 = (Color){ 255, 250, 200, 255 };
        body2 = (Color){ 255, 255, 220, 255 };
    }
    else {
        glow = (Color){ 255, 180, 100, 255 };
        body1 = (Color){ 255, 230, 180, 255 };
        body2 = (Color){ 255, 245, 220, 255 };
    }

    for (int k = -1; k <= 1; k++) {
        float x = sx + k * 3000.0f;
        for (int i = 6; i >= 0; i--) {
            float r = 60.0f + i * 40.0f;
            float a = 0.09f - i * 0.012f;
            if (a < 0.0f) a = 0.0f;
            DrawCircle((int)x, (int)sy, (int)r, Fade(glow, a));
        }
        DrawCircle((int)x, (int)sy, 70, body1);
        DrawCircle((int)x, (int)sy, 55, body2);
    }
}

// ---------------------------------------------------------------------------
// Theme colors
// ---------------------------------------------------------------------------

static Color ThemeMountainFar(MapTheme theme) {
    if (theme == MAP_NORMAL) return HSV(HueShift(0.55f), 0.6f, 0.7f, 255);
    switch (theme) {
    case MAP_SNOW:   return (Color) { 150, 170, 200, 255 };
    case MAP_JUNGLE: return (Color) { 50, 90, 70, 255 };
    case MAP_TIME:   return (Color) { 80, 50, 130, 255 };
    default:         return (Color) { 110, 90, 140, 255 };
    }
}
static Color ThemeMountainNear(MapTheme theme) {
    if (theme == MAP_NORMAL) return HSV(HueShift(0.55f) + 0.1f, 0.7f, 0.5f, 255);
    switch (theme) {
    case MAP_SNOW:   return (Color) { 80, 110, 150, 255 };
    case MAP_JUNGLE: return (Color) { 25, 60, 45, 255 };
    case MAP_TIME:   return (Color) { 50, 30, 90, 255 };
    default:         return (Color) { 60, 50, 90, 255 };
    }
}
static Color ThemeHillFar(MapTheme theme) {
    if (theme == MAP_NORMAL) return HSV(HueShift(0.30f), 0.8f, 0.85f, 255);
    switch (theme) {
    case MAP_SNOW:   return (Color) { 200, 220, 240, 255 };
    case MAP_JUNGLE: return (Color) { 60, 110, 60, 255 };
    case MAP_TIME:   return (Color) { 70, 50, 120, 255 };
    default:         return (Color) { 55, 80, 90, 255 };
    }
}
static Color ThemeHillNear(MapTheme theme) {
    if (theme == MAP_NORMAL) return HSV(HueShift(0.30f) + 0.15f, 0.85f, 0.65f, 255);
    switch (theme) {
    case MAP_SNOW:   return (Color) { 160, 190, 220, 255 };
    case MAP_JUNGLE: return (Color) { 35, 80, 45, 255 };
    case MAP_TIME:   return (Color) { 40, 25, 75, 255 };
    default:         return (Color) { 30, 45, 60, 255 };
    }
}
static Color ThemeTree(MapTheme theme) {
    if (theme == MAP_NORMAL) return HSV(HueShift(0.15f), 0.9f, 0.55f, 255);
    switch (theme) {
    case MAP_SNOW:   return (Color) { 60, 90, 75, 255 };
    case MAP_JUNGLE: return (Color) { 20, 60, 30, 255 };
    case MAP_TIME:   return (Color) { 30, 20, 50, 255 };
    default:         return (Color) { 30, 45, 55, 255 };
    }
}
static Color ThemeRock(MapTheme theme) {
    if (theme == MAP_NORMAL) return HSV(HueShift(0.75f), 0.7f, 0.45f, 255);
    switch (theme) {
    case MAP_SNOW:   return (Color) { 180, 200, 220, 255 };
    case MAP_JUNGLE: return (Color) { 20, 35, 25, 255 };
    case MAP_TIME:   return (Color) { 25, 10, 40, 255 };
    default:         return (Color) { 25, 20, 35, 255 };
    }
}
static Color ThemeRockTop(MapTheme theme) {
    if (theme == MAP_NORMAL) return HSV(HueShift(0.75f) + 0.1f, 0.6f, 0.7f, 255);
    switch (theme) {
    case MAP_SNOW:   return (Color) { 220, 235, 245, 255 };
    case MAP_JUNGLE: return (Color) { 35, 60, 40, 255 };
    case MAP_TIME:   return (Color) { 45, 20, 70, 255 };
    default:         return (Color) { 40, 32, 55, 255 };
    }
}

// ---------------------------------------------------------------------------
// Mountains / hills / clouds / rocks
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

static void DrawSoftCloud(float x, float y, float scale, float alpha,
    MapTheme theme)
{
    Color base;
    switch (theme) {
    case MAP_SNOW:   base = (Color){ 240, 245, 250, 255 }; break;
    case MAP_JUNGLE: base = (Color){ 230, 240, 220, 255 }; break;
    case MAP_TIME:   base = (Color){ 220, 200, 245, 255 }; break;
    default:         base = HSV(HueShift(0.0f) + 0.5f, 0.5f, 1.0f, 255); break;
    }
    Color c = Fade(base, alpha);
    DrawCircle((int)(x - 40 * scale), (int)y, (int)(26 * scale), c);
    DrawCircle((int)(x - 10 * scale), (int)(y - 12 * scale), (int)(34 * scale), c);
    DrawCircle((int)(x + 25 * scale), (int)(y - 6 * scale), (int)(30 * scale), c);
    DrawCircle((int)(x + 55 * scale), (int)(y + 4 * scale), (int)(22 * scale), c);
    DrawCircle((int)(x - 20 * scale), (int)(y + 10 * scale), (int)(22 * scale), c);
    DrawCircle((int)(x + 20 * scale), (int)(y + 12 * scale), (int)(24 * scale), c);
}

static void DrawCloudBand(Camera2D* camera, float scrollSpeed,
    float baseY, float spreadY,
    int count, float scale, float alpha,
    MapTheme theme)
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

            DrawSoftCloud(floorf(x), floorf(y), sc, alpha, theme);
        }
    }
}

static void DrawForegroundRocks(Camera2D* camera, float scrollSpeed,
    MapTheme theme)
{
    float offset = -(float)GetTime() * scrollSpeed;

    float baseY = 1020.0f;

    Color rockColor = ThemeRock(theme);
    Color rockTop = ThemeRockTop(theme);

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
// Snowfall (SNOW only)
// ---------------------------------------------------------------------------

static void DrawSnowfall(Camera2D* camera) {
    float vx, vy, vw, vh;
    GetViewRect(camera, &vx, &vy, &vw, &vh);

    float time = (float)GetTime();
    float tileW = 1200.0f;
    float tileH = 1200.0f;

    int minTX = (int)floorf(vx / tileW) - 1;
    int maxTX = (int)floorf((vx + vw) / tileW) + 1;
    int minTY = (int)floorf(vy / tileH) - 1;
    int maxTY = (int)floorf((vy + vh) / tileH) + 1;

    for (int ty = minTY; ty <= maxTY; ty++) {
        for (int tx = minTX; tx <= maxTX; tx++) {
            unsigned int base = Hash((unsigned int)(tx * 733 ^ ty * 911));
            for (int i = 0; i < 30; i++) {
                unsigned int s = base + (unsigned int)(i * 2654435761u);
                float ox = Rand01(s) * tileW;
                float oy = Rand01(s ^ 0xDEADBEEFu) * tileH;
                float speed = 40.0f + Rand01(s ^ 0xBEEFCAFEu) * 60.0f;
                float sway = sinf(time * 1.5f + (float)i) * 12.0f;

                float sx = ox + sway + tx * tileW;
                float sy = fmodf(oy + time * speed, tileH) + ty * tileH;
                sy = fmodf(sy - vy, tileH) + vy;

                float r = 1.0f + Rand01(s ^ 0x12345678u) * 1.5f;
                DrawCircle((int)sx, (int)sy, r, Fade(WHITE, 0.8f));
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Master background
// ---------------------------------------------------------------------------

void DrawLevelBackground(Level* level, Camera2D* camera) {
    DrawSky(camera, level->theme);
    DrawAurora(camera, level->theme);
    DrawStars(camera, level->theme);
    DrawSun(level, camera);

    if (level->theme == MAP_NORMAL) {
        DrawAcidOverlay(camera);
    }

    DrawCloudBand(camera, 8.0f, 420.0f, 120.0f, 3, 1.6f, 0.90f, level->theme);

    DrawMountainRidge(camera, 15.0f, 760.0f, 220.0f, 900.0f,
        ThemeMountainFar(level->theme));

    DrawMountainRidge(camera, 30.0f, 830.0f, 260.0f, 750.0f,
        ThemeMountainNear(level->theme));

    DrawCloudBand(camera, 45.0f, 480.0f, 90.0f, 2, 1.9f, 0.95f, level->theme);

    DrawHillBand(camera, 60.0f, 880.0f, 150.0f,
        ThemeHillFar(level->theme),
        ThemeTree(level->theme),
        true);

    DrawHillBand(camera, 90.0f, 930.0f, 170.0f,
        ThemeHillNear(level->theme),
        ThemeTree(level->theme),
        false);

    DrawCloudBand(camera, 110.0f, 600.0f, 80.0f, 2, 2.2f, 0.85f, level->theme);

    DrawForegroundRocks(camera, 150.0f, level->theme);
}

void DrawLevelForeground(Level* level, Camera2D* camera) {
    if (level->theme == MAP_SNOW) {
        DrawSnowfall(camera);
    }
}

// ---------------------------------------------------------------------------
// Platform rendering - themed
// ---------------------------------------------------------------------------

typedef struct PlatformPalette {
    Color dirtTop, dirtBottom;
    Color grassTop, grassBottom, tuft;
    Color highlight;
    Color outline;
    Color speckle, speckleHi;
    bool  flowers;
} PlatformPalette;

static PlatformPalette GetPlatformPalette(MapTheme theme) {
    PlatformPalette p;
    switch (theme) {
    case MAP_SNOW:
        p.dirtTop = (Color){ 200, 220, 240, 255 };
        p.dirtBottom = (Color){ 130, 160, 200, 255 };
        p.grassTop = (Color){ 245, 250, 255, 255 };
        p.grassBottom = (Color){ 200, 220, 240, 255 };
        p.tuft = (Color){ 230, 240, 250, 255 };
        p.highlight = (Color){ 255, 255, 255, 200 };
        p.outline = (Color){ 40,  60,  90, 255 };
        p.speckle = (Color){ 130, 160, 200, 200 };
        p.speckleHi = (Color){ 220, 235, 250, 220 };
        p.flowers = false;
        break;
    case MAP_JUNGLE:
        p.dirtTop = (Color){ 90,  60,  35, 255 };
        p.dirtBottom = (Color){ 45,  30,  18, 255 };
        p.grassTop = (Color){ 70, 170,  70, 255 };
        p.grassBottom = (Color){ 30, 110,  45, 255 };
        p.tuft = (Color){ 50, 140,  55, 255 };
        p.highlight = (Color){ 200, 255, 180, 200 };
        p.outline = (Color){ 20,  35,  20, 255 };
        p.speckle = (Color){ 40,  28,  18, 200 };
        p.speckleHi = (Color){ 120,  85,  55, 180 };
        p.flowers = true;
        break;
    case MAP_TIME:
        p.dirtTop = (Color){ 90,  50, 130, 255 };
        p.dirtBottom = (Color){ 40,  20,  70, 255 };
        p.grassTop = (Color){ 200, 140, 240, 255 };
        p.grassBottom = (Color){ 130,  80, 190, 255 };
        p.tuft = (Color){ 220, 170, 255, 255 };
        p.highlight = (Color){ 255, 220, 255, 200 };
        p.outline = (Color){ 20,  10,  40, 255 };
        p.speckle = (Color){ 30,  15,  60, 200 };
        p.speckleHi = (Color){ 180, 130, 230, 180 };
        p.flowers = true;
        break;
    default: // MAP_NORMAL
    {
        float h = HueShift(0.0f);
        p.dirtTop = HSV(h + 0.00f, 0.85f, 0.80f, 255);
        p.dirtBottom = HSV(h + 0.10f, 0.85f, 0.45f, 255);
        p.grassTop = HSV(h + 0.50f, 0.85f, 1.00f, 255);
        p.grassBottom = HSV(h + 0.40f, 0.85f, 0.75f, 255);
        p.tuft = HSV(h + 0.50f, 0.90f, 1.00f, 255);
        p.highlight = HSV(h + 0.50f, 0.20f, 1.00f, 200);
        p.outline = HSV(h + 0.00f, 0.90f, 0.25f, 255);
        p.speckle = HSV(h + 0.85f, 0.90f, 0.55f, 200);
        p.speckleHi = HSV(h + 0.15f, 0.60f, 1.00f, 220);
        p.flowers = true;
    }
    break;
    }
    return p;
}

static void DrawFlower(int fx, int fy, Color petal, Color core) {
    DrawRectangle(fx, fy, 1, 4, (Color) { 60, 130, 55, 255 });
    DrawCircle(fx - 2, fy - 1, 2, petal);
    DrawCircle(fx + 2, fy - 1, 2, petal);
    DrawCircle(fx, fy - 3, 2, petal);
    DrawCircle(fx, fy + 1, 2, petal);
    DrawCircle(fx, fy - 1, 1, core);
}

static void DrawPlatform(Rectangle r, PlatformPalette p) {
    DrawRectangle((int)r.x + 4, (int)(r.y + r.height),
        (int)r.width, 6, (Color) { 0, 0, 0, 60 });
    DrawRectangle((int)r.x + 6, (int)(r.y + r.height + 4),
        (int)r.width - 4, 4, (Color) { 0, 0, 0, 35 });

    DrawRectangleGradientV((int)r.x, (int)r.y,
        (int)r.width, (int)r.height,
        p.dirtTop, p.dirtBottom);

    int grassH = (r.height >= 24) ? 8 : (int)r.height;
    if (grassH > 0) {
        DrawRectangleGradientV((int)r.x, (int)r.y,
            (int)r.width, grassH,
            p.grassTop, p.grassBottom);

        for (int i = 0; i < (int)r.width; i += 10) {
            int h = (i * 73856093) ^ (int)r.x;
            float t = (float)((h >> 4) & 0x3F) / 63.0f;
            int tuftH = 2 + (int)(t * 3.0f);
            DrawRectangle((int)r.x + i + 2,
                (int)r.y - tuftH,
                2, tuftH, p.tuft);
        }
    }

    DrawRectangle((int)r.x, (int)r.y, (int)r.width, 2, p.highlight);

    DrawRectangleLinesEx(r, 2, p.outline);

    if (r.height >= 30) {
        for (int i = 0; i < (int)r.width; i += 24) {
            int h = (i * 2654435761u) ^ (int)(r.x * 7);
            float t = (float)((h >> 3) & 0xFF) / 255.0f;
            int dx = (int)r.x + i + (int)(t * 20.0f);
            int dy = (int)r.y + 12 + (int)(t * (r.height - 20));
            int r2 = 1 + ((h >> 8) & 1);
            DrawCircle(dx, dy, r2, p.speckle);
            DrawCircle(dx - 1, dy - 1, 1, p.speckleHi);
        }
    }

    if (p.flowers) {
        for (int i = 6; i < (int)r.width - 6; i += 26) {
            int h = (i * 40503) ^ (int)(r.x * 13);
            float t = (float)((h >> 4) & 0xFF) / 255.0f;
            if (t > 0.55f) {
                int fx = (int)r.x + i + (int)(t * 14.0f);
                int fy = (int)r.y - 2;

                int pick = (h >> 12) & 3;
                Color petal;
                switch (pick) {
                case 0: petal = (Color){ 255, 200, 220, 255 }; break;
                case 1: petal = (Color){ 255, 235, 160, 255 }; break;
                case 2: petal = (Color){ 200, 220, 255, 255 }; break;
                default: petal = (Color){ 230, 200, 255, 255 }; break;
                }
                DrawFlower(fx, fy, petal, (Color) { 255, 245, 120, 255 });
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Draw: fill the ground below the floor, then all platforms
// ---------------------------------------------------------------------------

void DrawLevel(Level* level) {
    PlatformPalette p = GetPlatformPalette(level->theme);

    // Fill everything below the floor so no sky shows through.
    // Collision still uses the original 50-tall floor rect.
    {
        Rectangle floor = level->platforms[0];
        float bottomY = level->worldHeight + 400.0f;
        float extraH = bottomY - (floor.y + floor.height);
        if (extraH > 0.0f) {
            DrawRectangleGradientV((int)floor.x, (int)floor.y,
                (int)floor.width,
                (int)(floor.height + extraH),
                p.dirtTop, p.dirtBottom);
        }
    }

    // Draw all platforms (including the floor) on top.
    for (int i = 0; i < level->platformCount; i++) {
        if (i == 1 || i == 2 || i == 3) continue;
        DrawPlatform(level->platforms[i], p);
    }
}

// ---------------------------------------------------------------------------
// Random spawn
// ---------------------------------------------------------------------------

Vector2 GetRandomSpawnPoint(Level* level, float playerWidth, float playerHeight) {
    int validCount = 0;
    int validIndices[MAX_PLATFORMS];
    for (int i = 0; i < level->platformCount; i++) {
        if (i == 1 || i == 2 || i == 3) continue;
        if (level->platforms[i].width < playerWidth + 10.0f) continue;
        validIndices[validCount++] = i;
    }

    if (validCount == 0) return level->playerStart;

    int idx = validIndices[GetRandomValue(0, validCount - 1)];
    Rectangle r = level->platforms[idx];

    float x = (float)GetRandomValue((int)r.x + 5, (int)(r.x + r.width - playerWidth - 5));
    float y = r.y - playerHeight;

    return (Vector2) { x, y };
}