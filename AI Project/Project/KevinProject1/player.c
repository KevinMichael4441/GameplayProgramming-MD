#include "player.h"

void InitPlayer(Player* player, float x, float y) {
    player->position = (Vector2){ x, y };
    player->velocity = (Vector2){ 0, 0 };
    player->rec = (Rectangle){ x, y, 30, 30 };
    player->isJumping = false;
    player->canJump = true;
    player->jumpForce = -400.0f;
    player->moveSpeed = 200.0f;
    player->gravity = 980.0f;
    player->color = RED;
}

void UpdatePlayer(Player* player, Level* level, float deltaTime) {
    // Horizontal movement
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->velocity.x = -player->moveSpeed;
    }
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->velocity.x = player->moveSpeed;
    }
    else {
        player->velocity.x = 0;
    }

    // Jumping
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        && player->canJump) {
        player->velocity.y = player->jumpForce;
        player->isJumping = true;
        player->canJump = false;
    }

    // Apply gravity
    player->velocity.y += player->gravity * deltaTime;

    // Update position
    player->position.x += player->velocity.x * deltaTime;

    // Check horizontal collisions
    player->rec.x = player->position.x;
    player->rec.y = player->position.y;

    for (int i = 0; i < level->platformCount; i++) {
        if (CheckCollisionRecs(player->rec, level->platforms[i])) {
            if (player->velocity.x > 0) {
                // Moving right, hit left side of platform
                player->position.x = level->platforms[i].x - player->rec.width;
            }
            else if (player->velocity.x < 0) {
                // Moving left, hit right side of platform
                player->position.x = level->platforms[i].x + level->platforms[i].width;
            }
            player->velocity.x = 0;
            player->rec.x = player->position.x;
        }
    }

    // Vertical movement
    player->position.y += player->velocity.y * deltaTime;
    player->rec.y = player->position.y;

    // Check vertical collisions
    player->canJump = false;
    for (int i = 0; i < level->platformCount; i++) {
        if (CheckCollisionRecs(player->rec, level->platforms[i])) {
            if (player->velocity.y > 0) {
                // Falling, hit top of platform
                player->position.y = level->platforms[i].y - player->rec.height;
                player->velocity.y = 0;
                player->isJumping = false;
                player->canJump = true;
            }
            else if (player->velocity.y < 0) {
                // Moving up, hit bottom of platform
                player->position.y = level->platforms[i].y + level->platforms[i].height;
                player->velocity.y = 0;
            }
            player->rec.y = player->position.y;
        }
    }

    // Keep player in bounds
    if (player->position.x < 0) {
        player->position.x = 0;
        player->rec.x = 0;
    }
    if (player->position.x + player->rec.width > 2000) {
        player->position.x = 2000 - player->rec.width;
        player->rec.x = player->position.x;
    }

    // Update rectangle position
    player->rec.x = player->position.x;
    player->rec.y = player->position.y;
}

void DrawPlayer(Player* player) {
    // Draw player body
    DrawRectangleRec(player->rec, player->color);

    // Draw eyes
    DrawCircle(player->position.x + 8, player->position.y + 10, 4, WHITE);
    DrawCircle(player->position.x + 22, player->position.y + 10, 4, WHITE);
    DrawCircle(player->position.x + 9, player->position.y + 10, 2, BLACK);
    DrawCircle(player->position.x + 23, player->position.y + 10, 2, BLACK);

    // Draw mouth (happy when jumping)
    if (player->isJumping) {
        DrawCircle(player->position.x + 15, player->position.y + 20, 5, WHITE);
    }
    else {
        DrawLine(player->position.x + 10, player->position.y + 22,
            player->position.x + 20, player->position.y + 22, WHITE);
    }
}