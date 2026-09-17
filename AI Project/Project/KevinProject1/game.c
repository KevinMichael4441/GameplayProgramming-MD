#include "game.h"
#include <stdio.h>
#include <math.h>

#define TAG_COOLDOWN 1.5f

// ---------------------------------------------------------------------------
// Particles
// ---------------------------------------------------------------------------

static float Rand01G(unsigned int seed) {
    seed = ((seed >> 16) ^ seed) * 0x45d9f3b;
    seed = ((seed >> 16) ^ seed) * 0x45d9f3b;
    seed = (seed >> 16) ^ seed;
    return (float)(seed & 0xFFFF) / 65535.0f;
}

static int AllocParticle(Game* game) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!game->particles[i].active) return i;
    }
    return -1;
}

static void SpawnDust(Game* game, Vector2 pos, int count,
    float spreadX, float baseY, Color tint)
{
    unsigned int now = (unsigned int)(GetTime() * 1000.0f);
    for (int i = 0; i < count; i++) {
        int idx = AllocParticle(game);
        if (idx < 0) return;

        Particle* p = &game->particles[idx];
        p->active = true;

        float ox = (Rand01G(now + i * 17) - 0.5f) * spreadX;
        float oy = (Rand01G(now + i * 29) - 0.5f) * 6.0f;

        p->position = (Vector2){ pos.x + ox, pos.y + oy };

        float vx = (Rand01G(now + i * 41) - 0.5f) * spreadX * 4.0f;
        float vy = baseY + Rand01G(now + i * 53) * 40.0f;

        p->velocity = (Vector2){ vx, vy };

        p->maxLife = 0.35f + Rand01G(now + i * 67) * 0.35f;
        p->life = p->maxLife;
        p->size = 2.5f + Rand01G(now + i * 71) * 2.5f;
        p->color = tint;
    }
}

static void UpdateParticles(Game* game, float deltaTime) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &game->particles[i];
        if (!p->active) continue;

        p->life -= deltaTime;
        if (p->life <= 0.0f) { p->active = false; continue; }

        p->position.x += p->velocity.x * deltaTime;
        p->position.y += p->velocity.y * deltaTime;

        p->velocity.x *= 1.0f - 3.0f * deltaTime;
        p->velocity.y += 120.0f * deltaTime;
    }
}

static void DrawParticles(Game* game) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &game->particles[i];
        if (!p->active) continue;

        float t = p->life / p->maxLife;
        unsigned char a = (unsigned char)(255 * t * t);
        float r = p->size * (1.0f + (1.0f - t) * 0.8f);

        Color c = p->color;
        c.a = a;
        DrawCircleV(p->position, r, c);
    }
}

static void ClearParticles(Game* game) {
    for (int i = 0; i < MAX_PARTICLES; i++) game->particles[i].active = false;
}

// ---------------------------------------------------------------------------
// Power-ups
// ---------------------------------------------------------------------------

static Color PowerupColor(PowerupKind kind) {
    switch (kind) {
    case POWERUP_SPEED:  return (Color) { 255, 220, 60, 255 };
    case POWERUP_JUMP:   return (Color) { 100, 220, 255, 255 };
    case POWERUP_SHIELD: return (Color) { 120, 255, 140, 255 };
    default:             return WHITE;
    }
}

static void SpawnOnePowerup(Game* game) {
    Vector2 pos = GetRandomSpawnPoint(&game->level, 24.0f, 24.0f);
    pos.x += 12.0f;
    pos.y += 12.0f;

    Powerup* p = &game->powerups[0];
    p->position = pos;
    p->kind = (PowerupKind)GetRandomValue(0, POWERUP_KIND_COUNT - 1);
    p->active = true;
    p->respawnTimer = 0.0f;
    p->bobPhase = (float)GetRandomValue(0, 628) / 100.0f;
}

static void SeedPowerups(Game* game) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        game->powerups[i].active = false;
        game->powerups[i].respawnTimer = 0.0f;
    }
    SpawnOnePowerup(game);
}

static void CollectPowerup(Game* game, Player* player, Powerup* power) {
    Player* other = (player == &game->player) ? &game->player2 : &game->player;

    player->speedBoostTimer = 0.0f;
    player->jumpBoostTimer = 0.0f;
    player->shieldTimer = 0.0f;
    other->speedBoostTimer = 0.0f;
    other->jumpBoostTimer = 0.0f;
    other->shieldTimer = 0.0f;

    switch (power->kind) {
    case POWERUP_SPEED:  player->speedBoostTimer = 5.0f; break;
    case POWERUP_JUMP:   player->jumpBoostTimer = 5.0f; break;
    case POWERUP_SHIELD: player->shieldTimer = 4.0f; break;
    default: break;
    }

    Color c = PowerupColor(power->kind);
    SpawnDust(game, power->position, 16, 40.0f, -60.0f, c);

    power->active = false;
    power->respawnTimer = 6.0f;
}

static void UpdatePowerups(Game* game, float deltaTime) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        Powerup* p = &game->powerups[i];

        if (!p->active) {
            if (p->respawnTimer > 0.0f) {
                p->respawnTimer -= deltaTime;
                if (p->respawnTimer <= 0.0f) {
                    SpawnOnePowerup(game);
                }
            }
            continue;
        }

        p->bobPhase += deltaTime * 3.0f;

        Rectangle r = { p->position.x - 12.0f, p->position.y - 12.0f,
                        24.0f, 24.0f };

        if (CheckCollisionRecs(r, game->player.rec)) {
            CollectPowerup(game, &game->player, p);
        }
        else if (CheckCollisionRecs(r, game->player2.rec)) {
            CollectPowerup(game, &game->player2, p);
        }
    }
}

