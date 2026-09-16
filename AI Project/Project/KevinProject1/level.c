#include "level.h"
#include <math.h>

void InitLevel(Level* level) {
    level->platformCount = 0;
    level->backgroundColor = SKYBLUE;

    // Platforms arranged left to right by x-coordinate
    // When two platforms share the same x, the top one comes first
    // NOTE: Total platforms = 22, MAX_PLATFORMS must be >= 22

    // Ground segments (left to right)
    level->platforms[level->platformCount++] = (Rectangle){ 0, 550, 400, 50 };
    level->platforms[level->platformCount++] = (Rectangle){ 550, 550, 300, 50 };
    level->platforms[level->platformCount++] = (Rectangle){ 1000, 550, 350, 50 };
    level->platforms[level->platformCount++] = (Rectangle){ 1500, 550, 500, 50 };

    // Left section platforms
    level->platforms[level->platformCount++] = (Rectangle){ 150, 420, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 300, 400, 20, 150 };
    level->platforms[level->platformCount++] = (Rectangle){ 320, 320, 100, 20 };

    // Middle-left section
    level->platforms[level->platformCount++] = (Rectangle){ 450, 430, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 580, 400, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 700, 320, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 750, 200, 20, 150 };
    level->platforms[level->platformCount++] = (Rectangle){ 770, 180, 100, 20 };

    // Middle section
    level->platforms[level->platformCount++] = (Rectangle){ 880, 430, 120, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1020, 350, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1050, 200, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1150, 300, 20, 200 };

    // Middle-right section
    level->platforms[level->platformCount++] = (Rectangle){ 1250, 420, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1380, 340, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1520, 400, 100, 20 };

    // Right section
    level->platforms[level->platformCount++] = (Rectangle){ 1650, 450, 120, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1750, 300, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1850, 480, 150, 20 };

    // Goal (flag)
    level->goal = (Rectangle){ 1920, 430, 20, 50 };

    // Initialize coins
    level->coinCount = 0;
    level->coins[level->coinCount++] = (Coin){ { 200, 380 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 370, 280 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 500, 390 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 630, 360 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 750, 280 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 820, 140 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 940, 390 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1070, 310 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1100, 160 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1300, 380 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1430, 300 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1570, 360 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1710, 410 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1800, 260 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1900, 440 }, false, 0 };

    // Initialize enemies
    // direction: -1 = left, +1 = right
    // directionTimer: seconds until next chance to change direction
    // gravity: pulls them down onto platforms
    level->enemyCount = 0;

    // Ground enemy in first section
    level->enemies[level->enemyCount++] = (Enemy){
        .position = { 200, 520 },
        .velocity = { 0, 0 },
        .rec = { 200, 520, 30, 30 },
        .speed = 0.0f,
        .moveSpeed = 100.0f,
        .direction = 1,
        .directionTimer = 1.0f,
        .gravity = 980.0f,
        .onGround = false,
        .color = PURPLE
    };

    // Enemy on wall-top platform
    level->enemies[level->enemyCount++] = (Enemy){
        .position = { 780, 150 },
        .velocity = { 0, 0 },
        .rec = { 780, 150, 30, 30 },
        .speed = 0.0f,
        .moveSpeed = 60.0f,
        .direction = 1,
        .directionTimer = 1.0f,
        .gravity = 980.0f,
        .onGround = false,
        .color = PURPLE
    };

    // Ground enemy in second section
    level->enemies[level->enemyCount++] = (Enemy){
        .position = { 650, 520 },
        .velocity = { 0, 0 },
        .rec = { 650, 520, 30, 30 },
        .speed = 0.0f,
        .moveSpeed = 120.0f,
        .direction = -1,
        .directionTimer = 1.0f,
        .gravity = 980.0f,
        .onGround = false,
        .color = PURPLE
    };

    // Enemy on middle platform
    level->enemies[level->enemyCount++] = (Enemy){
        .position = { 1260, 390 },
        .velocity = { 0, 0 },
        .rec = { 1260, 390, 30, 30 },
        .speed = 0.0f,
        .moveSpeed = 70.0f,
        .direction = 1,
        .directionTimer = 1.0f,
        .gravity = 980.0f,
        .onGround = false,
        .color = PURPLE
    };

    // Ground enemy in fourth section
    level->enemies[level->enemyCount++] = (Enemy){
        .position = { 1600, 520 },
        .velocity = { 0, 0 },
        .rec = { 1600, 520, 30, 30 },
        .speed = 0.0f,
        .moveSpeed = 110.0f,
        .direction = 1,
        .directionTimer = 1.0f,
        .gravity = 980.0f,
        .onGround = false,
        .color = PURPLE
    };

    // Enemy on high platform
    level->enemies[level->enemyCount++] = (Enemy){
        .position = { 1760, 270 },
        .velocity = { 0, 0 },
        .rec = { 1760, 270, 30, 30 },
        .speed = 0.0f,
        .moveSpeed = 60.0f,
        .direction = -1,
        .directionTimer = 1.0f,
        .gravity = 980.0f,
        .onGround = false,
        .color = PURPLE
    };
}

