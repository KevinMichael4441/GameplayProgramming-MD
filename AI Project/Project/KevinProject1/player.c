#include "player.h"

void InitPlayer(Player* player, float x, float y, InputMode input, Color color) {
    player->position = (Vector2){ x, y };
    player->velocity = (Vector2){ 0, 0 };
    player->rec = (Rectangle){ x, y, 30, 30 };
    player->isJumping = false;
    player->canJump = true;
    player->jumpForce = -560.0f;
    player->moveSpeed = 200.0f;
    player->gravity = 980.0f;
    player->color = color;
    player->input = input;
}

void UpdatePlayer(Player* player, Level* level, float deltaTime) {
    // Pick the key set for this player
    int keyLeft, keyRight, keyJump;
    if (player->input == INPUT_ARROWS) {
        keyLeft = KEY_LEFT;
        keyRight = KEY_RIGHT;
        keyJump = KEY_UP;
    }
    else {
        keyLeft = KEY_A;
        keyRight = KEY_D;
        keyJump = KEY_W;
    }

    // Horizontal input
    if (IsKeyDown(keyLeft)) {
        player->velocity.x = -player->moveSpeed;
    }
    else if (IsKeyDown(keyRight)) {
        player->velocity.x = player->moveSpeed;
    }
    else {
        player->velocity.x = 0;
    }

    // Jump — WASD player uses W, arrows player uses Up.
    // Space also jumps for the arrows player as a courtesy.
    bool jumpPressed = IsKeyPressed(keyJump);
    if (player->input == INPUT_ARROWS && IsKeyPressed(KEY_SPACE)) {
        jumpPressed = true;
    }

    if (jumpPressed && player->canJump) {
        player->velocity.y = player->jumpForce;
        player->isJumping = true;
        player->canJump = false;
    }

    // Gravity
    player->velocity.y += player->gravity * deltaTime;

    // Horizontal move + collision
    player->position.x += player->velocity.x * deltaTime;
    player->rec.x = player->position.x;
    player->rec.y = player->position.y;

    for (int i = 0; i < level->platformCount; i++) {
        if (CheckCollisionRecs(player->rec, level->platforms[i])) {
            if (player->velocity.x > 0) {
                player->position.x = level->platforms[i].x - player->rec.width;
            }
            else if (player->velocity.x < 0) {
                player->position.x = level->platforms[i].x + level->platforms[i].width;
            }
            player->velocity.x = 0;
            player->rec.x = player->position.x;
        }
    }

    // Vertical move + collision
    player->position.y += player->velocity.y * deltaTime;
    player->rec.y = player->position.y;

    player->canJump = false;
    for (int i = 0; i < level->platformCount; i++) {
        if (CheckCollisionRecs(player->rec, level->platforms[i])) {
            if (player->velocity.y > 0) {
                player->position.y = level->platforms[i].y - player->rec.height;
                player->velocity.y = 0;
                player->isJumping = false;
                player->canJump = true;
            }
            else if (player->velocity.y < 0) {
                player->position.y = level->platforms[i].y + level->platforms[i].height;
                player->velocity.y = 0;
            }
            player->rec.y = player->position.y;
        }
    }

    // Clamp to world bounds
    if (player->position.x < 0) {
        player->position.x = 0;
        player->rec.x = 0;
        if (player->velocity.x < 0) player->velocity.x = 0;
    }
    if (player->position.x + player->rec.width > level->worldWidth) {
        player->position.x = level->worldWidth - player->rec.width;
        player->rec.x = player->position.x;
        if (player->velocity.x > 0) player->velocity.x = 0;
    }
    if (player->position.y < 0) {
        player->position.y = 0;
        player->rec.y = 0;
        if (player->velocity.y < 0) player->velocity.y = 0;
    }
    if (player->position.y + player->rec.height > level->worldHeight) {
        player->position.y = level->worldHeight - player->rec.height;
        player->rec.y = player->position.y;
        if (player->velocity.y > 0) player->velocity.y = 0;
        player->isJumping = false;
        player->canJump = true;
    }

    player->rec.x = player->position.x;
    player->rec.y = player->position.y;
}

void DrawPlayer(Player* player) {
    DrawRectangleRec(player->rec, player->color);

    DrawCircle(player->position.x + 8, player->position.y + 10, 4, WHITE);
    DrawCircle(player->position.x + 22, player->position.y + 10, 4, WHITE);
    DrawCircle(player->position.x + 9, player->position.y + 10, 2, BLACK);
    DrawCircle(player->position.x + 23, player->position.y + 10, 2, BLACK);

    if (player->isJumping) {
        DrawCircle(player->position.x + 15, player->position.y + 20, 5, WHITE);
    }
    else {
        DrawLine(player->position.x + 10, player->position.y + 22,
            player->position.x + 20, player->position.y + 22, WHITE);
    }
}