#include "game.h"
#include <stdio.h>
#include <math.h>

void InitGame(Game* game) {
    // Initialize player
    InitPlayer(&game->player, 100, 400);

    // Initialize level
    InitLevel(&game->level);

    // Initialize camera
    game->camera.target = (Vector2){ game->player.position.x, game->player.position.y };
    game->camera.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;

    game->gameWon = false;
    game->gameOver = false;
    game->score = 0;
    game->jumpscareTimer = 0.0f;
    game->deathTimer = 0.0f;
    game->deathPosition = (Vector2){ 0, 0 };
}

void UpdateGame(Game* game, float deltaTime) {
    if (!game->gameWon && !game->gameOver) {
        // Update player
        UpdatePlayer(&game->player, &game->level, deltaTime);

        // Update camera to follow player
        game->camera.target = (Vector2){
            game->player.position.x,
            game->player.position.y - 50
        };

        // Update coins rotation and check collection
        for (int i = 0; i < game->level.coinCount; i++) {
            Coin* coin = &game->level.coins[i];
            if (!coin->collected) {
                coin->rotation += deltaTime * 3.0f;

                Rectangle coinRec = {
                    coin->position.x - 10,
                    coin->position.y - 10,
                    20, 20
                };
                if (CheckCollisionRecs(game->player.rec, coinRec)) {
                    coin->collected = true;
                    game->score += 10;
                }
            }
        }

        // Update enemies
        for (int i = 0; i < game->level.enemyCount; i++) {
            Enemy* enemy = &game->level.enemies[i];

            if (enemy->movingRight) {
                enemy->position.x += enemy->speed * deltaTime;
                if (enemy->position.x >= enemy->patrolEnd) {
                    enemy->movingRight = false;
                }
            }
            else {
                enemy->position.x -= enemy->speed * deltaTime;
                if (enemy->position.x <= enemy->patrolStart) {
                    enemy->movingRight = true;
                }
            }
            enemy->rec.x = enemy->position.x;
            enemy->rec.y = enemy->position.y;

            // Check collision with player
            if (CheckCollisionRecs(game->player.rec, enemy->rec)) {
                game->gameOver = true;
                game->deathPosition = game->player.position;
                game->deathTimer = 0.0f;
                game->jumpscareTimer = 0.0f;
            }
        }

        // Check win condition
        if (CheckCollisionRecs(game->player.rec, game->level.goal)) {
            game->gameWon = true;
        }

        // Check if player fell off the world
        if (game->player.position.y > 800) {
            game->gameOver = true;
            game->deathPosition = game->player.position;
            game->deathTimer = 0.0f;
            game->jumpscareTimer = 0.0f;
        }
    }
    else if (game->gameOver) {
        game->deathTimer += deltaTime;
        game->jumpscareTimer += deltaTime;

        if (game->deathTimer > 2.0f && IsKeyPressed(KEY_R)) {
            InitGame(game);
        }
    }
    else if (game->gameWon) {
        if (IsKeyPressed(KEY_R)) {
            InitGame(game);
        }
    }
}

