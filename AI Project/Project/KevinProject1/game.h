#ifndef GAME_H
#define GAME_H

#include "raylib.h"

typedef struct
{
    Vector2 position;
    float speed;
} Player;

typedef struct
{
    Player player;
} Game;

Game Game_Create(void);
void Game_Update(Game* game);
void Game_Draw(const Game* game);

#endif

