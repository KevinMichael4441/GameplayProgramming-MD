#include "raylib.h"
#include "game.h"

int main(void)
{
    InitWindow(1280, 720, "Test Game");
    SetTargetFPS(60);

    Game game = Game_Create();

    while (!WindowShouldClose())
    {
        Game_Update(&game);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        Game_Draw(&game);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
