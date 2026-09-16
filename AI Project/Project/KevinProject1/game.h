#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "level.h"

typedef struct Game {
    Player player;    // Player 1 (arrow keys, red)
    Player player2;   // Player 2 (WASD, blue)
    Level level;
    Camera2D camera;
    bool gameWon;
    bool gameOver;
    int score;
    float jumpscareTimer;
    float deathTimer;
    Vector2 deathPosition;
} Game;

void InitGame(Game* game);
void UpdateGame(Game* game, float deltaTime);
void DrawGame(Game* game);
void CleanupGame(Game* game);

#endif