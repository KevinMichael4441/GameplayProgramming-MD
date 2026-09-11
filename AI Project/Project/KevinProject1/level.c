#include "level.h"
#include <math.h>

void InitLevel(Level* level) {
    level->platformCount = 0;
    level->backgroundColor = SKYBLUE;

    // Ground
    level->platforms[level->platformCount++] = (Rectangle){ 0, 550, 2000, 50 };

    // Platforms
    level->platforms[level->platformCount++] = (Rectangle){ 200, 450, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 350, 350, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 500, 250, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 650, 350, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 800, 450, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 950, 350, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1100, 250, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1250, 300, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1400, 400, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1550, 300, 100, 20 };
    level->platforms[level->platformCount++] = (Rectangle){ 1700, 200, 100, 20 };

    // Walls
    level->platforms[level->platformCount++] = (Rectangle){ 300, 400, 20, 150 };
    level->platforms[level->platformCount++] = (Rectangle){ 750, 300, 20, 150 };
    level->platforms[level->platformCount++] = (Rectangle){ 1200, 350, 20, 200 };

    // Goal platform
    level->platforms[level->platformCount++] = (Rectangle){ 1800, 550, 200, 50 };

    // Goal (flag)
    level->goal = (Rectangle){ 1900, 500, 20, 50 };

    // Initialize coins
    level->coinCount = 0;
    level->coins[level->coinCount++] = (Coin){ { 230, 400 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 380, 300 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 530, 200 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 680, 300 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 830, 400 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 980, 300 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1130, 200 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1280, 250 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1430, 350 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1580, 250 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1730, 150 }, false, 0 };

    // Initialize enemies
    level->enemyCount = 0;

    // Ground enemy
    level->enemies[level->enemyCount] = (Enemy){
        { 400, 520 }, { 0, 0 }, { 400, 520, 30, 30 },
        350, 550, 100.0f, true, PURPLE
    };
    level->enemyCount++;

    // Platform enemy
    level->enemies[level->enemyCount] = (Enemy){
        { 820, 420 }, { 0, 0 }, { 820, 420, 30, 30 },
        800, 870, 80.0f, true, PURPLE
    };
    level->enemyCount++;

    // Ground enemy 2
    level->enemies[level->enemyCount] = (Enemy){
        { 1000, 520 }, { 0, 0 }, { 1000, 520, 30, 30 },
        950, 1150, 120.0f, true, PURPLE
    };
    level->enemyCount++;

    // Platform enemy 2
    level->enemies[level->enemyCount] = (Enemy){
        { 1420, 370 }, { 0, 0 }, { 1420, 370, 30, 30 },
        1400, 1470, 70.0f, true, PURPLE
    };
    level->enemyCount++;

    // Goal area enemy
    level->enemies[level->enemyCount] = (Enemy){
        { 1750, 520 }, { 0, 0 }, { 1750, 520, 30, 30 },
        1700, 1850, 90.0f, false, PURPLE
    };
    level->enemyCount++;
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
            // Inner detail
            DrawCircle(level->coins[i].position.x,
                level->coins[i].position.y + bounce, 5, YELLOW);
        }
    }

    // Draw enemies
    for (int i = 0; i < level->enemyCount; i++) {
        Enemy* e = &level->enemies[i];

        // Draw enemy body
        DrawRectangleRec(e->rec, e->color);

        // Draw enemy eyes (angry look)
        DrawCircle(e->position.x + 8, e->position.y + 10, 4, WHITE);
        DrawCircle(e->position.x + 22, e->position.y + 10, 4, WHITE);
        DrawCircle(e->position.x + 9, e->position.y + 10, 2, BLACK);
        DrawCircle(e->position.x + 23, e->position.y + 10, 2, BLACK);

        // Angry eyebrows
        DrawLine(e->position.x + 4, e->position.y + 5,
            e->position.x + 12, e->position.y + 8, BLACK);
        DrawLine(e->position.x + 18, e->position.y + 8,
            e->position.x + 26, e->position.y + 5, BLACK);

        // Angry mouth
        DrawLine(e->position.x + 8, e->position.y + 22,
            e->position.x + 22, e->position.y + 18, BLACK);

        // Spikes on top
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

    // Draw some clouds for decoration
    DrawCircle(150, 100, 30, WHITE);
    DrawCircle(180, 100, 40, WHITE);
    DrawCircle(210, 100, 30, WHITE);

    DrawCircle(600, 80, 25, WHITE);
    DrawCircle(625, 80, 35, WHITE);
    DrawCircle(650, 80, 25, WHITE);

    DrawCircle(1200, 120, 30, WHITE);
    DrawCircle(1230, 120, 40, WHITE);
    DrawCircle(1260, 120, 30, WHITE);
}