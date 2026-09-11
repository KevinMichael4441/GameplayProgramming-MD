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

                // Check collision with player
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

            // Move enemy
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

            // Check collision with player (only if player is not invincible)
            if (CheckCollisionRecs(game->player.rec, enemy->rec)) {
                game->gameOver = true;
                game->deathPosition = game->player.position;
                game->deathTimer = 0.0f;
                game->jumpscareTimer = 0.0f;
            }
        }

        // Check win condition (reached the goal)
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
        // Update death animation timers
        game->deathTimer += deltaTime;
        game->jumpscareTimer += deltaTime;

        // Allow restart with R key after a short delay
        if (game->deathTimer > 2.0f && IsKeyPressed(KEY_R)) {
            InitGame(game);
        }
    }
    else if (game->gameWon) {
        // Allow restart with R key
        if (IsKeyPressed(KEY_R)) {
            InitGame(game);
        }
    }
}

void DrawJumpscare(Game* game) {
    // This creates a creepy jumpscare effect
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

    // Face outline (creepy pale face)
    DrawCircle(faceX, faceY, 150 * faceScale, (Color) { 200, 180, 180, 255 });

    // Eyes (large and creepy)
    float eyeOffset = 60 * faceScale;
    float eyeSize = 40 * faceScale;

    // Left eye
    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize, (Color) { 255, 255, 255, 255 });
    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize * 0.6f, (Color) { 200, 0, 0, 255 });
    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize * 0.3f, BLACK);

    // Right eye
    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize, (Color) { 255, 255, 255, 255 });
    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize * 0.6f, (Color) { 200, 0, 0, 255 });
    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize * 0.3f, BLACK);

    // Mouth (wide creepy smile)
    float mouthWidth = 80 * faceScale;
    float mouthHeight = 40 * faceScale;

    // Draw mouth as a wide creepy smile
    DrawEllipse(faceX, faceY + 60 * faceScale, mouthWidth, mouthHeight, (Color) { 100, 0, 0, 255 });

    // Teeth
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

    // Blood drips
    for (int i = 0; i < 5; i++) {
        float dripX = faceX - 100 * faceScale + i * 50 * faceScale;
        float dripY = faceY - 150 * faceScale;
        float dripLength = 30 * faceScale + sinf(game->jumpscareTimer * 5 + i) * 10;
        DrawRectangle(dripX, dripY, 10 * faceScale, dripLength, (Color) { 150, 0, 0, 255 });
        DrawCircle(dripX + 5 * faceScale, dripY + dripLength, 5 * faceScale, (Color) { 150, 0, 0, 255 });
    }

    // Screen shake effect
    if (game->jumpscareTimer > 0.5f) {
        float shakeAmount = (game->jumpscareTimer - 0.5f) * 20;
        float shakeX = sinf(game->jumpscareTimer * 50) * shakeAmount;
        float shakeY = cosf(game->jumpscareTimer * 50) * shakeAmount;

        // Draw some random static/noise
        for (int i = 0; i < 50; i++) {
            float x = (float)(rand() % (int)screenW) + shakeX;
            float y = (float)(rand() % (int)screenH) + shakeY;
            DrawPixel(x, y, WHITE);
        }
    }

    // "YOU DIED" text
    if (game->jumpscareTimer > 1.0f) {
        const char* diedText = "YOU DIED";
        int textWidth = MeasureText(diedText, 60);
        DrawText(diedText,
            screenW / 2 - textWidth / 2,
            screenH / 2 + 200,
            60, (Color) { 200, 0, 0, 255 });

        if (game->deathTimer > 2.0f) {
            const char* restartText = "Press R to restart";
            int restartWidth = MeasureText(restartText, 30);
            DrawText(restartText,
                screenW / 2 - restartWidth / 2,
                screenH / 2 + 280,
                30, WHITE);
        }
    }
}

void DrawGame(Game* game) {
    if (game->gameOver) {
        // Draw jumpscare instead of normal game
        DrawJumpscare(game);
        return;
    }

    BeginMode2D(game->camera);

    // Draw level
    DrawLevel(&game->level);

    // Draw player
    DrawPlayer(&game->player);

    EndMode2D();

    // Draw UI
    DrawText("Use ARROW KEYS or A/D to move", 10, 10, 20, DARKGRAY);
    DrawText("SPACE to jump", 10, 35, 20, DARKGRAY);

    // Draw score
    char scoreText[50];
    DrawText(scoreText, 10, 60, 25, GOLD);

    if (game->gameWon) {
        const char* winText = "YOU WIN! Press R to restart";
        int textWidth = MeasureText(winText, 40);
        DrawText(winText,
            GetScreenWidth() / 2 - textWidth / 2,
            GetScreenHeight() / 2 - 20,
            40, GOLD);

        char finalScore[50];
        int finalWidth = MeasureText(finalScore, 30);
        DrawText(finalScore,
            GetScreenWidth() / 2 - finalWidth / 2,
            GetScreenHeight() / 2 + 40,
            30, WHITE);
    }
}

void CleanupGame(Game* game) {
    // Currently nothing dynamically allocated to free.
    // This function exists as a placeholder for future resources
    // (textures, sounds, fonts, etc.) that may need unloading.
    (void)game; // Suppress unused parameter warning
}