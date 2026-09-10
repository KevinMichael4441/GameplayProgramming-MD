#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "level.h"

#define MAX_PLATFORMS 20

typedef struct Game {
    Player player;
    Level level;
    Camera2D camera;
    bool gameWon;
} Game;

void InitGame(Game* game);
void UpdateGame(Game* game, float deltaTime);
void DrawGame(Game* game);
void CleanupGame(Game* game);

#endif