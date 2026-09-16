#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include "level.h"

typedef enum InputMode {
    INPUT_ARROWS = 0,
    INPUT_WASD
} InputMode;

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
    InputMode input;
} Player;

void InitPlayer(Player* player, float x, float y, InputMode input, Color color);
void UpdatePlayer(Player* player, Level* level, float deltaTime);
void DrawPlayer(Player* player);

#endif