static void DrawPowerup(Powerup* p) {
    if (!p->active) return;

    Color c = PowerupColor(p->kind);
    float bob = sinf(p->bobPhase) * 3.0f;
    Vector2 pos = { p->position.x, p->position.y + bob };

    DrawCircleV(pos, 18.0f, Fade(c, 0.20f));
    DrawCircleV(pos, 13.0f, Fade(c, 0.35f));
    DrawCircleV(pos, 10.0f, c);
    DrawCircleV(pos, 6.0f, Fade(WHITE, 0.55f));

    switch (p->kind) {
    case POWERUP_SPEED: {
        Vector2 a = { pos.x - 2, pos.y - 6 };
        Vector2 b = { pos.x + 2, pos.y - 1 };
        Vector2 c1 = { pos.x - 1, pos.y - 1 };
        Vector2 d = { pos.x + 2, pos.y + 6 };
        Vector2 e = { pos.x - 2, pos.y + 1 };
        Vector2 f = { pos.x + 1, pos.y + 1 };
        DrawTriangle(a, b, c1, BLACK);
        DrawTriangle(d, e, f, BLACK);
        break;
    }
    case POWERUP_JUMP: {
        Vector2 tip = { pos.x,     pos.y - 6 };
        Vector2 right = { pos.x + 5, pos.y - 1 };
        Vector2 left = { pos.x - 5, pos.y - 1 };
        DrawTriangle(tip, right, left, BLACK);
        DrawRectangle((int)(pos.x - 2), (int)(pos.y - 1), 4, 7, BLACK);
        break;
    }
    case POWERUP_SHIELD: {
        DrawCircleLines((int)pos.x, (int)pos.y, 5, BLACK);
        DrawCircleLines((int)pos.x, (int)pos.y, 4, BLACK);
        break;
    }
    default: break;
    }
}

static void DrawPowerups(Game* game) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        DrawPowerup(&game->powerups[i]);
    }
}

static void DrawPlayerGlow(Player* p) {
    Color glow = WHITE;
    bool  active = false;
    float timer = 0.0f;

    if (p->speedBoostTimer > 0.0f) {
        glow = (Color){ 255, 220,  60, 255 };
        timer = p->speedBoostTimer;
        active = true;
    }
    else if (p->jumpBoostTimer > 0.0f) {
        glow = (Color){ 100, 220, 255, 255 };
        timer = p->jumpBoostTimer;
        active = true;
    }
    else if (p->shieldTimer > 0.0f) {
        glow = (Color){ 120, 255, 140, 255 };
        timer = p->shieldTimer;
        active = true;
    }

    if (!active) return;

    Vector2 c = {
        p->position.x + p->rec.width * 0.5f,
        p->position.y + p->rec.height * 0.5f
    };

    float tail = (timer < 1.2f) ? (timer / 1.2f) : 1.0f;
    float pulse = 1.0f + sinf((float)GetTime() * 6.0f) * 0.06f;
    float baseR = 22.0f * pulse;

    DrawCircleV(c, baseR + 8.0f, Fade(glow, 0.10f * tail));
    DrawCircleV(c, baseR + 4.0f, Fade(glow, 0.18f * tail));
    DrawCircleV(c, baseR, Fade(glow, 0.28f * tail));
}

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------

static void ClearPowerups(Game* game) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        game->powerups[i].active = false;
        game->powerups[i].respawnTimer = 0.0f;
    }
}

static void ClearDebris(Game* game) {
    for (int i = 0; i < MAX_DEBRIS; i++) game->debris[i].active = false;
}

static void StartMatch(Game* game, MapTheme theme) {
    InitLevel(&game->level, theme);

    Vector2 spawn1 = GetRandomSpawnPoint(&game->level, 30.0f, 30.0f);
    Vector2 spawn2 = GetRandomSpawnPoint(&game->level, 30.0f, 30.0f);
    if (fabsf(spawn1.x - spawn2.x) < 40.0f &&
        fabsf(spawn1.y - spawn2.y) < 40.0f) {
        spawn2 = GetRandomSpawnPoint(&game->level, 30.0f, 30.0f);
    }

    InitPlayer(&game->player, spawn1.x, spawn1.y, INPUT_ARROWS, RED);
    InitPlayer(&game->player2, spawn2.x, spawn2.y, INPUT_WASD, BLUE);

    game->camera.target = (Vector2){ game->player.position.x, game->player.position.y };
    game->camera.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;

    game->itIndex = GetRandomValue(0, 1);
    game->tagCooldown = 0.0f;
    game->tagFlashTimer = 0.0f;
    game->cameraShake = 0.0f;
    game->cameraShakeTimer = 0.0f;

    game->gameWon = false;
    game->gameOver = false;
    game->gameTimedOut = false;
    game->gameTimer = TIMER_SECONDS;
    game->score = 0;
    game->jumpscareTimer = 0.0f;
    game->deathTimer = 0.0f;
    game->deathPosition = (Vector2){ 0, 0 };

    game->exploding = false;
    game->explosionTimer = 0.0f;
    game->explosionPos = (Vector2){ 0, 0 };
    game->explosionShakeTimer = 0.0f;
    game->explosionZoomKick = 0.0f;
    game->timeScale = 1.0f;
    game->chromaticFlash = false;
    game->scorchTimer = 0.0f;
    game->scorchActive = false;
    game->scorchPos = (Vector2){ 0, 0 };
    ClearDebris(game);

    ClearParticles(game);
    SeedPowerups(game);

    game->scene = SCENE_PLAY;
}

