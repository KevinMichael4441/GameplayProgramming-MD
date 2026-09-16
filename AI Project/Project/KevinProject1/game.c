#include "game.h"
#include <stdio.h>
#include <math.h>

void InitGame(Game* game) {
    InitLevel(&game->level);

    InitPlayer(&game->player,
        game->level.playerStart.x,
        game->level.playerStart.y,
        INPUT_ARROWS, RED);

    InitPlayer(&game->player2,
        game->level.playerStart.x + 50,
        game->level.playerStart.y,
        INPUT_WASD, BLUE);

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

// Discrete zoom steps. All of these scale cleanly (no fractional pixel drift).
static const float ZOOM_STEPS[] = { 0.5f, 0.75f, 1.0f, 1.5f, 2.0f };
#define ZOOM_STEP_COUNT (sizeof(ZOOM_STEPS) / sizeof(ZOOM_STEPS[0]))

// Snap a raw zoom value to the nearest discrete step.
static float SnapZoom(float raw) {
    float best = ZOOM_STEPS[0];
    float bestDist = fabsf(raw - ZOOM_STEPS[0]);
    for (int i = 1; i < (int)ZOOM_STEP_COUNT; i++) {
        float d = fabsf(raw - ZOOM_STEPS[i]);
        if (d < bestDist) {
            bestDist = d;
            best = ZOOM_STEPS[i];
        }
    }
    return best;
}

void UpdateGame(Game* game, float deltaTime) {
    if (!game->gameWon && !game->gameOver) {
        UpdateLevelBackground(&game->level, deltaTime);

        UpdatePlayer(&game->player, &game->level, deltaTime);
        UpdatePlayer(&game->player2, &game->level, deltaTime);

        // ---- Camera: zoom to fit both players, snapped to clean steps ----
        const float margin = 120.0f;

        float minX = fminf(game->player.position.x, game->player2.position.x) - margin;
        float maxX = fmaxf(game->player.position.x + game->player.rec.width,
            game->player2.position.x + game->player2.rec.width) + margin;
        float minY = fminf(game->player.position.y, game->player2.position.y) - margin;
        float maxY = fmaxf(game->player.position.y + game->player.rec.height,
            game->player2.position.y + game->player2.rec.height) + margin;

        float boxW = maxX - minX;
        float boxH = maxY - minY;

        float zoomX = (float)GetScreenWidth() / boxW;
        float zoomY = (float)GetScreenHeight() / boxH;
        float rawZoom = fminf(zoomX, zoomY);

        // Snap the target to a clean step
        float targetZoom = SnapZoom(rawZoom);

        // Smooth toward the snapped target, then jump exactly onto it.
        float diff = targetZoom - game->camera.zoom;
        if (fabsf(diff) < 0.01f) {
            game->camera.zoom = targetZoom;
        }
        else {
            game->camera.zoom += diff * fminf(1.0f, deltaTime * 8.0f);
        }

        // Center camera on the bounding box midpoint
        float camX = (minX + maxX) * 0.5f;
        float camY = (minY + maxY) * 0.5f;

        // Clamp camera so we don't show empty space outside the world
        float halfW = GetScreenWidth() / (2.0f * game->camera.zoom);
        float halfH = GetScreenHeight() / (2.0f * game->camera.zoom);

        if (game->level.worldWidth > halfW * 2.0f) {
            if (camX < halfW) camX = halfW;
            if (camX > game->level.worldWidth - halfW) camX = game->level.worldWidth - halfW;
        }
        else {
            camX = game->level.worldWidth * 0.5f;
        }
        if (game->level.worldHeight > halfH * 2.0f) {
            if (camY < halfH) camY = halfH;
            if (camY > game->level.worldHeight - halfH) camY = game->level.worldHeight - halfH;
        }
        else {
            camY = game->level.worldHeight * 0.5f;
        }

        game->camera.target = (Vector2){ camX, camY };
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

    if ((int)(game->jumpscareTimer * 10) % 2 == 0)
        DrawRectangle(0, 0, screenW, screenH, (Color) { 50, 0, 0, 200 });
    else
        DrawRectangle(0, 0, screenW, screenH, (Color) { 0, 0, 0, 200 });

    float faceX = screenW / 2;
    float faceY = screenH / 2;
    float faceScale = 1.0f + game->jumpscareTimer * 2.0f;

    DrawCircle(faceX, faceY, 150 * faceScale, (Color) { 200, 180, 180, 255 });

    float eyeOffset = 60 * faceScale;
    float eyeSize = 40 * faceScale;

    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize, WHITE);
    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize * 0.6f, (Color) { 200, 0, 0, 255 });
    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize * 0.3f, BLACK);

    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize, WHITE);
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

    if (game->jumpscareTimer > 1.0f) {
        const char* diedText = "YOU DIED";
        int textWidth = MeasureText(diedText, 60);
        DrawText(diedText, screenW / 2 - textWidth / 2, screenH / 2 - 250, 60,
            (Color) {
            200, 0, 0, 255
        });
    }

    if (game->jumpscareTimer > 1.33f) {
        const char* restartText = "Press R to restart";
        int restartWidth = MeasureText(restartText, 30);
        DrawText(restartText, screenW / 2 - restartWidth / 2, screenH / 2 - 170, 30, ORANGE);
    }
}

void DrawGame(Game* game) {
    if (game->gameOver) {
        DrawJumpscare(game);
        return;
    }

    BeginMode2D(game->camera);
    DrawLevelBackground(&game->level, &game->camera);
    DrawLevel(&game->level);
    DrawPlayer(&game->player);
    DrawPlayer(&game->player2);
    EndMode2D();

    DrawText("P1: ARROWS + UP to jump", 10, 10, 18, RED);
    DrawText("P2: WASD + W to jump", 10, 32, 18, BLUE);
}

void CleanupGame(Game* game) {
    (void)game;
}