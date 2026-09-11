#include "level.h"
#include <math.h>

void InitLevel(Level* level) {
    level->platformCount = 0;
    level->backgroundColor = SKYBLUE;

    // Platforms arranged left to right by x-coordinate
    // When two platforms share the same x, the top one comes first

    // Ground segments (left to right)
    // Main ground - first section
    level->platforms[level->platformCount++] = (Rectangle){ 0, 550, 400, 50 };
    // Ground gap (pit) between 400 and 550
    // Ground - second section
    level->platforms[level->platformCount++] = (Rectangle){ 550, 550, 300, 50 };
    // Ground gap (pit) between 850 and 1000
    // Ground - third section
    level->platforms[level->platformCount++] = (Rectangle){ 1000, 550, 350, 50 };
    // Ground gap (pit) between 1350 and 1500
    // Ground - fourth section
    level->platforms[level->platformCount++] = (Rectangle){ 1500, 550, 500, 50 };

    // Left section platforms (x = 100 to 400)
    // Wall at x=300 (tall, appears after ground)
    level->platforms[level->platformCount++] = (Rectangle){ 300, 400, 20, 150 };
    // Platform above wall area
    level->platforms[level->platformCount++] = (Rectangle){ 150, 420, 100, 20 };
    // Higher platform
    level->platforms[level->platformCount++] = (Rectangle){ 320, 320, 100, 20 };

    // Middle-left section (x = 450 to 800)
    // Platform before the pit
    level->platforms[level->platformCount++] = (Rectangle){ 450, 430, 100, 20 };
    // Platform over the pit
    level->platforms[level->platformCount++] = (Rectangle){ 580, 400, 100, 20 };
    // Platform stepping up
    level->platforms[level->platformCount++] = (Rectangle){ 700, 320, 100, 20 };
    // Wall at x=750
    level->platforms[level->platformCount++] = (Rectangle){ 750, 200, 20, 150 };
    // High platform above wall
    level->platforms[level->platformCount++] = (Rectangle){ 770, 180, 100, 20 };

    // Middle section (x = 850 to 1200)
    // Platform over second pit
    level->platforms[level->platformCount++] = (Rectangle){ 880, 430, 120, 20 };
    // Stepping platform
    level->platforms[level->platformCount++] = (Rectangle){ 1020, 350, 100, 20 };
    // Wall at x=1150
    level->platforms[level->platformCount++] = (Rectangle){ 1150, 300, 20, 200 };
    // High platform near wall
    level->platforms[level->platformCount++] = (Rectangle){ 1050, 200, 100, 20 };

    // Middle-right section (x = 1250 to 1550)
    // Platform after second pit
    level->platforms[level->platformCount++] = (Rectangle){ 1250, 420, 100, 20 };
    // Stepping up platform
    level->platforms[level->platformCount++] = (Rectangle){ 1380, 340, 100, 20 };
    // Platform over third pit
    level->platforms[level->platformCount++] = (Rectangle){ 1520, 400, 100, 20 };

    // Right section (x = 1600 to 2000)
    // Platform leading to goal
    level->platforms[level->platformCount++] = (Rectangle){ 1650, 450, 120, 20 };
    // High platform (bonus)
    level->platforms[level->platformCount++] = (Rectangle){ 1750, 300, 100, 20 };
    // Final platform before goal
    level->platforms[level->platformCount++] = (Rectangle){ 1850, 480, 150, 20 };

    // Goal (flag)
    level->goal = (Rectangle){ 1920, 430, 20, 50 };

    // Initialize coins - placed along the path
    level->coinCount = 0;
    // Left section coins
    level->coins[level->coinCount++] = (Coin){ { 200, 380 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 370, 280 }, false, 0 };
    // Middle-left coins
    level->coins[level->coinCount++] = (Coin){ { 500, 390 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 630, 360 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 750, 280 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 820, 140 }, false, 0 };
    // Middle coins
    level->coins[level->coinCount++] = (Coin){ { 940, 390 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1070, 310 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1100, 160 }, false, 0 };
    // Middle-right coins
    level->coins[level->coinCount++] = (Coin){ { 1300, 380 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1430, 300 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1570, 360 }, false, 0 };
    // Right section coins
    level->coins[level->coinCount++] = (Coin){ { 1710, 410 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1800, 260 }, false, 0 };
    level->coins[level->coinCount++] = (Coin){ { 1900, 440 }, false, 0 };

    // Initialize enemies - placed to create challenge
    level->enemyCount = 0;

    // Ground enemy in first section
    level->enemies[level->enemyCount] = (Enemy){
        { 200, 520 }, { 0, 0 }, { 200, 520, 30, 30 },
        150, 350, 100.0f, true, PURPLE
    };
    level->enemyCount++;

    // Enemy on wall-top platform
    level->enemies[level->enemyCount] = (Enemy){
        { 780, 150 }, { 0, 0 }, { 780, 150, 30, 30 },
        770, 840, 60.0f, true, PURPLE
    };
    level->enemyCount++;

    // Ground enemy in second section
    level->enemies[level->enemyCount] = (Enemy){
        { 650, 520 }, { 0, 0 }, { 650, 520, 30, 30 },
        550, 850, 120.0f, true, PURPLE
    };
    level->enemyCount++;

    // Enemy on middle platform
    level->enemies[level->enemyCount] = (Enemy){
        { 1260, 390 }, { 0, 0 }, { 1260, 390, 30, 30 },
        1250, 1320, 70.0f, true, PURPLE
    };
    level->enemyCount++;

    // Ground enemy in fourth section
    level->enemies[level->enemyCount] = (Enemy){
        { 1600, 520 }, { 0, 0 }, { 1600, 520, 30, 30 },
        1500, 1850, 110.0f, true, PURPLE
    };
    level->enemyCount++;

    // Enemy on high platform
    level->enemies[level->enemyCount] = (Enemy){
        { 1760, 270 }, { 0, 0 }, { 1760, 270, 30, 30 },
        1750, 1820, 60.0f, false, PURPLE
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

    DrawCircle(1800, 100, 25, WHITE);
    DrawCircle(1825, 100, 35, WHITE);
    DrawCircle(1850, 100, 25, WHITE);
}