void InitGame(Game* game) {
    game->scene = SCENE_MENU;
    game->menuSelection = 0;

    InitLevel(&game->level, MAP_NORMAL);
    InitPlayer(&game->player, 100, 100, INPUT_ARROWS, RED);
    InitPlayer(&game->player2, 130, 100, INPUT_WASD, BLUE);

    game->camera.target = (Vector2){ 0, 0 };
    game->camera.offset = (Vector2){ GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;

    game->itIndex = 0;
    game->tagCooldown = 0.0f;
    game->tagFlashTimer = 0.0f;
    game->cameraShake = 0.0f;
    game->cameraShakeTimer = 0.0f;

    game->gameWon = false;
    game->gameOver = false;
    game->gameTimedOut = false;
    game->gameTimer = TIMER_SECONDS;
    game->score = 0;
    game->jumpscareTimer = 0.0f;
    game->deathTimer = 0.0f;
    game->deathPosition = (Vector2){ 0, 0 };

    game->exploding = false;
    game->explosionTimer = 0.0f;
    game->explosionPos = (Vector2){ 0, 0 };
    game->explosionShakeTimer = 0.0f;
    game->explosionZoomKick = 0.0f;
    game->timeScale = 1.0f;
    game->chromaticFlash = false;
    game->scorchTimer = 0.0f;
    game->scorchActive = false;
    game->scorchPos = (Vector2){ 0, 0 };
    ClearDebris(game);

    ClearParticles(game);
    ClearPowerups(game);
}

// ---------------------------------------------------------------------------
// Zoom snap
// ---------------------------------------------------------------------------

static const float ZOOM_STEPS[] = { 0.5f, 0.75f, 1.0f, 1.5f, 2.0f };
#define ZOOM_STEP_COUNT (sizeof(ZOOM_STEPS) / sizeof(ZOOM_STEPS[0]))

static float SnapZoom(float raw) {
    float best = ZOOM_STEPS[0];
    float bestDist = fabsf(raw - ZOOM_STEPS[0]);
    for (int i = 1; i < (int)ZOOM_STEP_COUNT; i++) {
        float d = fabsf(raw - ZOOM_STEPS[i]);
        if (d < bestDist) { bestDist = d; best = ZOOM_STEPS[i]; }
    }
    return best;
}

// ---------------------------------------------------------------------------
// Update
// ---------------------------------------------------------------------------

void UpdateGame(Game* game, float deltaTime) {
    if (IsKeyPressed(KEY_R)) {
        game->scene = SCENE_MENU;
        game->gameOver = false;
        game->gameTimedOut = false;
        game->gameWon = false;
        game->menuSelection = 0;
        game->exploding = false;
        game->explosionTimer = 0.0f;
        game->timeScale = 1.0f;
        game->scorchActive = false;
        game->scorchTimer = 0.0f;
        game->explosionShakeTimer = 0.0f;
        game->explosionZoomKick = 0.0f;
        ClearParticles(game);
        ClearPowerups(game);
        ClearDebris(game);
        return;
    }

    if (game->scene == SCENE_MENU) {
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
            game->menuSelection = (game->menuSelection + 1) % MAP_THEME_COUNT;
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
            game->menuSelection = (game->menuSelection + MAP_THEME_COUNT - 1) % MAP_THEME_COUNT;
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            StartMatch(game, (MapTheme)game->menuSelection);
        return;
    }

    if (game->scene == SCENE_PLAY && !game->gameWon && !game->gameOver && !game->gameTimedOut) {
        UpdateLevelBackground(&game->level, deltaTime);

        UpdatePlayer(&game->player, &game->level, deltaTime);
        UpdatePlayer(&game->player2, &game->level, deltaTime);

        if (game->player.jumpedThisFrame) {
            Vector2 foot = { game->player.position.x + game->player.rec.width * 0.5f,
                             game->player.position.y + game->player.rec.height };
            SpawnDust(game, foot, 6, 18.0f, -30.0f, (Color) { 230, 230, 230, 255 });
        }
        if (game->player2.jumpedThisFrame) {
            Vector2 foot = { game->player2.position.x + game->player2.rec.width * 0.5f,
                             game->player2.position.y + game->player2.rec.height };
            SpawnDust(game, foot, 6, 18.0f, -30.0f, (Color) { 230, 230, 230, 255 });
        }

        if (game->player.landedThisFrame) {
            Vector2 foot = { game->player.position.x + game->player.rec.width * 0.5f,
                             game->player.position.y + game->player.rec.height };
            int count = (int)(6 + game->player.landImpact * 8.0f);
            SpawnDust(game, foot, count, 28.0f, -20.0f, (Color) { 210, 210, 210, 255 });
        }
        if (game->player2.landedThisFrame) {
            Vector2 foot = { game->player2.position.x + game->player2.rec.width * 0.5f,
                             game->player2.position.y + game->player2.rec.height };
            int count = (int)(6 + game->player2.landImpact * 8.0f);
            SpawnDust(game, foot, count, 28.0f, -20.0f, (Color) { 210, 210, 210, 255 });
        }

        UpdateParticles(game, deltaTime);
        UpdatePowerups(game, deltaTime);

        // Pre-explosion dust during the last 3 seconds
        if (game->gameTimer < 3.0f && game->gameTimer > 0.0f) {
            Player* tagger = (game->itIndex == 0) ? &game->player : &game->player2;
            if (GetRandomValue(0, 4) == 0) {
                Vector2 foot = {
                    tagger->position.x + tagger->rec.width * 0.5f,
                    tagger->position.y + tagger->rec.height
                };
                SpawnDust(game, foot, 2, 12.0f, -60.0f,
                    (Color) {
                    200, 180, 150, 220
                });
            }
        }

        if (game->tagCooldown > 0.0f) {
            game->tagCooldown -= deltaTime;
            if (game->tagCooldown < 0.0f) game->tagCooldown = 0.0f;
        }
        if (game->tagFlashTimer > 0.0f) {
            game->tagFlashTimer -= deltaTime;
            if (game->tagFlashTimer < 0.0f) game->tagFlashTimer = 0.0f;
        }
        if (game->cameraShakeTimer > 0.0f) {
            game->cameraShakeTimer -= deltaTime;
            if (game->cameraShakeTimer < 0.0f) game->cameraShakeTimer = 0.0f;
        }

        Player* runner = (game->itIndex == 0) ? &game->player2 : &game->player;

        if (game->tagCooldown <= 0.0f &&
            runner->shieldTimer <= 0.0f &&
            CheckCollisionRecs(game->player.rec, game->player2.rec)) {
            game->itIndex = 1 - game->itIndex;
            game->tagCooldown = TAG_COOLDOWN;
            game->tagFlashTimer = 0.4f;
            game->cameraShake = 6.0f;
            game->cameraShakeTimer = 0.25f;
        }

        game->gameTimer -= deltaTime;
        if (game->gameTimer <= 0.0f) {
            game->gameTimer = 0.0f;
            game->gameTimedOut = true;
            game->scene = SCENE_END;

            Player* tagger = (game->itIndex == 0) ? &game->player : &game->player2;
            game->explosionPos = (Vector2){
                tagger->position.x + tagger->rec.width * 0.5f,
                tagger->position.y + tagger->rec.height * 0.5f
            };
            game->explosionTimer = 0.0f;
            game->exploding = true;

            game->explosionShakeTimer = 1.5f;
            game->explosionZoomKick = 0.35f;
            game->chromaticFlash = true;

            for (int i = 0; i < MAX_DEBRIS; i++) {
                game->debris[i].active = true;
                game->debris[i].position = game->explosionPos;

                float ang = (float)i / (float)MAX_DEBRIS * 2.0f * PI
                    + ((float)GetRandomValue(-30, 30) / 100.0f);

                float speed = 200.0f + (float)GetRandomValue(0, 300);
                game->debris[i].velocity = (Vector2){
                    cosf(ang) * speed,
                    sinf(ang) * speed - 120.0f
                };

                game->debris[i].maxLife = 1.6f + (float)GetRandomValue(0, 80) / 100.0f;
                game->debris[i].life = game->debris[i].maxLife;
                game->debris[i].rotation = (float)GetRandomValue(0, 360);
                game->debris[i].spin = (float)GetRandomValue(-720, 720);
                game->debris[i].size = 3.0f + (float)GetRandomValue(0, 4);

                int pal = GetRandomValue(0, 2);
                game->debris[i].color = (pal == 0) ? (Color) { 255, 200, 100, 255 }
                : (pal == 1) ? (Color) { 255, 120, 40, 255 }
                : (Color) { 200, 60, 30, 255 };
            }
        }

        const float margin = 120.0f;
        float minX = fminf(game->player.position.x, game->player2.position.x) - margin;
        float maxX = fmaxf(game->player.position.x + game->player.rec.width,
            game->player2.position.x + game->player2.rec.width) + margin;
        float minY = fminf(game->player.position.y, game->player2.position.y) - margin;
        float maxY = fmaxf(game->player.position.y + game->player.rec.height,
            game->player2.position.y + game->player2.rec.height) + margin;

        float boxW = maxX - minX;
        float boxH = maxY - minY;

        float zoomX = (float)GetScreenWidth() / boxW;
        float zoomY = (float)GetScreenHeight() / boxH;
        float rawZoom = fminf(zoomX, zoomY);
        float targetZoom = SnapZoom(rawZoom);

        float diff = targetZoom - game->camera.zoom;
        if (fabsf(diff) < 0.01f) game->camera.zoom = targetZoom;
        else game->camera.zoom += diff * fminf(1.0f, deltaTime * 8.0f);

        float camX = (minX + maxX) * 0.5f;
        float camY = (minY + maxY) * 0.5f;

        float halfW = GetScreenWidth() / (2.0f * game->camera.zoom);
        float halfH = GetScreenHeight() / (2.0f * game->camera.zoom);

        if (game->level.worldWidth > halfW * 2.0f) {
            if (camX < halfW) camX = halfW;
            if (camX > game->level.worldWidth - halfW) camX = game->level.worldWidth - halfW;
        }
        else camX = game->level.worldWidth * 0.5f;
        if (game->level.worldHeight > halfH * 2.0f) {
            if (camY < halfH) camY = halfH;
            if (camY > game->level.worldHeight - halfH) camY = game->level.worldHeight - halfH;
        }
        else camY = game->level.worldHeight * 0.5f;

        // Tag shake
        if (game->cameraShakeTimer > 0.0f) {
            float strength = game->cameraShakeTimer / 0.25f;
            float amp = game->cameraShake * strength;
            float t = (float)GetTime() * 60.0f;
            camX += sinf(t * 1.7f) * amp;
            camY += cosf(t * 2.3f) * amp;
        }

        // Pre-explosion rumble (last 3 seconds)
        if (game->gameTimer < 3.0f && game->gameTimer > 0.0f) {
            float p = 1.0f - (game->gameTimer / 3.0f);
            float amp = 6.0f * p;
            float t = (float)GetTime() * 70.0f;
            camX += sinf(t * 5.1f) * amp;
            camY += cosf(t * 6.3f) * amp;
        }

        // Explosion shake (fires when the timer runs out — same frame)
        if (game->explosionShakeTimer > 0.0f) {
            float s = game->explosionShakeTimer / 1.5f;
            float amp = 30.0f * s * s;

            float t = (float)GetTime() * 55.0f;
            camX += sinf(t * 2.1f) * amp;
            camY += cosf(t * 2.7f) * amp;

            float t2 = (float)GetTime() * 160.0f;
            camX += sinf(t2 * 3.3f) * amp * 0.45f;
            camY += cosf(t2 * 4.1f) * amp * 0.45f;

            if (game->explosionShakeTimer > 1.3f) {
                float microAmp = (game->explosionShakeTimer - 1.3f) * 50.0f;
                camX += (float)GetRandomValue(-100, 100) / 100.0f * microAmp;
                camY += (float)GetRandomValue(-100, 100) / 100.0f * microAmp;
            }

            if (game->explosionZoomKick > 0.0f) {
                float zoomPhase = game->explosionZoomKick * 6.0f;
                float zoomAmount = sinf(zoomPhase) * game->explosionZoomKick * 0.06f;
                game->camera.zoom = game->camera.zoom * (1.0f + zoomAmount);
            }
        }

        game->camera.target = (Vector2){ camX, camY };
        return;
    }

    // End scene
    if (game->scene == SCENE_END || game->gameOver) {
        if (game->exploding && game->explosionTimer < 0.3f) {
            game->timeScale = 0.25f;
        }
        else {
            game->timeScale += (1.0f - game->timeScale) * fminf(1.0f, deltaTime * 3.0f);
            if (game->timeScale > 0.98f) game->timeScale = 1.0f;
        }

        float scaledDt = deltaTime * game->timeScale;

        UpdateParticles(game, scaledDt);

        if (game->exploding) {
            game->explosionTimer += scaledDt;
            if (game->explosionTimer > 2.5f) game->exploding = false;
        }

        if (game->explosionShakeTimer > 0.0f) {
            game->explosionShakeTimer -= deltaTime;
            if (game->explosionShakeTimer < 0.0f) game->explosionShakeTimer = 0.0f;
        }

        if (game->explosionZoomKick > 0.0f) {
            game->explosionZoomKick -= deltaTime * 1.6f;
            if (game->explosionZoomKick < 0.0f) game->explosionZoomKick = 0.0f;
        }

        if (game->chromaticFlash && game->explosionTimer > 0.12f) {
            game->chromaticFlash = false;
        }

        for (int i = 0; i < MAX_DEBRIS; i++) {
            if (!game->debris[i].active) continue;
            game->debris[i].life -= scaledDt;
            if (game->debris[i].life <= 0.0f) { game->debris[i].active = false; continue; }

            game->debris[i].velocity.y += 900.0f * scaledDt;
            game->debris[i].velocity.x *= 1.0f - 0.6f * scaledDt;
            game->debris[i].position.x += game->debris[i].velocity.x * scaledDt;
            game->debris[i].position.y += game->debris[i].velocity.y * scaledDt;
            game->debris[i].rotation += game->debris[i].spin * scaledDt;
        }

        if (game->exploding && game->explosionTimer > 0.4f && !game->scorchActive) {
            game->scorchActive = true;
            game->scorchPos = game->explosionPos;
            game->scorchTimer = 1.0f;
        }
        if (game->scorchActive) {
            game->scorchTimer -= deltaTime / 6.0f;
            if (game->scorchTimer < 0.0f) {
                game->scorchTimer = 0.0f;
                game->scorchActive = false;
            }
        }

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            game->scene = SCENE_MENU;
            game->gameOver = false;
            game->gameTimedOut = false;
            game->exploding = false;
            game->explosionTimer = 0.0f;
            game->timeScale = 1.0f;
            game->scorchActive = false;
            game->scorchTimer = 0.0f;
            game->explosionShakeTimer = 0.0f;
            game->explosionZoomKick = 0.0f;
            ClearParticles(game);
            ClearPowerups(game);
            ClearDebris(game);
        }
        return;
    }
}

