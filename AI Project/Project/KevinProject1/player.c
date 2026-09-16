#include "player.h"
#include <math.h>

void InitPlayer(Player* player, float x, float y, InputMode input, Color color) {
    player->position = (Vector2){ x, y };
    player->velocity = (Vector2){ 0, 0 };
    player->rec = (Rectangle){ x, y, 30, 30 };
    player->isJumping = false;
    player->canJump = true;
    player->jumpForce = PLAYER_BASE_JUMP;
    player->moveSpeed = PLAYER_BASE_SPEED;
    player->gravity = 980.0f;
    player->color = color;
    player->input = input;

    player->jumpedThisFrame = false;
    player->landedThisFrame = false;
    player->landImpact = 0.0f;

    player->speedBoostTimer = 0.0f;
    player->jumpBoostTimer = 0.0f;
    player->shieldTimer = 0.0f;
}

void UpdatePlayer(Player* player, Level* level, float deltaTime) {
    player->jumpedThisFrame = false;
    player->landedThisFrame = false;

    // Tick buff timers
    if (player->speedBoostTimer > 0.0f) {
        player->speedBoostTimer -= deltaTime;
        if (player->speedBoostTimer < 0.0f) player->speedBoostTimer = 0.0f;
    }
    if (player->jumpBoostTimer > 0.0f) {
        player->jumpBoostTimer -= deltaTime;
        if (player->jumpBoostTimer < 0.0f) player->jumpBoostTimer = 0.0f;
    }
    if (player->shieldTimer > 0.0f) {
        player->shieldTimer -= deltaTime;
        if (player->shieldTimer < 0.0f) player->shieldTimer = 0.0f;
    }

    float effectiveSpeed = PLAYER_BASE_SPEED;
    if (player->speedBoostTimer > 0.0f) effectiveSpeed *= 1.8f;

    float effectiveJump = PLAYER_BASE_JUMP;
    if (player->jumpBoostTimer > 0.0f) effectiveJump *= 1.35f;

    int keyLeft, keyRight, keyJump;
    if (player->input == INPUT_ARROWS) {
        keyLeft = KEY_LEFT; keyRight = KEY_RIGHT; keyJump = KEY_UP;
    }
    else {
        keyLeft = KEY_A; keyRight = KEY_D; keyJump = KEY_W;
    }

    if (IsKeyDown(keyLeft)) player->velocity.x = -effectiveSpeed;
    else if (IsKeyDown(keyRight)) player->velocity.x = effectiveSpeed;
    else player->velocity.x = 0;

    bool jumpPressed = IsKeyPressed(keyJump);
    if (player->input == INPUT_ARROWS && IsKeyPressed(KEY_SPACE)) {
        jumpPressed = true;
    }

    if (jumpPressed && player->canJump) {
        player->velocity.y = effectiveJump;
        player->isJumping = true;
        player->canJump = false;
        player->jumpedThisFrame = true;
    }

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
                const float MIN_LAND_SPEED = 150.0f;

                bool wasFalling = (player->velocity.y > MIN_LAND_SPEED) ||
                    player->isJumping;

                if (wasFalling) {
                    float impact = player->velocity.y / 800.0f;
                    if (impact > 1.0f) impact = 1.0f;
                    if (impact < 0.3f) impact = 0.3f;

                    player->landImpact = impact;
                    player->landedThisFrame = true;
                }

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
    Vector2 c = {
        player->position.x + player->rec.width * 0.5f,
        player->position.y + player->rec.height * 0.5f
    };

    // Drop shadow
    float shadowScale = player->isJumping ? 0.55f : 1.0f;
    DrawEllipse((int)c.x,
        (int)(player->position.y + player->rec.height + 2),
        14.0f * shadowScale, 4.0f * shadowScale,
        (Color) {
        0, 0, 0, 80
    });

    // Idle / walk bob
    float bob = 0.0f;
    if (!player->isJumping) {
        if (player->velocity.x != 0.0f) {
            bob = sinf((float)GetTime() * 12.0f) * 1.5f;
        }
        else {
            bob = sinf((float)GetTime() * 3.0f) * 0.6f;
        }
    }

    Vector2 bodyCenter = { c.x, c.y + bob };

    // Body
    float bodyRX = 13.0f;
    float bodyRY = 15.0f;

    DrawEllipse((int)bodyCenter.x, (int)bodyCenter.y,
        bodyRX, bodyRY,
        (Color) {
        (unsigned char)(player->color.r * 0.7f),
            (unsigned char)(player->color.g * 0.7f),
            (unsigned char)(player->color.b * 0.7f),
            255
    });

    DrawEllipse((int)bodyCenter.x, (int)(bodyCenter.y - 2.0f),
        bodyRX * 0.95f, bodyRY * 0.9f,
        player->color);

    // Highlight crescent
    DrawCircle((int)(bodyCenter.x - 4.0f), (int)(bodyCenter.y - 7.0f),
        3.5f, Fade(WHITE, 0.35f));

    // Eyes
    float eyeDX = (player->velocity.x > 0.0f) ? 1.0f
        : (player->velocity.x < 0.0f) ? -1.0f : 0.0f;
    float eyeDY = (player->velocity.y > 0.0f) ? 1.0f
        : (player->velocity.y < 0.0f) ? -1.0f : 0.0f;

    Vector2 eyeL = { bodyCenter.x - 5.0f, bodyCenter.y - 3.0f };
    Vector2 eyeR = { bodyCenter.x + 5.0f, bodyCenter.y - 3.0f };

    DrawCircle((int)eyeL.x, (int)eyeL.y, 3.5f, WHITE);
    DrawCircle((int)eyeR.x, (int)eyeR.y, 3.5f, WHITE);

    DrawCircle((int)(eyeL.x + eyeDX), (int)(eyeL.y + eyeDY), 1.8f, BLACK);
    DrawCircle((int)(eyeR.x + eyeDX), (int)(eyeR.y + eyeDY), 1.8f, BLACK);

    DrawCircle((int)(eyeL.x + eyeDX - 0.6f), (int)(eyeL.y + eyeDY - 0.6f),
        0.7f, WHITE);
    DrawCircle((int)(eyeR.x + eyeDX - 0.6f), (int)(eyeR.y + eyeDY - 0.6f),
        0.7f, WHITE);

    // Mouth
    float mouthY = bodyCenter.y + 6.0f;
    if (player->isJumping) {
        DrawEllipse((int)bodyCenter.x, (int)(mouthY + 1.0f),
            3.5f, 2.5f, (Color) { 60, 20, 20, 255 });
    }
    else if (player->velocity.x != 0.0f) {
        DrawCircle((int)(bodyCenter.x - 2.0f), (int)mouthY, 1.0f, BLACK);
        DrawCircle((int)bodyCenter.x, (int)(mouthY + 1.0f), 1.0f, BLACK);
        DrawCircle((int)(bodyCenter.x + 2.0f), (int)mouthY, 1.0f, BLACK);
    }
    else {
        DrawCircle((int)bodyCenter.x, (int)mouthY, 1.0f, BLACK);
    }

    // Shoes
    float footOffset = 0.0f;
    if (!player->isJumping && player->velocity.x != 0.0f) {
        footOffset = sinf((float)GetTime() * 12.0f) * 2.0f;
    }
    float footY = bodyCenter.y + bodyRY - 1.0f;
    DrawEllipse((int)(bodyCenter.x - 5.0f), (int)(footY + footOffset),
        4.0f, 2.5f, (Color) { 40, 30, 30, 255 });
    DrawEllipse((int)(bodyCenter.x + 5.0f), (int)(footY - footOffset),
        4.0f, 2.5f, (Color) { 40, 30, 30, 255 });

    // Head tuft
    Color accent = (player->input == INPUT_ARROWS)
        ? (Color) { 255, 220, 120, 255 }
    : (Color) { 140, 220, 255, 255 };
    Vector2 tuftBase = { bodyCenter.x, bodyCenter.y - bodyRY + 2.0f };
    DrawCircle((int)(tuftBase.x - 3.0f), (int)(tuftBase.y - 2.0f), 2.5f, accent);
    DrawCircle((int)tuftBase.x, (int)(tuftBase.y - 4.0f), 3.0f, accent);
    DrawCircle((int)(tuftBase.x + 3.0f), (int)(tuftBase.y - 2.0f), 2.5f, accent);
}