#include "game.h"

Game Game_Create(void)
{
    Game game = { 0 };

    game.player.position = (Vector2){ 640.0f, 360.0f };
    game.player.speed = 300.0f;

    return game;
}

void Game_Update(Game * game)
{
    Player* player = &game->player;

    float dt = GetFrameTime();

    if (IsKeyDown(KEY_W))
        player->position.y -= player->speed * dt;

    if (IsKeyDown(KEY_S))
        player->position.y += player->speed * dt;

    if (IsKeyDown(KEY_A))
        player->position.x -= player->speed * dt;

    if (IsKeyDown(KEY_D))
        player->position.x += player->speed * dt;
}



void Game_Draw(const Game* game)
{
    DrawCircleV(
        game->player.position,
        10.0f,
        RED
    );
}


