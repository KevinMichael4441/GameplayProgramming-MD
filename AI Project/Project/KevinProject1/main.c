#include "raylib.h"

// ---------------------------------------------------------
// Constants
// ---------------------------------------------------------

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720
#define TARGET_FPS    60

// ---------------------------------------------------------
// Game state
// ---------------------------------------------------------

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float speed;
} Player;

// ---------------------------------------------------------
// Initialization
// ---------------------------------------------------------

static Player Player_Create(void)
{
    Player player = {
        .position = {
            SCREEN_WIDTH / 2.0f,
            SCREEN_HEIGHT / 2.0f
        },
        .velocity = { 0 },
        .speed = 300.0f
    };

    return player;
}

// ---------------------------------------------------------
// Update
// ---------------------------------------------------------

static void Player_Update(Player* player)
{
    player->velocity = (Vector2){ 0 };

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        player->velocity.y -= 1.0f;

    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        player->velocity.y += 1.0f;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        player->velocity.x -= 1.0f;

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        player->velocity.x += 1.0f;

    player->position.x += player->velocity.x * player->speed * GetFrameTime();
    player->position.y += player->velocity.y * player->speed * GetFrameTime();

    // Keep player on screen
    player->position.x = Clamp(
        player->position.x,
        20.0f,
        SCREEN_WIDTH - 20.0f
    );

    player->position.y = Clamp(
        player->position.y,
        20.0f,
        SCREEN_HEIGHT - 20.0f
    );
}

// ---------------------------------------------------------
// Drawing
// ---------------------------------------------------------

static void Player_Draw(const Player* player)
{
    DrawCircleV(player->position, 20.0f, BLUE);
}

static void DrawGame(const Player* player)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    Player_Draw(player);

    DrawText(
        "WASD / Arrow Keys to move",
        20,
        20,
        20,
        DARKGRAY
    );

    DrawFPS(SCREEN_WIDTH - 100, 20);

    EndDrawing();
}

// ---------------------------------------------------------
// Main
// ---------------------------------------------------------

int main(void)
{
    InitWindow(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "My Raylib Game"
    );

    SetTargetFPS(TARGET_FPS);

    Player player = Player_Create();

    while (!WindowShouldClose())
    {
        // -----------------------------
        // Update
        // -----------------------------
        Player_Update(&player);

        // -----------------------------
        // Draw
        // -----------------------------
        DrawGame(&player);
    }

    CloseWindow();

    return 0;
}