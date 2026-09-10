#include "game.h"
#include <stdio.h>

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
}

void UpdateGame(Game* game, float deltaTime) {
    if (!game->gameWon) {
        // Update player
        UpdatePlayer(&game->player, &game->level, deltaTime);

        // Update camera to follow player
        game->camera.target = (Vector2){
            game->player.position.x,
            game->player.position.y - 50
        };

        // Check win condition (reached the goal)
        if (CheckCollisionRecs(game->player.rec, game->level.goal)) {
            game->gameWon = true;
        }

        // Check if player fell off the world
        if (game->player.position.y > 800) {
            // Reset player
            game->player.position = (Vector2){ 100, 400 };
            game->player.velocity = (Vector2){ 0, 0 };
        }
    }
    else {
        // Allow restart with R key
        if (IsKeyPressed(KEY_R)) {
            InitGame(game);
        }
    }
}

void DrawGame(Game* game) {
    BeginMode2D(game->camera);

    // Draw level
    DrawLevel(&game->level);

    // Draw player
    DrawPlayer(&game->player);

    EndMode2D();

    // Draw UI
    DrawText("Use ARROW KEYS or A/D to move", 10, 10, 20, DARKGRAY);
    DrawText("SPACE to jump", 10, 35, 20, DARKGRAY);

    if (game->gameWon) {
        const char* winText = "YOU WIN! Press R to restart";
        int textWidth = MeasureText(winText, 40);
        DrawText(winText,
            GetScreenWidth() / 2 - textWidth / 2,
            GetScreenHeight() / 2 - 20,
            40, GOLD);
    }
}

void CleanupGame(Game* game) {
    // Currently nothing dynamically allocated to free.
    // This function exists as a placeholder for future resources
    // (textures, sounds, fonts, etc.) that may need unloading.
    (void)game; // Suppress unused parameter warning
}