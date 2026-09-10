#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "level.h"

typedef struct Player {
    Vector2 position;
    Vector2 velocity;
    Rectangle rec;
    bool isJumping;
    bool canJump;
    float jumpForce;
    float moveSpeed;
    float gravity;
    Color color;
} Player;

void InitPlayer(Player* player, float x, float y);
void UpdatePlayer(Player* player, Level* level, float deltaTime);
void DrawPlayer(Player* player);

#endif