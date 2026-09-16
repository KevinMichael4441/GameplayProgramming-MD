#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"

#define MAX_PLATFORMS 128
#define TIMER_SECONDS 30.0f

typedef enum MapTheme {
    MAP_NORMAL = 0,
    MAP_SNOW,
    MAP_JUNGLE,
    MAP_TIME,
    MAP_THEME_COUNT
} MapTheme;

typedef struct Level {
    Rectangle platforms[MAX_PLATFORMS];
    int platformCount;
    Color backgroundColor;
    Vector2 playerStart;
    float worldWidth;
    float worldHeight;
    float bgTime;
    MapTheme theme;
} Level;

void InitLevel(Level* level, MapTheme theme);
void UpdateLevelBackground(Level* level, float deltaTime);
void DrawLevelBackground(Level* level, Camera2D* camera);
void DrawLevel(Level* level);
Vector2 GetRandomSpawnPoint(Level* level, float playerWidth, float playerHeight);
void DrawLevelBackground(Level* level, Camera2D* camera);
void DrawLevelForeground(Level* level, Camera2D* camera);
const char* GetMapThemeName(MapTheme theme);

#endif