// ---------------------------------------------------------------------------
// Jumpscare (dormant)
// ---------------------------------------------------------------------------

void DrawJumpscare(Game* game) {
    float screenW = GetScreenWidth();
    float screenH = GetScreenHeight();

    if ((int)(game->jumpscareTimer * 10) % 2 == 0)
        DrawRectangle(0, 0, screenW, screenH, (Color) { 50, 0, 0, 200 });
    else
        DrawRectangle(0, 0, screenW, screenH, (Color) { 0, 0, 0, 200 });

    float faceX = screenW / 2;
    float faceY = screenH / 2;
    float faceScale = 1.0f + game->jumpscareTimer * 2.0f;

    DrawCircle(faceX, faceY, 150 * faceScale, (Color) { 200, 180, 180, 255 });

    float eyeOffset = 60 * faceScale;
    float eyeSize = 40 * faceScale;

    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize, WHITE);
    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize * 0.6f, (Color) { 200, 0, 0, 255 });
    DrawCircle(faceX - eyeOffset, faceY - 30 * faceScale, eyeSize * 0.3f, BLACK);

    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize, WHITE);
    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize * 0.6f, (Color) { 200, 0, 0, 255 });
    DrawCircle(faceX + eyeOffset, faceY - 30 * faceScale, eyeSize * 0.3f, BLACK);

    float mouthWidth = 80 * faceScale;
    float mouthHeight = 40 * faceScale;
    DrawEllipse(faceX, faceY + 60 * faceScale, mouthWidth, mouthHeight, (Color) { 100, 0, 0, 255 });

    for (int i = 0; i < 6; i++) {
        float toothX = faceX - mouthWidth + (i * mouthWidth / 3);
        DrawTriangle(
            (Vector2) {
            toothX, faceY + 40 * faceScale
        },
            (Vector2) {
            toothX + 20 * faceScale, faceY + 60 * faceScale
        },
            (Vector2) {
            toothX + 40 * faceScale, faceY + 40 * faceScale
        },
            (Color) {
            255, 255, 220, 255
        });
    }
}

