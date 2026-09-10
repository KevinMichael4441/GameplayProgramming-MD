#include "raylib.h"
#include "game.h"

int main(void) {
    // Initialize window
    InitWindow(800, 600, "Simple Platformer");
    SetTargetFPS(60);

    // Initialize game
    Game game;
    InitGame(&game);

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Update
        UpdateGame(&game, deltaTime);

        // Draw
        BeginDrawing();
        ClearBackground(SKYBLUE);

        DrawGame(&game);

        EndDrawing();
    }

    // Cleanup
    CleanupGame(&game);
    CloseWindow();

    return 0;
}