#include "level.h"

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