// ---------------------------------------------------------------------------
// IT arrow
// ---------------------------------------------------------------------------

static void DrawItArrowWorld(Player* p, Camera2D* camera) {
    Vector2 worldHead = {
        p->position.x + p->rec.width * 0.5f,
        p->position.y
    };
    Vector2 screenHead = GetWorldToScreen2D(worldHead, *camera);

    float cx = screenHead.x;
    float tipY = screenHead.y - 10.0f;
    float baseY = tipY - 14.0f;
    float halfW = 9.0f;

    Vector2 tip = { cx,         tipY };
    Vector2 right = { cx + halfW, baseY };
    Vector2 left = { cx - halfW, baseY };
    DrawTriangle(tip, right, left, WHITE);
}

// ---------------------------------------------------------------------------
// Explosion
// ---------------------------------------------------------------------------

static void DrawExplosion(Game* game) {
    if (game->explosionTimer == 0.0f && !game->exploding) return;
    if (game->explosionTimer > 2.5f && !game->scorchActive) return;

    float t = game->explosionTimer;
    Vector2 pos = game->explosionPos;

    if (game->scorchActive) {
        unsigned char a = (unsigned char)(140 * game->scorchTimer);
        float r = 40.0f + (1.0f - game->scorchTimer) * 20.0f;
        DrawEllipse((int)game->scorchPos.x,
            (int)(game->scorchPos.y + 14.0f),
            r, r * 0.35f,
            (Color) {
            20, 10, 5, a
        });
        DrawEllipse((int)game->scorchPos.x,
            (int)(game->scorchPos.y + 14.0f),
            r * 0.65f, r * 0.22f,
            (Color) {
            40, 20, 10, a
        });
    }

    if (!game->exploding && t > 2.5f) return;

    if (t > 0.02f && t < 0.55f) {
        float p = (t - 0.02f) / 0.53f;
        float len = 120.0f + p * 380.0f;
        unsigned char a = (unsigned char)(220 * (1.0f - p));

        int rayCount = 14;
        for (int i = 0; i < rayCount; i++) {
            float ang = (float)i / (float)rayCount * 2.0f * PI;
            ang += sinf((float)i * 2.3f) * 0.1f;

            Vector2 baseL = { pos.x + cosf(ang - 0.08f) * 20.0f,
                              pos.y + sinf(ang - 0.08f) * 20.0f };
            Vector2 baseR = { pos.x + cosf(ang + 0.08f) * 20.0f,
                              pos.y + sinf(ang + 0.08f) * 20.0f };
            Vector2 tip = { pos.x + cosf(ang) * len,
                              pos.y + sinf(ang) * len };

            DrawTriangle(baseL, baseR, tip,
                Fade((Color) { 255, 220, 140, 255 }, a * 0.55f));
        }
    }

    if (t < 0.55f) {
        float p = t / 0.55f;
        float r = 30.0f + p * 180.0f;
        unsigned char a = (unsigned char)(255 * (1.0f - p));

        DrawCircleV(pos, r * 1.10f, (Color) { 255, 80, 30, (unsigned char)(a * 0.9f) });
        DrawCircleV(pos, r * 0.90f, (Color) { 255, 130, 50, a });
        DrawCircleV(pos, r * 0.65f, (Color) { 255, 190, 80, a });
        DrawCircleV(pos, r * 0.42f, (Color) { 255, 235, 160, a });
        DrawCircleV(pos, r * 0.20f, (Color) { 255, 255, 240, a });
    }

    if (t > 0.03f && t < 1.2f) {
        float p = (t - 0.03f) / 1.17f;
        float r = 30.0f + p * 420.0f;
        unsigned char a = (unsigned char)(240 * (1.0f - p));

        DrawRing(pos, r, r + 3.0f, 0, 360, 64,
            (Color) {
            255, 255, 240, a
        });
        DrawRing(pos, r + 6.0f, r + 11.0f, 0, 360, 64,
            (Color) {
            255, 210, 120, (unsigned char)(a * 0.7f)
        });
        DrawRing(pos, r + 14.0f, r + 22.0f, 0, 360, 64,
            (Color) {
            255, 140, 60, (unsigned char)(a * 0.4f)
        });
    }

    for (int i = 0; i < MAX_DEBRIS; i++) {
        if (!game->debris[i].active) continue;
        float lifeFrac = game->debris[i].life / game->debris[i].maxLife;
        if (lifeFrac < 0.0f) lifeFrac = 0.0f;

        unsigned char a = (unsigned char)(255 * lifeFrac);

        Color c = game->debris[i].color;
        if (lifeFrac < 0.4f) {
            c = (Color){ 80, 40, 30, a };
        }
        else {
            c.a = a;
        }

        float sz = game->debris[i].size * (0.5f + lifeFrac * 0.5f);

        Rectangle dr = {
            game->debris[i].position.x - sz * 0.5f,
            game->debris[i].position.y - sz * 0.5f,
            sz, sz
        };
        Vector2 origin = { sz * 0.5f, sz * 0.5f };
        DrawRectanglePro(dr, origin, game->debris[i].rotation, c);
    }

    if (t > 0.25f && t < 2.2f) {
        float p = (t - 0.25f) / 1.95f;
        const int SMOKE = 12;
        for (int i = 0; i < SMOKE; i++) {
            float ang = (float)i / (float)SMOKE * 2.0f * PI + p * 0.8f;
            float dist = 25.0f + p * 130.0f;
            float px = pos.x + cosf(ang) * dist;
            float py = pos.y + sinf(ang) * dist - p * 40.0f;

            float r = 22.0f + p * 36.0f;
            unsigned char a = (unsigned char)(110 * (1.0f - p));

            DrawCircleV((Vector2) { px, py }, r, (Color) { 45, 40, 55, a });
            DrawCircleV((Vector2) { px, py }, r * 0.7f, (Color) { 75, 70, 90, (unsigned char)(a * 0.9f) });
        }
    }

    if (t < 1.0f) {
        int N = 48;
        for (int i = 0; i < N; i++) {
            float ang = (float)i / (float)N * 2.0f * PI;
            float speed = 400.0f + ((i * 73) % 250) * 2.0f;
            ang += sinf((float)i * 7.77f) * 0.25f;

            float dist = speed * t;
            float life = 1.0f - (t / 1.0f);
            if (life < 0.0f) life = 0.0f;

            float px = pos.x + cosf(ang) * dist;
            float py = pos.y + sinf(ang) * dist;
            py += 260.0f * t * t;

            unsigned char a = (unsigned char)(255 * life);

            float pxPrev = pos.x + cosf(ang) * dist * 0.92f;
            float pyPrev = pos.y + sinf(ang) * dist * 0.92f + 260.0f * t * t * 0.92f;

            DrawLineEx((Vector2) { pxPrev, pyPrev }, (Vector2) { px, py },
                2.0f, (Color) { 255, 220, 120, a });
            DrawCircleV((Vector2) { px, py }, 1.8f,
                (Color) {
                255, 255, 220, a
            });
        }
    }
}

