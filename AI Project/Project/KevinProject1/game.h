#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "level.h"

#define MAX_PARTICLES 128
#define MAX_POWERUPS  1
#define MAX_DEBRIS    24

typedef enum Scene {
    SCENE_MENU = 0,
    SCENE_PLAY,
    SCENE_END
} Scene;

typedef struct Particle {
    Vector2 position;
    Vector2 velocity;
    float   life;
    float   maxLife;
    float   size;
    Color   color;
    bool    active;
} Particle;

typedef struct Debris {
    Vector2 position;
    Vector2 velocity;
    float   life;
    float   maxLife;
    float   rotation;
    float   spin;
    float   size;
    Color   color;
    bool    active;
} Debris;

typedef enum PowerupKind {
    POWERUP_SPEED = 0,
    POWERUP_JUMP,
    POWERUP_SHIELD,
    POWERUP_KIND_COUNT
} PowerupKind;

typedef struct Powerup {
    Vector2 position;
    PowerupKind kind;
    bool    active;
    float   respawnTimer;
    float   bobPhase;
} Powerup;

typedef struct Game {
    Player player;
    Player player2;
    Level level;
    Camera2D camera;

    Scene scene;
    int   menuSelection;

    int itIndex;
    float tagCooldown;
    float tagFlashTimer;

    float cameraShake;
    float cameraShakeTimer;

    bool gameWon;
    bool gameOver;
    bool gameTimedOut;
    float gameTimer;
    int score;
    float jumpscareTimer;
    float deathTimer;
    Vector2 deathPosition;

    Vector2 explosionPos;
    float   explosionTimer;
    bool    exploding;

    // Explosion extras
    float   explosionShakeTimer;
    float   explosionZoomKick;
    float   timeScale;
    bool    chromaticFlash;
    Vector2 scorchPos;
    float   scorchTimer;
    bool    scorchActive;
    Debris  debris[MAX_DEBRIS];

    Particle particles[MAX_PARTICLES];
    Powerup  powerups[MAX_POWERUPS];
} Game;

void InitGame(Game* game);
void UpdateGame(Game* game, float deltaTime);
void DrawGame(Game* game);
void CleanupGame(Game* game);

#endif