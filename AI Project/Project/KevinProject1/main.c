#include "raylib.h"
#include "game.h"

int main(void) {
    InitWindow(800, 600, "Simple Platformer");
    SetTargetFPS(60);

    Game game;
    InitGame(&game);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        UpdateGame(&game, deltaTime);

        BeginDrawing();
        ClearBackground(SKYBLUE);
        DrawGame(&game);
        EndDrawing();
    }

    CleanupGame(&game);
    CloseWindow();
    return 0;
}