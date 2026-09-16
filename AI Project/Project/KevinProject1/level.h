#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"

#define MAX_PLATFORMS 128

typedef struct Level {
    Rectangle platforms[MAX_PLATFORMS];
    int platformCount;
    Color backgroundColor;
    Vector2 playerStart;
    float worldWidth;
    float worldHeight;
    float bgTime;        // <-- accumulates deltaTime, drives background animation
} Level;

void InitLevel(Level* level);
void UpdateLevelBackground(Level* level, float deltaTime);
void DrawLevelBackground(Level* level, Camera2D* camera);
void DrawLevel(Level* level);

#endif