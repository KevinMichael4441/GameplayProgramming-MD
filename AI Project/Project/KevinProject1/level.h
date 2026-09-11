#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"

#define MAX_PLATFORMS 20
#define MAX_COINS 50
#define MAX_ENEMIES 10

typedef struct Coin {
    Vector2 position;
    bool collected;
    float rotation;
} Coin;

typedef struct Enemy {
    Vector2 position;
    Vector2 velocity;
    Rectangle rec;
    float patrolStart;
    float patrolEnd;
    float speed;
    bool movingRight;
    Color color;
} Enemy;

typedef struct Level {
    Rectangle platforms[MAX_PLATFORMS];
    int platformCount;
    Rectangle goal;
    Color backgroundColor;
    Coin coins[MAX_COINS];
    int coinCount;
    Enemy enemies[MAX_ENEMIES];
    int enemyCount;
} Level;

void InitLevel(Level* level);
void DrawLevel(Level* level);

#endif