void DrawLevel(Level* level) {
    // Draw all platforms
    for (int i = 0; i < level->platformCount; i++) {
        DrawRectangleRec(level->platforms[i], DARKGREEN);
        DrawRectangleLinesEx(level->platforms[i], 2, DARKGRAY);
    }

    // Draw goal (flag)
    DrawRectangleRec(level->goal, GOLD);
    DrawRectangleLinesEx(level->goal, 2, ORANGE);

    // Draw flag
    DrawTriangle(
        (Vector2) {
        level->goal.x + 20, level->goal.y
    },
        (Vector2) {
        level->goal.x + 20, level->goal.y + 30
    },
        (Vector2) {
        level->goal.x + 50, level->goal.y + 15
    },
        RED
    );

    // Draw coins
    for (int i = 0; i < level->coinCount; i++) {
        if (!level->coins[i].collected) {
            float bounce = sinf(level->coins[i].rotation) * 3.0f;
            DrawCircle(level->coins[i].position.x,
                level->coins[i].position.y + bounce, 10, GOLD);
            DrawCircleLines(level->coins[i].position.x,
                level->coins[i].position.y + bounce, 10, ORANGE);
            DrawCircle(level->coins[i].position.x,
                level->coins[i].position.y + bounce, 5, YELLOW);
        }
    }

    // Draw enemies
    for (int i = 0; i < level->enemyCount; i++) {
        Enemy* e = &level->enemies[i];

        DrawRectangleRec(e->rec, e->color);

        DrawCircle(e->position.x + 8, e->position.y + 10, 4, WHITE);
        DrawCircle(e->position.x + 22, e->position.y + 10, 4, WHITE);
        DrawCircle(e->position.x + 9, e->position.y + 10, 2, BLACK);
        DrawCircle(e->position.x + 23, e->position.y + 10, 2, BLACK);

        DrawLine(e->position.x + 4, e->position.y + 5,
            e->position.x + 12, e->position.y + 8, BLACK);
        DrawLine(e->position.x + 18, e->position.y + 8,
            e->position.x + 26, e->position.y + 5, BLACK);

        DrawLine(e->position.x + 8, e->position.y + 22,
            e->position.x + 22, e->position.y + 18, BLACK);

        DrawTriangle(
            (Vector2) {
            e->position.x + 5, e->position.y
        },
            (Vector2) {
            e->position.x + 10, e->position.y - 8
        },
            (Vector2) {
            e->position.x + 15, e->position.y
        },
            DARKPURPLE
        );
        DrawTriangle(
            (Vector2) {
            e->position.x + 15, e->position.y
        },
            (Vector2) {
            e->position.x + 20, e->position.y - 8
        },
            (Vector2) {
            e->position.x + 25, e->position.y
        },
            DARKPURPLE
        );
    }

    // Clouds
    DrawCircle(150, 100, 30, WHITE);
    DrawCircle(180, 100, 40, WHITE);
    DrawCircle(210, 100, 30, WHITE);

    DrawCircle(600, 80, 25, WHITE);
    DrawCircle(625, 80, 35, WHITE);
    DrawCircle(650, 80, 25, WHITE);

    DrawCircle(1200, 120, 30, WHITE);
    DrawCircle(1230, 120, 40, WHITE);
    DrawCircle(1260, 120, 30, WHITE);

    DrawCircle(1800, 100, 25, WHITE);
    DrawCircle(1825, 100, 35, WHITE);
    DrawCircle(1850, 100, 25, WHITE);
}