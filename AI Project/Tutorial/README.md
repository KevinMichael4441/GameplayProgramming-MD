# Drawing Shapes, Collision Detection, and Cameras in Raylib (C)

A comprehensive, from-scratch guide to drawing shapes, building characters from primitives, detecting collisions, and working with the 2D camera in [Raylib](https://www.raylib.com/) using C. Everything here uses only built-in drawing functions — no textures, no asset files, nothing to load from disk.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Installing Raylib](#installing-raylib)
3. [Core Concepts](#core-concepts)
4. [Drawing Basic Shapes](#drawing-basic-shapes)
5. [The Complete Shape Reference](#the-complete-shape-reference)
6. [Colors in Raylib](#colors-in-raylib)
7. [Building Characters from Shapes](#building-characters-from-shapes)
8. [Collision Detection Tutorial](#collision-detection-tutorial)
9. [Drawing a Player Character](#drawing-a-player-character)
10. [Camera Tutorial (2D)](#camera-tutorial-2d)
11. [Compiling](#compiling)
12. [References](#references)
13. [License](#license)

---

## Prerequisites

You need two things before any of this code will run:

- **A C compiler.** `gcc` and `clang` work on Linux and macOS. On Windows, use MinGW-w64 (the `w64devkit` bundle is the easiest path) or MSVC.
- **Raylib itself**, installed and linkable from your compiler.

Raylib is a single C library with no external dependencies beyond what your OS provides (OpenGL, math, threads, X11 on Linux). That's the whole point of the library — you drop it in and you're drawing pixels the same afternoon.

## Installing Raylib

**Linux (Debian/Ubuntu):**
```bash
sudo apt install libraylib-dev
```

**macOS (Homebrew):**
```bash
brew install raylib
```

**Windows:**
Download the prebuilt binaries from the [Raylib releases page](https://github.com/raysan5/raylib/releases) and link against them, or use `w64devkit`, which bundles a compiler, Raylib headers, and the library together.

**From source:** See the [official wiki](https://github.com/raysan5/raylib/wiki). Building from source gives you the newest features and lets you customize the build (disable audio, use a different backend, etc.).

## Core Concepts

Before writing a single line of drawing code, internalize these four ideas. Almost every beginner mistake in Raylib comes from misunderstanding one of them.

### 1. Everything happens inside a window and a loop

Raylib is a *retained-window, immediate-mode* library. You open a window with `InitWindow`, then run a loop that runs until the user closes it. Each iteration of that loop is one **frame**. Inside the frame you (a) read input, (b) update your game state, and (c) draw everything from scratch. Nothing persists on screen between frames — the canvas is wiped and redrawn every time.

```c
InitWindow(width, height, "Title");
SetTargetFPS(60);
while (!WindowShouldClose()) { /* input, update, draw */ }
CloseWindow();
```

### 2. Drawing must be bracketed

You can only call drawing functions between `BeginDrawing()` and `EndDrawing()`. These calls set up the OpenGL frame, clear the previous frame's contents (if you call `ClearBackground`), and present the finished image to the window. Calling `DrawCircle` outside this bracket does nothing useful — it may silently fail or draw to the wrong buffer.

```c
BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawCircle(100, 100, 40, RED);
EndDrawing();
```

### 3. Coordinates start at the top-left

The origin `(0, 0)` is the **top-left corner** of the window. X grows to the right; **Y grows downward**. This is the opposite of the math-class Cartesian system you may be used to. A consequence: to move something "up" on the screen, you *subtract* from its Y.

### 4. Colors are RGBA bytes

Raylib represents colors as a `Color` struct with four `unsigned char` fields: red, green, blue, alpha — each 0–255. Dozens of named constants (`RED`, `MAROON`, `SKYBLUE`, etc.) are provided for convenience. You can construct your own with a compound literal: `(Color){ 255, 100, 50, 255 }`.

### 5. Shapes are drawn in order, back to front

Every draw call paints over whatever was already on the canvas at that location. There is no Z-buffer for 2D. To build a composite figure (say, a character with a body, then eyes, then pupils), draw the underlying shapes first and the details on top.

### 6. The camera doesn't move objects — it moves the view

`Camera2D` transforms how subsequent drawing calls are interpreted. Instead of translating every object as the player walks around, you leave the world coordinates fixed and slide the camera across them. Everything drawn between `BeginMode2D(camera)` and `EndMode2D()` uses **world coordinates**; everything outside uses **screen coordinates**.

## Drawing Basic Shapes

The simplest possible Raylib program that draws something:

```c
#include "raylib.h"

int main(void)
{
    InitWindow(800, 450, "Raylib - Basic Shapes");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // A filled circle at (120, 120) with radius 60
            DrawCircle(120, 120, 60, MAROON);
            // The same circle, but with a dark outline
            DrawCircleLines(120, 120, 60, DARKGRAY);

            // A rectangle at (220, 60), 120 wide and 120 tall
            DrawRectangle(220, 60, 120, 120, BLUE);
            DrawRectangleLines(220, 60, 120, 120, DARKBLUE);

            // A rounded rectangle
            DrawRectangleRounded((Rectangle){ 380, 60, 120, 120 }, 0.3f, 16, GREEN);
            DrawRectangleRoundedLines((Rectangle){ 380, 60, 120, 120 }, 0.3f, 16, DARKGREEN);

            // A triangle from three vertices
            DrawTriangle(
                (Vector2){ 600, 180 },
                (Vector2){ 540, 60  },
                (Vector2){ 660, 60  },
                ORANGE
            );
            DrawTriangleLines(
                (Vector2){ 600, 180 },
                (Vector2){ 540, 60  },
                (Vector2){ 660, 60  },
                DARKBROWN
            );

            // A regular hexagon centered at (780, 120)
            DrawPoly((Vector2){ 780, 120 }, 6, 60.0f, 0.0f, PURPLE);
            DrawPolyLines((Vector2){ 780, 120 }, 6, 60.0f, 0.0f, DARKPURPLE);

            // An ellipse with radii 70 (x) and 40 (y)
            DrawEllipse(120, 320, 70, 40, SKYBLUE);
            DrawEllipseLines(120, 320, 70, 40, DARKBLUE);

            // A thick line
            DrawLineEx((Vector2){ 220, 280 }, (Vector2){ 340, 360 }, 6.0f, RED);

            // A ring (annulus) with inner radius 30, outer 55
            DrawRing((Vector2){ 470, 320 }, 30, 55, 0, 360, 32, GOLD);
            DrawRingLines((Vector2){ 470, 320 }, 30, 55, 0, 360, 32, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

Every one of those calls does exactly one thing: it pushes a set of triangles into the draw queue in a shape that approximates a circle, rectangle, etc. That's why curve-heavy shapes like circles have no "resolution" argument — Raylib triangulates them for you.

### Variants: `V` suffix and `Lines` suffix

Raylib's naming is remarkably consistent:

- **`V` suffix** means the function takes a `Vector2` (or `Rectangle`) instead of separate `int` coordinates. `DrawCircle(cx, cy, r, c)` and `DrawCircleV(center, r, c)` do the same thing.
- **`Lines` suffix** means "outline only, no fill." `DrawRectangle` is filled; `DrawRectangleLines` is just the border. `DrawCircle` is filled; `DrawCircleLines` is just the ring.
- **`Ex` suffix** means "extended" — an enhanced version. `DrawLineEx` takes a thickness; `DrawLine` doesn't. `DrawRectangleLinesEx` takes a thickness; `DrawRectangleLines` doesn't.
- **Gradient variants** (`DrawCircleGradient`, `DrawRectangleGradientH`, `DrawRectangleGradientV`) blend two colors across the shape.

Once you know those three suffixes, you can guess the name of almost every shape function in the library.

## The Complete Shape Reference

### Basic Primitives

| Function | Description |
|----------|-------------|
| `DrawPixel(int x, int y, Color color)` | Draws a single pixel. |
| `DrawLine(int x1, int y1, int x2, int y2, Color color)` | Draws a 1-pixel-wide line. |
| `DrawLineV(Vector2 start, Vector2 end, Color color)` | Same as `DrawLine` but with vectors. |
| `DrawLineEx(Vector2 start, Vector2 end, float thick, Color color)` | Draws a thick line. |
| `DrawLineBezier(Vector2 start, Vector2 end, float thick, Color color)` | Draws a Bezier curve between two points (3rd control point is the midpoint offset). |

### Circles and Arcs

| Function | Description |
|----------|-------------|
| `DrawCircle(int cx, int cy, float radius, Color color)` | Filled circle using integer coordinates. |
| `DrawCircleV(Vector2 center, float radius, Color color)` | Filled circle using a `Vector2`. |
| `DrawCircleLines(int cx, int cy, float radius, Color color)` | Circle outline. |
| `DrawCircleLinesV(Vector2 center, float radius, Color color)` | Circle outline using a `Vector2`. |
| `DrawCircleGradient(int cx, int cy, float radius, Color inner, Color outer)` | Circle with a radial gradient from inner to outer. |
| `DrawCircleSector(Vector2 center, float radius, float startAngle, float endAngle, int segments, Color color)` | A pie-slice (arc with fill). |
| `DrawCircleSectorLines(...)` | Same as above, outline only. |
| `DrawRing(Vector2 center, float innerR, float outerR, float startAngle, float endAngle, int segments, Color color)` | Ring/annulus (a donut). |
| `DrawRingLines(...)` | Ring outline. |

### Rectangles

| Function | Description |
|----------|-------------|
| `DrawRectangle(int x, int y, int w, int h, Color color)` | Filled rectangle. |
| `DrawRectangleV(Vector2 pos, Vector2 size, Color color)` | Filled rectangle using vectors. |
| `DrawRectangleRec(Rectangle rec, Color color)` | Filled rectangle using a `Rectangle` struct. |
| `DrawRectangleLines(int x, int y, int w, int h, Color color)` | Rectangle outline. |
| `DrawRectangleLinesEx(Rectangle rec, float thick, Color color)` | Rectangle outline with a given thickness. |
| `DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color)` | Filled rounded rectangle. `roundness` is 0.0–1.0 (fraction of the shorter side). |
| `DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, Color color)` | Rounded rectangle outline. |
| `DrawRectangleGradientH(...)` / `DrawRectangleGradientV(...)` | Rectangles with a horizontal or vertical color gradient. |

### Triangles

| Function | Description |
|----------|-------------|
| `DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color)` | Filled triangle. |
| `DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color)` | Triangle outline. |
| `DrawTriangleFan(Vector2 *points, int pointCount, Color color)` | A "fan" of triangles sharing a common first point — useful for custom star shapes. |
| `DrawTriangleStrip(Vector2 *points, int pointCount, Color color)` | A strip of triangles sharing edges. |

### Polygons

| Function | Description |
|----------|-------------|
| `DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color)` | Filled regular polygon. `sides` is the number of edges. |
| `DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color)` | Polygon outline. |
| `DrawPolyLinesEx(Vector2 center, int sides, float radius, float rotation, float thick, Color color)` | Polygon outline with a chosen thickness. |

### Ellipses

| Function | Description |
|----------|-------------|
| `DrawEllipse(int cx, int cy, float rx, float ry, Color color)` | Filled ellipse. `rx`/`ry` are the horizontal and vertical radii. |
| `DrawEllipseLines(int cx, int cy, float rx, float ry, Color color)` | Ellipse outline. |

### Text

| Function | Description |
|----------|-------------|
| `DrawText(const char *text, int x, int y, int fontSize, Color color)` | Draws text using the built-in font. |
| `DrawTextEx(Font font, const char *text, Vector2 pos, float fontSize, float spacing, Color color)` | Draws text using a custom font. |
| `MeasureText(const char *text, int fontSize)` | Returns the pixel width of the text — useful for centering. |

### Rendering Modes

| Function | Description |
|----------|-------------|
| `BeginDrawing()` / `EndDrawing()` | Bracket the frame. Must wrap all drawing. |
| `BeginMode2D(Camera2D camera)` / `EndMode2D()` | Bracket world-space drawing. See the camera tutorial. |
| `BeginBlendMode(int mode)` / `EndBlendMode()` | Switch between blend modes (alpha, additive, etc.). |
| `BeginScissorMode(int x, int y, int w, int h)` / `EndScissorMode()` | Clip drawing to a rectangle. |

## Colors in Raylib

### Named Constants

Raylib defines these built-in colors as `Color` structs:

**Grayscale:** `LIGHTGRAY`, `GRAY`, `DARKGRAY`, `YELLOW`, `GOLD`, `ORANGE`, `PINK`, `RED`, `MAROON`, `GREEN`, `LIME`, `DARKGREEN`, `SKYBLUE`, `BLUE`, `DARKBLUE`, `PURPLE`, `VIOLET`, `DARKPURPLE`, `BEIGE`, `BROWN`, `DARKBROWN`, `WHITE`, `BLACK`, `BLANK`, `MAGENTA`, `RAYWHITE`.

`RAYWHITE` is Raylib's signature off-white background — a very light warm gray, easier on the eyes than pure `WHITE`.

### Custom Colors

```c
Color myColor = (Color){ 255, 100, 50, 255 }; // R, G, B, A
```

The `(Color){ ... }` syntax is a C **compound literal** — a way to construct a struct value inline. It works anywhere a `Color` is expected.

### Color Helpers

| Function | Description |
|----------|-------------|
| `Fade(Color color, float alpha)` | Returns the same color with a new alpha (0.0–1.0). Common for shadows, overlays, ghosting effects. |
| `ColorAlpha(Color color, float alpha)` | Same as `Fade`. |
| `ColorToInt(Color color)` | Packs the color into a 32-bit integer. |
| `ColorFromHSV(float hue, float sat, float val)` | Builds a color from hue/saturation/value (HSV). |
| `ColorNormalize(Color color)` | Returns a `Vector4` with components in 0.0–1.0. |
| `ColorLerp(Color a, Color b, float t)` | Blends two colors by a factor t (0.0–1.0). |

### Practical Color Tips

- `Fade(BLACK, 0.3f)` is the standard way to draw a soft shadow.
- `Fade(RAYWHITE, 0.85f)` is a good translucent panel background for HUDs.
- Cycle hues with `ColorFromHSV(fmodf(GetTime() * 60.0f, 360.0f), 0.8f, 0.9f)` for rainbow animations.

## Building Characters from Shapes

### The Core Idea

A character is just a **composition of primitives anchored to a common origin**. Instead of drawing each part at absolute coordinates scattered across your code, pick a single anchor point (usually the character's head or feet) and define every part as an offset from that anchor.

Wrap the whole thing in one function that takes the anchor as a parameter. That way the same character can be drawn anywhere on screen by changing one variable.

```c
void DrawCharacter(Vector2 pos)
{
    // Every part is drawn relative to pos.
    DrawRectangle(pos.x - 20, pos.y - 10, 40, 50, BLUE);   // body
    DrawCircle(pos.x - 8, pos.y + 5, 4, WHITE);            // left eye white
    DrawCircle(pos.x + 8, pos.y + 5, 4, WHITE);            // right eye white
    DrawCircle(pos.x - 7, pos.y + 5, 2, BLACK);            // left pupil
    DrawCircle(pos.x + 9, pos.y + 5, 2, BLACK);            // right pupil
    DrawLine(pos.x - 5, pos.y + 15, pos.x + 5, pos.y + 15, BLACK); // mouth
}
```

### Anatomy of a Humanoid

The typical order of composition for a simple humanoid, drawn back-to-front:

1. **Body / torso** — `DrawRectangle`, `DrawRectangleRounded`, or `DrawCircle`.
2. **Limbs** — thin `DrawRectangleRounded` or `DrawLineEx`.
3. **Head** — `DrawCircleV` or `DrawRectangleRounded`.
4. **Face features** — small `DrawCircleV` (eyes), `DrawLine` (mouth), `DrawTriangle` (nose).
5. **Details & highlights** — tiny circles for shine, lines for texture.
6. **Outline** — `DrawRectangleRoundedLines` etc. around any shape you want a border on.

Draw order matters because later shapes paint over earlier ones. Body first, face last.

### Making the Code Resolution-Independent

The magic numbers in a `DrawCharacter` function are usually tuned for one specific size. To make the function work at any size, express offsets as fractions of a width/height:

```c
void DrawCharacter(Vector2 pos, float w, float h)
{
    // Body
    DrawRectangle(pos.x, pos.y, w, h, BLUE);

    // Eyes at 27% and 73% across, 33% down
    DrawCircle(pos.x + w * 0.27f, pos.y + h * 0.33f, w * 0.13f, WHITE);
    DrawCircle(pos.x + w * 0.73f, pos.y + h * 0.33f, w * 0.13f, WHITE);

    // Pupils slightly offset toward the "front"
    DrawCircle(pos.x + w * 0.30f, pos.y + h * 0.33f, w * 0.07f, BLACK);
    DrawCircle(pos.x + w * 0.76f, pos.y + h * 0.33f, w * 0.07f, BLACK);

    // Mouth at 73% down, 33% wide
    DrawRectangle(pos.x + w * 0.33f, pos.y + h * 0.73f,
                  w * 0.33f, h * 0.07f, BLACK);
}
```

Now `DrawCharacter(pos, 30, 30)` draws a small version and `DrawCharacter(pos, 90, 90)` draws a large one — same silhouette.

### Parameterizing Style

A few extra parameters unlock a lot of variety without writing new draw functions:

- `Color bodyColor` — recolors the character (green hero, red enemy, gold NPC).
- `bool facingRight` — flips the pupil offset to make the character "look" left or right.
- `float t` — animation time, for blinking, bobbing, or cycle-based limb motion.

### Why This Approach Works

- **No assets to load.** A full character is 20 lines of drawing code.
- **Trivially scalable.** Change the width/height and it resizes.
- **Easy to theme.** Swap a color and you have a new character type.
- **Easy to animate.** Change an offset per frame and you get motion.
- **Composable.** A robot, a slime, a ghost — all built the same way with different primitives.

## Collision Detection Tutorial

### Overview

Raylib ships with a full set of collision helpers in `raylib.h`. The functions all follow the same idea: you check whether two shapes intersect **before** drawing, then react (stop movement, trigger damage, play a sound, etc.).

There are two main categories:

1. **Shape-vs-shape** helpers: `CheckCollisionRecs`, `CheckCollisionCircles`, `CheckCollisionCircleRec`, etc.
2. **Ray-vs-shape** helpers: `GetRayCollisionBox`, `GetRayCollisionSphere`, `GetRayCollisionMesh`, etc.

Everything here deals with the first category in 2D — the ones you'll use in practice.

### The Collision Functions You'll Actually Use

| Function | Signature | What it checks |
|----------|-----------|----------------|
| `CheckCollisionRecs` | `bool(Rectangle a, Rectangle b)` | Two axis-aligned rectangles. |
| `CheckCollisionCircles` | `bool(Vector2 c1, float r1, Vector2 c2, float r2)` | Two circles. |
| `CheckCollisionCircleRec` | `bool(Vector2 center, float radius, Rectangle rec)` | A circle and a rectangle. |
| `CheckCollisionPointRec` | `bool(Vector2 point, Rectangle rec)` | A point inside a rectangle. |
| `CheckCollisionPointCircle` | `bool(Vector2 point, Vector2 center, float radius)` | A point inside a circle. |
| `CheckCollisionPointTriangle` | `bool(Vector2 point, Vector2 p1, Vector2 p2, Vector2 p3)` | A point inside a triangle. |
| `CheckCollisionLines` | `bool(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, Vector2 *collisionPoint)` | Two line segments. |
| `CheckCollisionCircleLine` | `bool(Vector2 center, float radius, Vector2 p1, Vector2 p2)` | A circle and a line segment. |

**Critical fact:** all of these return `true` on collision and `false` otherwise. None of them resolve the collision for you. There is no physics engine inside Raylib. Detecting overlap is step one; *responding* to the overlap — pushing the objects apart, flipping velocity, triggering damage — is code you write.

### Example 1 — Two Rectangles

The simplest case. Two axis-aligned rectangles overlap if and only if their X ranges and Y ranges both overlap.

```c
Rectangle player = { 100, 100, 40, 40 };
Rectangle wall   = { 200, 100, 60, 200 };

if (CheckCollisionRecs(player, wall))
{
    // Snap the player back to the wall's left edge
    player.x = wall.x - player.width;
}
```

The response here is a hard snap: the player's right edge lands exactly on the wall's left edge. This is the foundation of "solid wall" behavior.

### Example 2 — Circle vs. Rectangle

Common for balls bouncing off paddles or bullets hitting walls.

```c
Vector2 ballPos = { 400, 300 };
float   ballR   = 15.0f;
Rectangle paddle = { 500, 250, 20, 100 };

if (CheckCollisionCircleRec(ballPos, ballR, paddle))
{
    ballVel.x *= -1.0f;                    // flip horizontal velocity
    ballPos.x = paddle.x - ballR;          // push out of the paddle
}
```

The **push-out** step is essential. If you only flip velocity but leave the ball overlapping the paddle, next frame the ball is *still* overlapping and the velocity flips again — the ball jitters in place and never escapes. Always reposition the object fully outside the shape it collided with.

### Example 3 — Circle vs. Circle (Elastic Bounce)

Two balls of equal mass bouncing off each other is the classic problem. The solution:

1. Compute the vector between the two centers.
2. Normalize it — that's the collision normal.
3. Project each ball's velocity onto the normal.
4. Swap the projections (like billiard balls).
5. Reconstruct each ball's velocity.
6. Push the balls apart by half the overlap each.

```c
typedef struct Ball {
    Vector2 pos, vel;
    float   radius;
    Color   color;
} Ball;

Ball a = { { 200, 200 }, {  2.5f,  1.5f }, 30, MAROON };
Ball b = { { 500, 300 }, { -2.0f,  1.0f }, 30, BLUE };

// Advance positions first...
a.pos = Vector2Add(a.pos, a.vel);
b.pos = Vector2Add(b.pos, b.vel);

if (CheckCollisionCircles(a.pos, a.radius, b.pos, b.radius))
{
    Vector2 delta  = Vector2Subtract(a.pos, b.pos);
    Vector2 normal = Vector2Normalize(delta);

    float aDot = Vector2DotProduct(a.vel, normal);
    float bDot = Vector2DotProduct(b.vel, normal);

    a.vel = Vector2Subtract(a.vel, Vector2Scale(normal, aDot - bDot));
    b.vel = Vector2Subtract(b.vel, Vector2Scale(normal, bDot - aDot));

    // Separate them so they don't stay overlapped
    float overlap = a.radius + b.radius - Vector2Distance(a.pos, b.pos);
    a.pos = Vector2Add(a.pos, Vector2Scale(normal,  overlap * 0.5f));
    b.pos = Vector2Add(b.pos, Vector2Scale(normal, -overlap * 0.5f));
}
```

That's about as simple as a physically plausible two-body collision gets. Unequal masses, restitution coefficients, and friction all change the math but follow the same structure.

### Example 4 — Point vs. Rectangle (Buttons and Menus)

The most common use of collision detection outside of movement: hovering over UI.

```c
Rectangle button = { 300, 200, 200, 60 };
Vector2 mouse = GetMousePosition();

bool hovered = CheckCollisionPointRec(mouse, button);
bool clicked = hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

Color fill = hovered ? LIGHTGRAY : RAYWHITE;
DrawRectangleRec(button, fill);
DrawRectangleLinesEx(button, 2.0f, DARKGRAY);

if (clicked) { /* trigger the button */ }
```

This exact pattern is how you build buttons, inventory slots, and every clickable thing in a menu.

### Example 5 — Sliding Along Walls

A player who stops dead when brushing a wall feels awful. You want them to **slide** along it. The trick: split the intended motion into an X-only attempt and a Y-only attempt, and keep whichever one doesn't collide.

```c
Rectangle next = player;

// Try the full move first.
if (IsKeyDown(KEY_RIGHT)) next.x += speed * dt;
if (IsKeyDown(KEY_LEFT))  next.x -= speed * dt;
if (IsKeyDown(KEY_DOWN))  next.y += speed * dt;
if (IsKeyDown(KEY_UP))    next.y -= speed * dt;

// For each obstacle, if the full move collides, try axes separately.
for (int i = 0; i < obstacleCount; i++)
{
    if (CheckCollisionRecs(next, obstacles[i]))
    {
        Rectangle tryX = { next.x, player.y, next.width, next.height };
        Rectangle tryY = { player.x, next.y, next.width, next.height };

        if      (!CheckCollisionRecs(tryX, obstacles[i])) next.y = player.y;
        else if (!CheckCollisionRecs(tryY, obstacles[i])) next.x = player.x;
        else { next.x = player.x; next.y = player.y; }
    }
}

player = next;
```

If the player is pressing right and down against a vertical wall, the X attempt fails but the Y attempt succeeds — so the player keeps moving down while being stopped horizontally. That's sliding.

### Example 6 — Complete Runnable Demo

This puts a movable player (WASD/arrows) against three rectangular obstacles and one circular obstacle, draws a highlight whenever they touch, and shows a live collision readout. The player slides along walls instead of sticking.

```c
#include "raylib.h"
#include <stdbool.h>

int main(void)
{
    const int W = 800, H = 450;
    InitWindow(W, H, "Raylib - Collision Demo");
    SetTargetFPS(60);

    Rectangle player = { 100, 100, 40, 40 };
    float speed = 220.0f;

    Rectangle obstacles[] = {
        { 300, 100, 120, 40  },
        { 450, 250, 40, 150  },
        { 200, 320, 150, 40  },
    };
    const int obstacleCount = sizeof(obstacles) / sizeof(obstacles[0]);

    Vector2 circleCenter = { 650, 100 };
    float   circleRadius = 45.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        Rectangle next = player;

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) next.x += speed * dt;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) next.x -= speed * dt;
        if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) next.y += speed * dt;
        if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) next.y -= speed * dt;

        // Window bounds
        if (next.x < 0) next.x = 0;
        if (next.y < 0) next.y = 0;
        if (next.x + next.width  > W) next.x = W - next.width;
        if (next.y + next.height > H) next.y = H - next.height;

        // Rectangle collisions with sliding
        bool hit = false;
        for (int i = 0; i < obstacleCount; i++)
        {
            if (CheckCollisionRecs(next, obstacles[i]))
            {
                hit = true;
                Rectangle tryX = { next.x, player.y, next.width, next.height };
                Rectangle tryY = { player.x, next.y, next.width, next.height };
                if (!CheckCollisionRecs(tryX, obstacles[i]))      next.y = player.y;
                else if (!CheckCollisionRecs(tryY, obstacles[i])) next.x = player.x;
                else { next.x = player.x; next.y = player.y; }
            }
        }

        // Circle collision
        Vector2 playerCenter = { next.x + next.width / 2, next.y + next.height / 2 };
        if (CheckCollisionCircleRec(circleCenter, circleRadius, next))
        {
            hit = true;
            Vector2 dir = Vector2Normalize(Vector2Subtract(playerCenter, circleCenter));
            float push = circleRadius + next.width / 2
                       - Vector2Distance(playerCenter, circleCenter);
            next.x += dir.x * push;
            next.y += dir.y * push;
        }

        player = next;

        BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int i = 0; i < obstacleCount; i++)
            {
                DrawRectangleRec(obstacles[i], DARKGRAY);
                DrawRectangleLinesEx(obstacles[i], 2.0f, BLACK);
            }

            DrawCircleV(circleCenter, circleRadius, SKYBLUE);
            DrawCircleLinesV(circleCenter, circleRadius, DARKBLUE);

            DrawRectangleRec(player, hit ? RED : GREEN);
            DrawRectangleLinesEx(player, 2.0f, DARKGRAY);

            DrawText(TextFormat("Collision: %s", hit ? "YES" : "no"), 10, 10, 20,
                     hit ? MAROON : DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

### Common Pitfalls

- **Tunneling.** A fast-moving object can pass entirely through a thin wall in a single frame, because at no point were the shapes overlapping during that frame's sampling. Fix by sub-stepping the movement (move in small increments and test collision after each), or by using `CheckCollisionCircleLine` / ray casts along the segment from `previousPos` to `nextPos`.
- **Sticking.** If you flip velocity without pushing the object fully out of the shape, it re-collides every frame and jitters. Always resolve penetration by repositioning, not just by changing velocity.
- **Rectangle helpers assume axis alignment.** `CheckCollisionRecs` works only for rectangles that are not rotated. Rotated boxes need SAT (Separating Axis Theorem). Raylib's `CheckCollisionBoxes` is 3D — for 2D rotation you need to write your own or use a physics library.
- **Circle-vs-rect is exact.** `CheckCollisionCircleRec` accounts for the rounded corners of the circle, not a bounding-box approximation. Useful to know when tuning hitboxes.
- **Y is inverted.** Screen Y grows downward. When you normalize a vector between two shapes, "up" is negative Y. This trips people up when writing bounce code.
- **Multiple simultaneous collisions.** If the player is touching two walls at once, resolving them one at a time in the order they appear in your array can produce different results than resolving them all at once. For most games, per-obstacle resolution works fine; for precise stacking, iterate until no collisions remain or use a physics engine.

### Going Further

- **Swept collision.** For bullets and fast projectiles, use `CheckCollisionCircleLine` on the segment from `previousPos` to `nextPos`. That checks whether the *path* crossed a wall, not just the end position.
- **Broad phase.** With hundreds of objects, checking every pair is O(n²). Bucket objects into a uniform grid and only test pairs in nearby cells.
- **Layers and masks.** Tag objects (`PLAYER`, `ENEMY`, `WALL`, `PICKUP`) and skip checks between irrelevant pairs. `PLAYER` vs `PICKUP` matters; `PICKUP` vs `PICKUP` doesn't.
- **Physics engines.** If you need rotation, joints, stacking, friction, restitution, and a solver that handles all of it robustly, drop Raylib's helpers and use Box2D or Chipmunk. Raylib integrates cleanly — you read body positions from the engine and draw them with Raylib's shape functions.

## Drawing a Player Character

This section shows how to draw a simple character (a "player") composed from multiple primitives, and how to animate its face based on game state (jumping vs. grounded).

### The Player Struct

Everything the draw function needs lives on the player struct:

```c
#include "raylib.h"
#include <stdbool.h>

typedef struct Player {
    Vector2 position;   // top-left of the bounding box
    Rectangle rec;      // collision rectangle (kept in sync with position)
    Color   color;      // body color
    bool    isJumping;  // drives the face expression
} Player;
```

`rec` is what you use for collision (`CheckCollisionRecs`, `CheckCollisionCircleRec`, etc.), while `position` is the anchor used for drawing the face features. Keeping them as separate fields is a deliberate choice: it makes the draw code readable (`position.x + 8`) while keeping the collision code correct (`rec.width`).

Keep them in sync whenever the player moves:

```c
void SyncPlayerRec(Player* player)
{
    player->rec.x = player->position.x;
    player->rec.y = player->position.y;
}
```

Call this once per frame after updating `position`.

### The Draw Function

```c
void DrawPlayer(Player* player) {
    DrawRectangleRec(player->rec, player->color);

    DrawCircle(player->position.x + 8, player->position.y + 10, 4, WHITE);
    DrawCircle(player->position.x + 22, player->position.y + 10, 4, WHITE);
    DrawCircle(player->position.x + 9, player->position.y + 10, 2, BLACK);
    DrawCircle(player->position.x + 23, player->position.y + 10, 2, BLACK);

    if (player->isJumping) {
        DrawCircle(player->position.x + 15, player->position.y + 20, 5, WHITE);
    }
    else {
        DrawLine(player->position.x + 10, player->position.y + 22,
            player->position.x + 20, player->position.y + 22, WHITE);
    }
}
```

**What each line does:**

- `DrawRectangleRec(player->rec, player->color)` draws the body as a filled rectangle using the collision rect. Drawing the body *from the collision rect* is deliberate — what you see is exactly what collides.
- The two large `DrawCircle(..., 4, WHITE)` calls draw the **eye whites** (the sclera). They're positioned relative to `position` — not `rec` — for readability.
- The two smaller `DrawCircle(..., 2, BLACK)` calls draw the **pupils** on top of the whites. Notice the pupil X coordinates are `+9` and `+23` while the whites are at `+8` and `+22` — a deliberate 1-pixel shift gives the character a "looking forward" expression.
- `if (player->isJumping)` draws an open mouth (`DrawCircle`, radius 5, WHITE) as a surprised face.
- `else` draws a flat horizontal line as a neutral face.

The whole thing is a working character in 12 lines.

### Full Runnable Example

Move with **arrow keys** or **WASD**; press **SPACE** to jump. The player's face changes while airborne, and a small shadow tracks the ground beneath them, shrinking as they rise.

```c
#include "raylib.h"
#include <stdbool.h>

#define GRAVITY    1400.0f
#define JUMP_VEL  -550.0f
#define MOVE_SPEED 220.0f

typedef struct Player {
    Vector2 position;
    Rectangle rec;
    Color   color;
    bool    isJumping;
    float   velocityY;
} Player;

void SyncPlayerRec(Player* player)
{
    player->rec.x = player->position.x;
    player->rec.y = player->position.y;
}

void DrawPlayer(Player* player) {
    DrawRectangleRec(player->rec, player->color);

    DrawCircle(player->position.x + 8, player->position.y + 10, 4, WHITE);
    DrawCircle(player->position.x + 22, player->position.y + 10, 4, WHITE);
    DrawCircle(player->position.x + 9, player->position.y + 10, 2, BLACK);
    DrawCircle(player->position.x + 23, player->position.y + 10, 2, BLACK);

    if (player->isJumping) {
        DrawCircle(player->position.x + 15, player->position.y + 20, 5, WHITE);
    }
    else {
        DrawLine(player->position.x + 10, player->position.y + 22,
            player->position.x + 20, player->position.y + 22, WHITE);
    }
}

int main(void)
{
    const int W = 800, H = 450;
    const float GROUND_Y = 380.0f;

    InitWindow(W, H, "Raylib - Draw Player");
    SetTargetFPS(60);

    Player player = {
        .position  = { 100, GROUND_Y - 30 },
        .rec       = { 100, GROUND_Y - 30, 30, 30 },
        .color     = SKYBLUE,
        .isJumping = false,
        .velocityY = 0.0f,
    };

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player.position.x += MOVE_SPEED * dt;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) player.position.x -= MOVE_SPEED * dt;

        if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            && !player.isJumping)
        {
            player.velocityY = JUMP_VEL;
            player.isJumping = true;
        }

        player.velocityY += GRAVITY * dt;
        player.position.y += player.velocityY * dt;

        if (player.position.y + player.rec.height >= GROUND_Y)
        {
            player.position.y = GROUND_Y - player.rec.height;
            player.velocityY  = 0.0f;
            player.isJumping  = false;
        }

        if (player.position.x < 0) player.position.x = 0;
        if (player.position.x + player.rec.width > W)
            player.position.x = W - player.rec.width;

        SyncPlayerRec(&player);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Ground
            DrawRectangle(0, (int)GROUND_Y, W, H - (int)GROUND_Y, DARKGREEN);
            DrawLine(0, (int)GROUND_Y, W, (int)GROUND_Y, DARKGREEN);

            // Shadow that shrinks with height
            float heightAboveGround = GROUND_Y - (player.position.y + player.rec.height);
            float shadowScale = 1.0f - (heightAboveGround / 200.0f);
            if (shadowScale < 0.3f) shadowScale = 0.3f;
            DrawEllipse(
                (int)(player.position.x + player.rec.width / 2),
                (int)GROUND_Y + 2,
                15 * shadowScale, 5 * shadowScale,
                Fade(BLACK, 0.3f)
            );

            DrawPlayer(&player);

            DrawText(TextFormat("Jumping: %s", player.isJumping ? "YES" : "no"),
                     10, 10, 20, DARKGRAY);
            DrawText("Arrows/WASD to move, SPACE to jump", 10, 35, 18, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

### How the Drawing Works, Step by Step

Because the player is drawn from several primitives, **draw order matters** — later calls paint over earlier ones:

1. **Body.** A filled rectangle covering the collision box. This is the base layer.
2. **Eye whites.** Two white circles positioned relative to `position`. Because they're drawn after the body, they sit on top.
3. **Pupils.** Two smaller black circles drawn *after* the whites, offset slightly (+1 px in X) so the character appears to look forward.
4. **Mouth.** The last shape drawn. Its form depends on `isJumping` — an open circle or a flat line. This gives a free expression change with zero extra art.

### Offsets and Anchoring

Every feature is positioned as `player->position + offset`:

- Eye white 1 → `(+8, +10)` radius 4
- Eye white 2 → `(+22, +10)` radius 4
- Pupil 1 → `(+9, +10)` radius 2 (1 px right of the eye white center)
- Pupil 2 → `(+23, +10)` radius 2
- Mouth (grounded) → horizontal line from `(+10, +22)` to `(+20, +22)`
- Mouth (jumping) → circle at `(+15, +20)` radius 5

These numbers are tuned for a **30×30** rectangle. If you change the player size, scale every offset proportionally, or rewrite the function to use fractional offsets (as shown in the "Building Characters from Shapes" section). Fractional offsets make the draw code resolution-independent — the same function draws a 30×30 player or a 90×90 boss.

### Variations

**Blinking.** Close the eyes for a fraction of a second every couple seconds:

```c
static double nextBlink  = 2.0;
static double blinkUntil = 0.0;
double now = GetTime();

if (now > nextBlink) {
    blinkUntil = now + 0.12;
    nextBlink  = now + 2.0 + GetRandomValue(0, 200) / 100.0;
}

if (now < blinkUntil) {
    DrawLine(player->position.x + 5,  player->position.y + 10,
             player->position.x + 11, player->position.y + 10, BLACK);
    DrawLine(player->position.x + 19, player->position.y + 10,
             player->position.x + 25, player->position.y + 10, BLACK);
} else {
    // normal eyes
}
```

**Facing direction.** Track a `facingRight` bool and mirror the pupil offset:

```c
float pupilDx = player->facingRight ? 1.0f : -1.0f;
DrawCircle(player->position.x + 8  + pupilDx, player->position.y + 10, 2, BLACK);
DrawCircle(player->position.x + 22 + pupilDx, player->position.y + 10, 2, BLACK);
```

**Additional expression states.** Swap the `if (isJumping)` for a `switch` over an enum (`IDLE`, `JUMPING`, `HURT`, `DEAD`). Each case draws a different mouth shape — a flat line for idle, an open circle for jump, an upside-down arc for hurt, an X for dead. Adding an expression is one case each.

**Recolor by state.** Because the body rectangle is drawn first, you can freely change `player->color` at runtime (flash red on damage, gold when invincible) without touching the face code.

### Why Draw from Primitives Instead of a Sprite?

- **Zero assets.** Nothing to load, nothing to ship.
- **Resolution-independent.** Scale the offsets and it works at any size.
- **Programmable.** Every pixel is under your control — you can animate, recolor, and recompose at runtime.
- **Fast to prototype.** A recognizable character in 12 lines of code.
- **Great for learning.** You understand exactly what every shape does because you wrote it.

When the game is real, you replace `DrawPlayer` with a sprite-based version that uses `DrawTexturePro`. Until then, primitives get you a playable character on day one.

## Camera Tutorial (2D)

### What the Camera Is

Raylib's `Camera2D` is how you give the player a view into a world larger than the window. Instead of moving every object when the player moves, you move the **camera** and let every `Draw*` call render relative to that view.

The camera does not move objects. It changes the **transform** applied to subsequent drawing calls. Objects stay put in world space; the camera slides across them.

### The Camera2D Struct

```c
typedef struct Camera2D {
    Vector2 offset;   // screen-space point the target is drawn at (usually screen center)
    Vector2 target;   // world-space point the camera is looking at
    float   rotation; // degrees, clockwise
    float   zoom;     // 1.0 = no zoom, 2.0 = 2x closer, 0.5 = zoomed out
} Camera2D;
```

The mental model:

- **`target`** is **where in the world** the camera is pointing. This is the point that ends up at `offset` on screen.
- **`offset`** is **where on the screen** that world point should land. `{ GetScreenWidth()/2, GetScreenHeight()/2 }` centers it. Setting it to `{ 0, 0 }` puts the target at the top-left of the screen.
- **`zoom`** scales the world. A zoom of 2.0 makes everything twice as big on screen (closer). A zoom of 0.5 makes everything half size (further away).
- **`rotation`** rotates the world around the target. Most 2D games leave this at 0.

### The Two Functions That Matter

| Function | Description |
|----------|-------------|
| `BeginMode2D(Camera2D camera)` | Everything drawn after this is transformed by the camera. |
| `EndMode2D(void)` | Pops back to screen-space drawing. |

Everything between `BeginMode2D` and `EndMode2D` uses **world coordinates** — the coordinates of your level. Everything outside uses **screen coordinates** — pixel coordinates on the actual window. This distinction is fundamental. Draw your game world between the brackets; draw your HUD outside them.

### Example 1 — Basic Centered Camera

A camera locked onto a player, centered on screen:

```c
#include "raylib.h"

int main(void)
{
    const int W = 800, H = 450;
    InitWindow(W, H, "Raylib - Basic Camera");
    SetTargetFPS(60);

    Vector2 player = { 400, 300 };

    Camera2D camera = { 0 };
    camera.target   = player;
    camera.offset   = (Vector2){ W / 2.0f, H / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom     = 1.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player.x += 200 * dt;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) player.x -= 200 * dt;
        if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) player.y += 200 * dt;
        if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) player.y -= 200 * dt;

        camera.target = player;

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(camera);
                // A big grid so you can see the camera moving
                for (int x = -1000; x <= 1000; x += 50)
                    DrawLine(x, -1000, x, 1000, LIGHTGRAY);
                for (int y = -1000; y <= 1000; y += 50)
                    DrawLine(-1000, y, 1000, y, LIGHTGRAY);

                // Some world objects
                DrawCircleV((Vector2){ 200, 200 }, 40, MAROON);
                DrawRectangle(600, 500, 80, 80, BLUE);
                DrawPoly((Vector2){ -300, 400 }, 6, 60, 0, PURPLE);

                // The player (world-space)
                DrawCircleV(player, 20, SKYBLUE);
                DrawCircleLinesV(player, 20, DARKBLUE);
            EndMode2D();

            // Screen-space HUD
            DrawText(TextFormat("Player: %.0f, %.0f", player.x, player.y),
                     10, 10, 20, DARKGRAY);
            DrawText("WASD to move, world scrolls", 10, 35, 18, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

Notice that the player never leaves the center of the screen — the world slides past because the camera's `target` follows the player.

### Example 2 — Zooming with the Scroll Wheel

Changing `camera.zoom` scales the world. Users expect the scroll wheel to control this.

```c
float wheel = GetMouseWheelMove();
if (wheel != 0)
{
    camera.zoom += wheel * 0.1f;
    if (camera.zoom < 0.25f) camera.zoom = 0.25f;
    if (camera.zoom > 4.0f)  camera.zoom = 4.0f;
}
```

The clamp is essential. Without it, a user can zoom to absurd factors and clip into everything (or zoom so far out that the world is a pixel). Pick a range that suits your game — 0.25x to 4x is a reasonable default.

For a "precision" modifier, hold Ctrl:

```c
if (IsKeyDown(KEY_LEFT_CONTROL)) camera.zoom += wheel * 0.02f;
else                             camera.zoom += wheel * 0.15f;
```

### Example 3 — Smooth Follow (Lerp)

A hard-locked camera feels rigid. Lerping the target gives a nice "trailing" feel, like a film camera on a slightly loose mount.

```c
float followSpeed = 5.0f;
camera.target.x += (player.x - camera.target.x) * followSpeed * GetFrameTime();
camera.target.y += (player.y - camera.target.y) * followSpeed * GetFrameTime();
```

`followSpeed` controls snappiness. Higher means tighter follow; lower means more lag. Values between 3 and 10 feel good for most games.

For a frame-rate-independent version that behaves identically whether the game runs at 60 or 144 FPS:

```c
float t = 1.0f - expf(-followSpeed * GetFrameTime());
camera.target.x += (player.x - camera.target.x) * t;
camera.target.y += (player.y - camera.target.y) * t;
```

The `expf` trick converts the linear interpolation factor into one that's independent of frame time. If you're not sure whether you need it: use the exponential version. It's never wrong.

### Example 4 — Camera Bounds (Clamping to Level)

To stop the camera from showing the void past the edges of your level, clamp `target` **after** updating it. This must account for zoom — the visible world region is larger when zoomed out.

```c
const float LEVEL_LEFT   =    0.0f;
const float LEVEL_RIGHT  = 3000.0f;
const float LEVEL_TOP    =    0.0f;
const float LEVEL_BOTTOM = 2000.0f;

// Half the screen in world units (accounting for zoom)
float halfW = (GetScreenWidth()  / 2.0f) / camera.zoom;
float halfH = (GetScreenHeight() / 2.0f) / camera.zoom;

if (camera.target.x - halfW < LEVEL_LEFT)   camera.target.x = LEVEL_LEFT   + halfW;
if (camera.target.x + halfW > LEVEL_RIGHT)  camera.target.x = LEVEL_RIGHT  - halfW;
if (camera.target.y - halfH < LEVEL_TOP)    camera.target.y = LEVEL_TOP    + halfH;
if (camera.target.y + halfH > LEVEL_BOTTOM) camera.target.y = LEVEL_BOTTOM - halfH;
```

If the level is smaller than the visible region on either axis (rare but possible when zoomed way out), the two clamps above can fight each other. Guard against it:

```c
if (LEVEL_RIGHT - LEVEL_LEFT < 2.0f * halfW)
    camera.target.x = (LEVEL_LEFT + LEVEL_RIGHT) / 2.0f;
```

### Example 5 — Screen ↔ World Coordinates

Whenever you use a camera, you need to translate between the two coordinate spaces. Common cases: spawning objects at the mouse, drawing a name tag over an entity, or checking a click against a world object.

```c
// Convert a screen point (e.g. mouse) to a world point
Vector2 worldPos = GetScreenToWorld2D(GetMousePosition(), camera);

// Convert a world point (e.g. an enemy) to a screen point
Vector2 screenPos = GetWorldToScreen2D(enemyPos, camera);

// Example: click to spawn a marker at the mouse position
if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
{
    Vector2 spawn = GetScreenToWorld2D(GetMousePosition(), camera);
    // push spawn into a list...
}
```

**Always use these helpers** instead of doing the math by hand. Zoom and rotation make manual conversion error-prone; the helpers account for everything.

If you're rendering a name tag over an entity **inside** `BeginMode2D`, you don't need any conversion — just draw at the entity's world position. Only convert if you want to draw the tag in screen space (for example, so it doesn't scale with zoom).

### Example 6 — Camera Shake (Screen Effects)

A quick shake effect for hits, explosions, stomps — anything that needs impact.

```c
float shakeTime      = 0.0f;
float shakeMagnitude = 8.0f;

if (playerHit) shakeTime = 0.25f;

if (shakeTime > 0)
{
    shakeTime -= GetFrameTime();
    camera.offset.x = (GetScreenWidth()  / 2.0f) + GetRandomValue(-shakeMagnitude, shakeMagnitude);
    camera.offset.y = (GetScreenHeight() / 2.0f) + GetRandomValue(-shakeMagnitude, shakeMagnitude);
}
else
{
    camera.offset.x = GetScreenWidth()  / 2.0f;
    camera.offset.y = GetScreenHeight() / 2.0f;
}
```

The `else` branch is critical — it restores `offset` to the screen center once the shake ends. Without it, the camera stays permanently offset and the whole world looks shifted.

### Example 7 — Full Runnable Demo

Putting it all together: a player, a big world, a grid, a smooth follow camera, zoom, clamping to level bounds, a mini-map drawn in screen space, and a screen-to-world mouse marker.

```c
#include "raylib.h"
#include <math.h>

int main(void)
{
    const int W = 900, H = 600;
    InitWindow(W, H, "Raylib - Camera Demo");
    SetTargetFPS(60);

    const float LEVEL_LEFT   = -500.0f;
    const float LEVEL_RIGHT  =  2500.0f;
    const float LEVEL_TOP    = -500.0f;
    const float LEVEL_BOTTOM =  1500.0f;

    Vector2 player = { 0, 0 };
    const float PLAYER_SPEED = 300.0f;

    Camera2D camera = { 0 };
    camera.target   = player;
    camera.offset   = (Vector2){ W / 2.0f, H / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom     = 1.0f;

    Vector2 markers[64] = { 0 };
    int markerCount = 0;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) player.x += PLAYER_SPEED * dt;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) player.x -= PLAYER_SPEED * dt;
        if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) player.y += PLAYER_SPEED * dt;
        if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) player.y -= PLAYER_SPEED * dt;

        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            camera.zoom += wheel * 0.1f;
            if (camera.zoom < 0.25f) camera.zoom = 0.25f;
            if (camera.zoom > 4.0f)  camera.zoom = 4.0f;
        }

        float t = 1.0f - expf(-5.0f * dt);
        camera.target.x += (player.x - camera.target.x) * t;
        camera.target.y += (player.y - camera.target.y) * t;

        float halfW = (W / 2.0f) / camera.zoom;
        float halfH = (H / 2.0f) / camera.zoom;
        if (camera.target.x - halfW < LEVEL_LEFT)   camera.target.x = LEVEL_LEFT   + halfW;
        if (camera.target.x + halfW > LEVEL_RIGHT)  camera.target.x = LEVEL_RIGHT  - halfW;
        if (camera.target.y - halfH < LEVEL_TOP)    camera.target.y = LEVEL_TOP    + halfH;
        if (camera.target.y + halfH > LEVEL_BOTTOM) camera.target.y = LEVEL_BOTTOM - halfH;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && markerCount < 64)
            markers[markerCount++] = GetScreenToWorld2D(GetMousePosition(), camera);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(camera);
                for (int x = (int)LEVEL_LEFT; x <= (int)LEVEL_RIGHT; x += 100)
                    DrawLine(x, (int)LEVEL_TOP, x, (int)LEVEL_BOTTOM, LIGHTGRAY);
                for (int y = (int)LEVEL_TOP; y <= (int)LEVEL_BOTTOM; y += 100)
                    DrawLine((int)LEVEL_LEFT, y, (int)LEVEL_RIGHT, y, LIGHTGRAY);

                DrawRectangleLines((int)LEVEL_LEFT, (int)LEVEL_TOP,
                                   (int)(LEVEL_RIGHT - LEVEL_LEFT),
                                   (int)(LEVEL_BOTTOM - LEVEL_TOP), DARKGRAY);

                DrawCircleV((Vector2){ 400, 300 }, 60, MAROON);
                DrawRectangle(900, 700, 120, 120, BLUE);
                DrawPoly((Vector2){ 1500, 200 }, 6, 80, 0, PURPLE);

                for (int i = 0; i < markerCount; i++)
                {
                    DrawCircleV(markers[i], 6, ORANGE);
                    DrawCircleLinesV(markers[i], 6, DARKBROWN);
                }

                DrawCircleV(player, 20, SKYBLUE);
                DrawCircleLinesV(player, 20, DARKBLUE);
            EndMode2D();

            DrawText(TextFormat("Player: %.0f, %.0f", player.x, player.y), 10, 10, 20, DARKGRAY);
            DrawText(TextFormat("Zoom: %.2fx", camera.zoom), 10, 35, 20, DARKGRAY);
            DrawText(TextFormat("Markers: %d", markerCount), 10, 60, 20, DARKGRAY);
            DrawText("WASD move | Wheel zoom | Click place marker", 10, H - 30, 18, GRAY);

            // Mini-map in the top-right (screen space)
            const float MM_W = 200, MM_H = 130;
            float mmX = W - MM_W - 10, mmY = 10;
            float sx = MM_W / (LEVEL_RIGHT - LEVEL_LEFT);
            float sy = MM_H / (LEVEL_BOTTOM - LEVEL_TOP);

            DrawRectangle((int)mmX, (int)mmY, (int)MM_W, (int)MM_H, Fade(RAYWHITE, 0.85f));
            DrawRectangleLines((int)mmX, (int)mmY, (int)MM_W, (int)MM_H, DARKGRAY);

            float px = mmX + (player.x - LEVEL_LEFT) * sx;
            float py = mmY + (player.y - LEVEL_TOP)  * sy;
            DrawCircle((int)px, (int)py, 3, RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

### Common Camera Pitfalls

- **Drawing the HUD inside `BeginMode2D`.** The HUD scrolls and zooms with the world. Always draw UI **after** `EndMode2D()`.
- **Forgetting to set `offset`.** If it stays at `{0, 0}`, the camera target lands in the top-left corner of the screen. Set it to screen center on init.
- **Changing `zoom` without updating bounds clamping.** The clamp math depends on `zoom`. Recompute every frame.
- **Manually converting screen ↔ world coordinates.** Use `GetScreenToWorld2D` and `GetWorldToScreen2D`; doing it by hand forgets rotation and the offset.
- **Recreating the camera every frame.** Build `Camera2D` once outside the loop and mutate its fields.
- **Camera shake that never resets.** Restore the base offset when the shake timer runs out, or the shake accumulates.
- **Zoom that isn't centered.** The simple version zooms around the camera target. To zoom toward the mouse, adjust `target` by the world-space delta before applying the zoom.

### Camera Function Reference

| Function | Description |
|----------|-------------|
| `BeginMode2D(Camera2D camera)` | Applies the camera transform to all subsequent draws. |
| `EndMode2D(void)` | Restores screen-space drawing. |
| `GetScreenToWorld2D(Vector2 position, Camera2D camera)` | Converts a screen point (e.g. mouse) to a world point. |
| `GetWorldToScreen2D(Vector2 position, Camera2D camera)` | Converts a world point to a screen point. |
| `GetCameraMatrix2D(Camera2D camera)` | Returns the underlying `Matrix` — rarely needed by hand. |

### Notes

- The camera does **not** move objects; it moves the **view**. Every `Draw*` between `BeginMode2D` and `EndMode2D` is transformed by the camera's target, offset, rotation, and zoom.
- World coordinates and screen coordinates are different spaces. Only HUDs and mouse input live in screen space.
- Combine with collision detection freely — collisions use world coordinates, exactly like the objects you're colliding.

## Compiling

Assuming you've saved your program as `shapes.c`:

**Linux / macOS:**
```bash
gcc shapes.c -o shapes -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

**macOS (with Homebrew paths):**
```bash
gcc shapes.c -o shapes -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib \
    -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
```

**Windows (MinGW):**
```bash
gcc shapes.c -o shapes.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

Then run:
```bash
./shapes
```

The link flags are exactly what Raylib needs on each platform: OpenGL for rendering, math for the shape triangulation, threads and dl for the internal windowing, X11 on Linux and Cocoa/IOKit/OpenGL frameworks on macOS.

## References

- [Raylib Cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html)
- [Raylib Examples – Shapes](https://www.raylib.com/examples.html)
- [Raylib Examples – 2D Camera](https://www.raylib.com/examples.html#core)
- [Raylib Collision Functions (raylib.h source)](https://github.com/raysan5/raylib/blob/master/src/raylib.h)
- [Raylib GitHub](https://github.com/raysan5/raylib)
- [Raylib Wiki](https://github.com/raysan5/raylib/wiki)

## License

This example is public domain (Unlicense). Do whatever you want with it.