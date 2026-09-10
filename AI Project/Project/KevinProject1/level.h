#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"

#define MAX_PLATFORMS 20

typedef struct Level {
    Rectangle platforms[MAX_PLATFORMS];
    int platformCount;
    Rectangle goal;
    Color backgroundColor;
} Level;

void InitLevel(Level* level);
void DrawLevel(Level* level);

#endif
