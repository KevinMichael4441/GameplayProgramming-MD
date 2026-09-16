#ifndef LEVEL_H
#define LEVEL_H

#include "raylib.h"

#define MAX_PLATFORMS 32
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
    float speed;
    float moveSpeed;        // horizontal speed magnitude
    int direction;          // -1 = left, +1 = right
    float directionTimer;   // time until next possible direction change
    float gravity;          // for settling onto platforms
    bool onGround;          // whether currently standing on something
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