void DrawJumpscare(Game* game) {
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    // Flash effect
    if ((int)(game->jumpscareTimer * 10) % 2 == 0) {
        DrawRectangle(0, 0, screenW, screenH, (Color) { 50, 0, 0, 200 });
    }
    else {
        DrawRectangle(0, 0, screenW, screenH, (Color) { 0, 0, 0, 200 });
    }

    // Draw creepy face
    float faceX = screenW / 2;
    float faceY = screenH / 2;
    float faceScale = 1.0f + game->jumpscareTimer * 2.0f;

    DrawCircle(faceX, faceY, 150 * faceScale, (Color) { 200, 180, 180, 255 });

    float eyeOffset = 60 * faceScale;
    float eyeSize = 40 * faceScale;

    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize, (Color) { 255, 255, 255, 255 });
    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize * 0.6f, (Color) { 200, 0, 0, 255 });
    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize * 0.3f, BLACK);

    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize, (Color) { 255, 255, 255, 255 });
    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize * 0.6f, (Color) { 200, 0, 0, 255 });
    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize * 0.3f, BLACK);

    float mouthWidth = 80 * faceScale;
    float mouthHeight = 40 * faceScale;

    DrawEllipse(faceX, faceY + 60 * faceScale, mouthWidth, mouthHeight, (Color) { 100, 0, 0, 255 });

    for (int i = 0; i < 6; i++) {
        float toothX = faceX - mouthWidth + (i * mouthWidth / 3);
        DrawTriangle(
            (Vector2) {
            toothX, faceY + 40 * faceScale
        },
            (Vector2) {
            toothX + 20 * faceScale, faceY + 60 * faceScale
        },
            (Vector2) {
            toothX + 40 * faceScale, faceY + 40 * faceScale
        },
            (Color) {
            255, 255, 220, 255
        }
        );
    }

    for (int i = 0; i < 5; i++) {
        float dripX = faceX - 100 * faceScale + i * 50 * faceScale;
        float dripY = faceY - 150 * faceScale;
        float dripLength = 30 * faceScale + sinf(game->jumpscareTimer * 5 + i) * 10;
        DrawRectangle(dripX, dripY, 10 * faceScale, dripLength, (Color) { 150, 0, 0, 255 });
        DrawCircle(dripX + 5 * faceScale, dripY + dripLength, 5 * faceScale, (Color) { 150, 0, 0, 255 });
    }

    if (game->jumpscareTimer > 0.5f) {
        float shakeAmount = (game->jumpscareTimer - 0.5f) * 20;
        float shakeX = sinf(game->jumpscareTimer * 50) * shakeAmount;
        float shakeY = cosf(game->jumpscareTimer * 50) * shakeAmount;

        for (int i = 0; i < 50; i++) {
            float x = (float)(rand() % (int)screenW) + shakeX;
            float y = (float)(rand() % (int)screenH) + shakeY;
            DrawPixel(x, y, WHITE);
        }
    }

    // "YOU DIED" text - moved up
    if (game->jumpscareTimer > 1.0f) {
        const char* diedText = "YOU DIED";
        int textWidth = MeasureText(diedText, 60);
        DrawText(diedText,
            screenW / 2 - textWidth / 2,
            screenH / 2 - 250,   // moved up (was screenH / 2 + 200)
            60, (Color) { 200, 0, 0, 255 });
    }

    // "Press R to restart" - appears 2/3 of the way through the jumpscare
    // Full jumpscare window is roughly deathTimer > 2.0f to restart,
    // so 2/3 of that is about 1.33 seconds.
    if (game->jumpscareTimer > 1.33f) {
        const char* restartText = "Press R to restart";
        int restartWidth = MeasureText(restartText, 30);
        DrawText(restartText,
            screenW / 2 - restartWidth / 2,
            screenH / 2 - 170,   // moved up (was screenH / 2 + 280)
            30, (Color) {0xE2, 0x57, 0x10, 0xFF});
    }
}

void DrawGame(Game* game) {
    if (game->gameOver) {
        DrawJumpscare(game);
        return;
    }

    BeginMode2D(game->camera);

    DrawLevel(&game->level);
    DrawPlayer(&game->player);

    EndMode2D();

    // Draw UI
    DrawText("Use ARROW KEYS or A/D to move", 10, 10, 20, DARKGRAY);
    DrawText("SPACE to jump", 10, 35, 20, DARKGRAY);

    // Draw score with a solid background for readability
    const char* scoreLabel = "SCORE";
    DrawRectangle(8, 58, 180, 34, (Color) { 0, 0, 0, 120 });
    DrawRectangleLines(8, 58, 180, 34, GOLD);

    // Label
    DrawText(scoreLabel, 16, 62, 20, GOLD);

    // Score value - use a fixed buffer and snprintf for safety
    char scoreText[32];
    if (game->score <= 0) {
        // Placeholder text for zero score
        snprintf(scoreText, sizeof(scoreText), "0000");
    }
    else {
        snprintf(scoreText, sizeof(scoreText), "%04d", game->score);
    }

    int scoreValueWidth = MeasureText(scoreText, 24);
    DrawText(scoreText, 180 - scoreValueWidth - 8, 62, 24, WHITE);

    if (game->gameWon) {
        const char* winText = "YOU WIN! Press R to restart";
        int textWidth = MeasureText(winText, 40);
        DrawText(winText,
            GetScreenWidth() / 2 - textWidth / 2,
            GetScreenHeight() / 2 - 20,
            40, GOLD);

        char finalScore[48];
        if (game->score <= 0) {
            snprintf(finalScore, sizeof(finalScore), "Final Score: 0000");
        }
        else {
            snprintf(finalScore, sizeof(finalScore), "Final Score: %04d", game->score);
        }
        int finalWidth = MeasureText(finalScore, 30);
        DrawText(finalScore,
            GetScreenWidth() / 2 - finalWidth / 2,
            GetScreenHeight() / 2 + 40,
            30, WHITE);
    }
}

void CleanupGame(Game* game) {
    (void)game;
}