// ---------------------------------------------------------------------------
// Menu
// ---------------------------------------------------------------------------

static void DrawMenu(Game* game) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    ClearBackground((Color) { 15, 10, 25, 255 });

    const char* title = "TAG";
    int titleW = MeasureText(title, 90);
    DrawText(title, w / 2 - titleW / 2, 40, 90, (Color) { 255, 200, 120, 255 });

    const char* subtitle = "Pick a map";
    int subW = MeasureText(subtitle, 24);
    DrawText(subtitle, w / 2 - subW / 2, 140, 24, (Color) { 200, 200, 220, 255 });

    const int cardW = 150;
    const int cardH = 150;
    const int gap = 20;
    int totalW = cardW * MAP_THEME_COUNT + gap * (MAP_THEME_COUNT - 1);
    int startX = w / 2 - totalW / 2;
    int cardY = h / 2 - 120;

    Color cardBg[MAP_THEME_COUNT] = {
        (Color) {
 120, 100, 160, 255
},
(Color) {
180, 210, 240, 255
},
(Color) {
60, 130,  70, 255
},
(Color) {
120,  70, 180, 255
},
    };

    for (int i = 0; i < MAP_THEME_COUNT; i++) {
        int cx = startX + i * (cardW + gap);
        Rectangle rec = { cx, cardY, cardW, cardH };

        DrawRectangleRec(rec, cardBg[i]);
        DrawRectangle(cx, cardY, cardW, 30, Fade(WHITE, 0.15f));

        Color border = (game->menuSelection == i) ? GOLD : (Color) { 40, 30, 60, 255 };
        DrawRectangleLinesEx(rec, 4, border);

        const char* name = GetMapThemeName((MapTheme)i);
        int nameW = MeasureText(name, 20);
        DrawText(name, cx + cardW / 2 - nameW / 2, cardY + cardH - 30, 20, WHITE);
    }

    const char* hint = "ARROWS or A/D to choose   ENTER to start";
    int hintW = MeasureText(hint, 20);
    DrawText(hint, w / 2 - hintW / 2, cardY + cardH + 15, 20,
        (Color) {
        180, 180, 200, 255
    });

    const int bottomMargin = 40;

    const char* note = "Only one power-up can be active at a time";
    int noteW = MeasureText(note, 16);
    int noteY = GetScreenHeight() - bottomMargin - 16;
    DrawText(note, w / 2 - noteW / 2, noteY, 16,
        (Color) {
        160, 160, 180, 255
    });

    const char* legend = "POWER-UPS";
    int legendW = MeasureText(legend, 22);
    int legendY = noteY - 90;
    DrawText(legend, w / 2 - legendW / 2, legendY, 22,
        (Color) {
        255, 200, 120, 255
    });

    struct Item {
        Color       color;
        const char* name;
        const char* desc;
    } items[3] = {
        { (Color) { 255, 220,  60, 255 }, "SPEED",  "Run faster for 5s"      },
        { (Color) { 100, 220, 255, 255 }, "JUMP",   "Jump higher for 5s"     },
        { (Color) { 120, 255, 140, 255 }, "SHIELD", "Can't be tagged for 4s" },
    };

    int rowY = legendY + 35;
    int itemW = 220;
    int totalItemW = itemW * 3;
    int itemStartX = w / 2 - totalItemW / 2;

    for (int i = 0; i < 3; i++) {
        int ix = itemStartX + i * itemW;
        int iconCx = ix + 30;
        int iconCy = rowY + 15;

        DrawCircle(iconCx, iconCy, 12, Fade(items[i].color, 0.30f));
        DrawCircle(iconCx, iconCy, 9, items[i].color);
        DrawCircle(iconCx, iconCy, 5, Fade(WHITE, 0.55f));

        switch (i) {
        case 0: {
            Vector2 a = { iconCx - 2, iconCy - 5 };
            Vector2 b = { iconCx + 2, iconCy - 1 };
            Vector2 c1 = { iconCx - 1, iconCy - 1 };
            Vector2 d = { iconCx + 2, iconCy + 5 };
            Vector2 e = { iconCx - 2, iconCy + 1 };
            Vector2 f = { iconCx + 1, iconCy + 1 };
            DrawTriangle(a, b, c1, BLACK);
            DrawTriangle(d, e, f, BLACK);
            break;
        }
        case 1: {
            Vector2 tip = { iconCx,     iconCy - 5 };
            Vector2 right = { iconCx + 4, iconCy - 1 };
            Vector2 left = { iconCx - 4, iconCy - 1 };
            DrawTriangle(tip, right, left, BLACK);
            DrawRectangle(iconCx - 2, iconCy - 1, 4, 6, BLACK);
            break;
        }
        case 2: {
            DrawCircleLines(iconCx, iconCy, 4, BLACK);
            DrawCircleLines(iconCx, iconCy, 3, BLACK);
            break;
        }
        }

        DrawText(items[i].name, iconCx + 25, rowY, 20, items[i].color);
        DrawText(items[i].desc, iconCx + 25, rowY + 22, 16,
            (Color) {
            220, 220, 230, 255
        });
    }
}

// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------

void DrawGame(Game* game) {
    if (game->scene == SCENE_MENU) {
        DrawMenu(game);
        return;
    }

    if (game->gameOver) {
        DrawJumpscare(game);
        return;
    }

    BeginMode2D(game->camera);
    DrawLevelBackground(&game->level, &game->camera);
    DrawLevel(&game->level);
    DrawPowerups(game);

    if (game->exploding) {
        Player* tagger = (game->itIndex == 0) ? &game->player : &game->player2;
        Player* runner = (game->itIndex == 0) ? &game->player2 : &game->player;
        if (game->explosionTimer < 0.4f) DrawPlayer(tagger);
        DrawPlayer(runner);
    }
    else {
        DrawPlayerGlow(&game->player);
        DrawPlayerGlow(&game->player2);

        DrawPlayer(&game->player);
        DrawPlayer(&game->player2);

        if (game->player.shieldTimer > 0.0f) {
            Vector2 c = { game->player.position.x + game->player.rec.width * 0.5f,
                          game->player.position.y + game->player.rec.height * 0.5f };
            unsigned char a = (unsigned char)(180 * (game->player.shieldTimer / 4.0f));
            DrawCircleLines((int)c.x, (int)c.y, 24, (Color) { 120, 255, 140, a });
            DrawCircleLines((int)c.x, (int)c.y, 26, (Color) { 120, 255, 140, (unsigned char)(a / 2) });
        }
        if (game->player2.shieldTimer > 0.0f) {
            Vector2 c = { game->player2.position.x + game->player2.rec.width * 0.5f,
                          game->player2.position.y + game->player2.rec.height * 0.5f };
            unsigned char a = (unsigned char)(180 * (game->player2.shieldTimer / 4.0f));
            DrawCircleLines((int)c.x, (int)c.y, 24, (Color) { 120, 255, 140, a });
            DrawCircleLines((int)c.x, (int)c.y, 26, (Color) { 120, 255, 140, (unsigned char)(a / 2) });
        }
    }

    DrawParticles(game);
    DrawLevelForeground(&game->level, &game->camera);

    if (game->exploding || game->scorchActive) {
        DrawExplosion(game);
    }

    EndMode2D();

    // Rumble vignette
    if (game->explosionShakeTimer > 0.0f) {
        float s = game->explosionShakeTimer / 1.5f;
        unsigned char a = (unsigned char)(110 * s);
        int W = GetScreenWidth();
        int H = GetScreenHeight();
        int band = 40;

        DrawRectangleGradientV(0, 0, W, band,
            (Color) {
            0, 0, 0, a
        },
            (Color) {
            0, 0, 0, 0
        });
        DrawRectangleGradientV(0, H - band, W, band,
            (Color) {
            0, 0, 0, 0
        },
            (Color) {
            0, 0, 0, a
        });
        DrawRectangleGradientH(0, 0, band, H,
            (Color) {
            0, 0, 0, a
        },
            (Color) {
            0, 0, 0, 0
        });
        DrawRectangleGradientH(W - band, 0, band, H,
            (Color) {
            0, 0, 0, 0
        },
            (Color) {
            0, 0, 0, a
        });
    }

    // Chromatic flash
    if (game->chromaticFlash) {
        float t = game->explosionTimer;
        float a = 1.0f - (t / 0.12f);
        if (a > 1.0f) a = 1.0f;
        if (a > 0.0f) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                Fade((Color) { 255, 60, 60, 255 }, a * 0.35f));
            DrawRectangle(6, 0, GetScreenWidth(), GetScreenHeight(),
                Fade((Color) { 60, 60, 255, 255 }, a * 0.25f));
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                Fade(WHITE, a * 0.9f));
        }
    }

    if (game->scene == SCENE_PLAY && !game->gameTimedOut) {
        Player* tagger = (game->itIndex == 0) ? &game->player : &game->player2;
        DrawItArrowWorld(tagger, &game->camera);
    }

    if (game->scene == SCENE_PLAY && !game->gameTimedOut) {
        DrawText("P1: ARROWS + UP to jump", 10, 10, 18, RED);
        DrawText("P2: WASD + W to jump", 10, 32, 18, BLUE);

        const char* itText = (game->itIndex == 0) ? "P1 IS IT!" : "P2 IS IT!";
        Color itColor = (game->itIndex == 0) ? RED : BLUE;
        DrawText(itText, 10, 58, 22, itColor);

        if (game->tagCooldown > 0.0f) {
            float frac = game->tagCooldown / TAG_COOLDOWN;
            int barW = 120;
            DrawRectangle(10, 86, barW, 6, (Color) { 0, 0, 0, 100 });
            DrawRectangle(10, 86, (int)(barW * (1.0f - frac)), 6, ORANGE);
        }

        if (game->player.speedBoostTimer > 0.0f) {
            DrawRectangle(10, 100, 40, 6, Fade((Color) { 255, 220, 60, 255 }, 0.85f));
        }
        else if (game->player.jumpBoostTimer > 0.0f) {
            DrawRectangle(10, 100, 40, 6, Fade((Color) { 100, 220, 255, 255 }, 0.85f));
        }
        else if (game->player.shieldTimer > 0.0f) {
            DrawRectangle(10, 100, 40, 6, Fade((Color) { 120, 255, 140, 255 }, 0.85f));
        }

        if (game->player2.speedBoostTimer > 0.0f) {
            DrawRectangle(GetScreenWidth() - 50, 100, 40, 6,
                Fade((Color) { 255, 220, 60, 255 }, 0.85f));
        }
        else if (game->player2.jumpBoostTimer > 0.0f) {
            DrawRectangle(GetScreenWidth() - 50, 100, 40, 6,
                Fade((Color) { 100, 220, 255, 255 }, 0.85f));
        }
        else if (game->player2.shieldTimer > 0.0f) {
            DrawRectangle(GetScreenWidth() - 50, 100, 40, 6,
                Fade((Color) { 120, 255, 140, 255 }, 0.85f));
        }

        if (game->tagFlashTimer > 0.0f) {
            float a = (game->tagFlashTimer / 0.4f) * 0.35f;
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(WHITE, a));
        }

        char timerText[16];
        int secs = (int)ceilf(game->gameTimer);
        if (secs < 0) secs = 0;
        snprintf(timerText, sizeof(timerText), "%02d", secs);

        int timerFontSize = 40;
        int timerW = MeasureText(timerText, timerFontSize);
        int timerX = GetScreenWidth() - timerW - 20;
        int timerY = 15;

        Color timerColor = WHITE;
        if (game->gameTimer <= 5.0f) {
            if ((int)(game->gameTimer * 4.0f) % 2 == 0) timerColor = RED;
            else                                        timerColor = ORANGE;
        }
        DrawText(timerText, timerX, timerY, timerFontSize, timerColor);
    }

    if (game->gameTimedOut && game->explosionTimer > 1.2f) {
        float fadeIn = (game->explosionTimer - 1.2f) / 1.0f;
        if (fadeIn > 1.0f) fadeIn = 1.0f;

        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
            Fade(BLACK, 0.7f * fadeIn));

        const char* loserText = (game->itIndex == 0)
            ? "P1 EXPLODED!"
            : "P2 EXPLODED!";
        Color loserColor = (game->itIndex == 0) ? RED : BLUE;
        int lw = MeasureText(loserText, 48);
        DrawText(loserText,
            GetScreenWidth() / 2 - lw / 2,
            GetScreenHeight() / 2 - 60, 48,
            Fade(loserColor, fadeIn));

        const char* winText = (game->itIndex == 0) ? "P2 WINS!" : "P1 WINS!";
        Color winColor = (game->itIndex == 0) ? BLUE : RED;
        int ww = MeasureText(winText, 60);
        DrawText(winText,
            GetScreenWidth() / 2 - ww / 2,
            GetScreenHeight() / 2 + 10, 60,
            Fade(winColor, fadeIn));

        const char* backText = "ENTER for menu";
        int btW = MeasureText(backText, 24);
        DrawText(backText,
            GetScreenWidth() / 2 - btW / 2,
            GetScreenHeight() / 2 + 90, 24,
            Fade(WHITE, fadeIn));
    }
}

void CleanupGame(Game* game) {
    (void)game;
}