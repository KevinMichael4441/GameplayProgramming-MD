# Drawing Shapes in Raylib (C)

A minimal example of how to draw various shapes using [Raylib](https://www.raylib.com/) in C, including simple primitives and a composite character built from multiple shapes.

## Prerequisites

- A C compiler (`gcc`, `clang`, or MSVC)
- Raylib installed on your system

### Installing Raylib

**Linux (Debian/Ubuntu):**
```bash
sudo apt install libraylib-dev
```

**macOS (Homebrew):**
```bash
brew install raylib
```

**Windows:**
Download the prebuilt binaries from the [Raylib releases page](https://github.com/raysan5/raylib/releases) and link against them, or use `w64devkit`.

**From source:** See the [official wiki](https://github.com/raysan5/raylib/wiki).

## The Code

Save this as `shapes.c`:

```c
#include "raylib.h"
#include <math.h>

int main(void)
{
    const int screenWidth  = 900;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Raylib - Draw Shapes");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // ---------- Circle ----------
            DrawCircleV((Vector2){ 120, 120 }, 60, MAROON);
            DrawCircleLinesV((Vector2){ 120, 120 }, 60, DARKGRAY);

            // ---------- Rectangle ----------
            DrawRectangle(220, 60, 120, 120, BLUE);
            DrawRectangleLines(220, 60, 120, 120, DARKBLUE);

            // ---------- Rounded Rectangle ----------
            DrawRectangleRounded((Rectangle){ 380, 60, 120, 120 }, 0.3f, 16, GREEN);
            DrawRectangleRoundedLines((Rectangle){ 380, 60, 120, 120 }, 0.3f, 16, DARKGREEN);

            // ---------- Triangle ----------
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

            // ---------- Polygon (Hexagon) ----------
            Vector2 hexCenter = { 780, 120 };
            float   hexRadius = 60.0f;
            int     hexSides  = 6;
            DrawPoly(hexCenter, hexSides, hexRadius, 0.0f, PURPLE);
            DrawPolyLines(hexCenter, hexSides, hexRadius, 0.0f, DARKPURPLE);

            // ---------- Ellipse ----------
            DrawEllipse(120, 320, 70, 40, SKYBLUE);
            DrawEllipseLines(120, 320, 70, 40, DARKBLUE);

            // ---------- Line / Thick Line ----------
            DrawLineEx((Vector2){ 220, 280 }, (Vector2){ 340, 360 }, 6.0f, RED);

            // ---------- Ring ----------
            DrawRing((Vector2){ 470, 320 }, 30, 55, 0, 360, 32, GOLD);
            DrawRingLines((Vector2){ 470, 320 }, 30, 55, 0, 360, 32, DARKGRAY);

            // ---------- Star (custom, built from triangles) ----------
            DrawStar((Vector2){ 650, 320 }, 5, 30, 65, YELLOW);

            // ---------- Composite Character: a simple robot ----------
            DrawRobot((Vector2){ 780, 320 });

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

### Helper: A character made from multiple shapes

Add this function above `main()`:

```c
// Draws a simple robot character made of basic shapes.
void DrawRobot(Vector2 pos)
{
    // pos is the center of the robot's head

    // --- Antenna ---
    DrawLineEx((Vector2){ pos.x, pos.y - 45 }, (Vector2){ pos.x, pos.y - 75 }, 3.0f, DARKGRAY);
    DrawCircleV((Vector2){ pos.x, pos.y - 80 }, 6, RED);

    // --- Head ---
    DrawRectangleRounded(
        (Rectangle){ pos.x - 40, pos.y - 45, 80, 70 },
        0.3f, 12, LIGHTGRAY
    );
    DrawRectangleRoundedLines(
        (Rectangle){ pos.x - 40, pos.y - 45, 80, 70 },
        0.3f, 12, DARKGRAY
    );

    // --- Eyes ---
    DrawCircleV((Vector2){ pos.x - 15, pos.y - 15 }, 7, BLACK);
    DrawCircleV((Vector2){ pos.x + 15, pos.y - 15 }, 7, BLACK);
    DrawCircleV((Vector2){ pos.x - 13, pos.y - 17 }, 2, WHITE); // highlight
    DrawCircleV((Vector2){ pos.x + 17, pos.y - 17 }, 2, WHITE);

    // --- Mouth ---
    DrawRectangle(pos.x - 18, pos.y + 8, 36, 5, DARKGRAY);
    for (int i = 0; i < 4; i++)
        DrawLine(pos.x - 12 + i * 8, pos.y + 8, pos.x - 12 + i * 8, pos.y + 13, LIGHTGRAY);

    // --- Body ---
    DrawRectangleRounded(
        (Rectangle){ pos.x - 45, pos.y + 30, 90, 80 },
        0.2f, 12, BLUE
    );
    DrawRectangleRoundedLines(
        (Rectangle){ pos.x - 45, pos.y + 30, 90, 80 },
        0.2f, 12, DARKBLUE
    );

    // Chest light
    DrawCircleV((Vector2){ pos.x, pos.y + 65 }, 10, GREEN);
    DrawCircleLinesV((Vector2){ pos.x, pos.y + 65 }, 10, DARKGREEN);

    // --- Arms ---
    DrawRectangleRounded(
        (Rectangle){ pos.x - 75, pos.y + 35, 25, 65 },
        0.4f, 8, LIGHTGRAY
    );
    DrawRectangleRounded(
        (Rectangle){ pos.x + 50, pos.y + 35, 25, 65 },
        0.4f, 8, LIGHTGRAY
    );

    // --- Legs ---
    DrawRectangleRounded(
        (Rectangle){ pos.x - 35, pos.y + 110, 25, 55 },
        0.3f, 8, DARKGRAY
    );
    DrawRectangleRounded(
        (Rectangle){ pos.x + 10, pos.y + 110, 25, 55 },
        0.3f, 8, DARKGRAY
    );

    // --- Feet ---
    DrawRectangleRounded(
        (Rectangle){ pos.x - 40, pos.y + 160, 35, 15 },
        0.5f, 8, BLACK
    );
    DrawRectangleRounded(
        (Rectangle){ pos.x + 5, pos.y + 160, 35, 15 },
        0.5f, 8, BLACK
    );
}
```

## Compiling

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

## Key Shape Functions

### Basic Primitives

| Function | Description |
|----------|-------------|
| `DrawPixel(int x, int y, Color color)` | Draws a single pixel. |
| `DrawLine(int x1, int y1, int x2, int y2, Color color)` | Draws a 1px line. |
| `DrawLineEx(Vector2 start, Vector2 end, float thick, Color color)` | Draws a thick line. |
| `DrawCircle(int cx, int cy, float radius, Color color)` | Filled circle (integer coords). |
| `DrawCircleV(Vector2 center, float radius, Color color)` | Filled circle using `Vector2`. |
| `DrawCircleLines(int cx, int cy, float radius, Color color)` | Circle outline. |
| `DrawCircleLinesV(Vector2 center, float radius, Color color)` | Circle outline using `Vector2`. |
| `DrawCircleGradient(...)` | Circle with a color gradient. |
| `DrawCircleSector(...)` | Pie-slice (arc) of a circle. |
| `DrawRing(Vector2 center, float innerR, float outerR, float startAngle, float endAngle, int segments, Color color)` | Ring/annulus. |
| `DrawRingLines(...)` | Ring outline. |

### Rectangles

| Function | Description |
|----------|-------------|
| `DrawRectangle(int x, int y, int w, int h, Color color)` | Filled rectangle. |
| `DrawRectangleRec(Rectangle rec, Color color)` | Filled rectangle using `Rectangle`. |
| `DrawRectangleLines(int x, int y, int w, int h, Color color)` | Rectangle outline. |
| `DrawRectangleLinesEx(Rectangle rec, float thick, Color color)` | Thick rectangle outline. |
| `DrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color)` | Filled rounded rectangle. |
| `DrawRectangleRoundedLines(Rectangle rec, float roundness, int segments, Color color)` | Rounded rectangle outline. |
| `DrawRectangleGradientH(...)` / `DrawRectangleGradientV(...)` | Gradient rectangles. |

### Triangles & Polygons

| Function | Description |
|----------|-------------|
| `DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color)` | Filled triangle. |
| `DrawTriangleLines(Vector2 v1, Vector2 v2, Vector2 v3, Color color)` | Triangle outline. |
| `DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color)` | Regular polygon (e.g. hexagon). |
| `DrawPolyLines(Vector2 center, int sides, float radius, float rotation, Color color)` | Polygon outline. |
| `DrawPolyLinesEx(...)` | Polygon outline with thick lines. |

### Ellipses

| Function | Description |
|----------|-------------|
| `DrawEllipse(int cx, int cy, float rx, float ry, Color color)` | Filled ellipse. |
| `DrawEllipseLines(int cx, int cy, float rx, float ry, Color color)` | Ellipse outline. |

### Text

| Function | Description |
|----------|-------------|
| `DrawText(const char *text, int x, int y, int fontSize, Color color)` | Draws text. |
| `DrawTextEx(Font font, const char *text, Vector2 pos, float fontSize, float spacing, Color color)` | Custom-font text. |

## Color Constants

Raylib includes many built-in colors you can use directly:

`RAYWHITE`, `WHITE`, `BLACK`, `RED`, `GREEN`, `BLUE`, `YELLOW`, `ORANGE`, `PURPLE`, `PINK`, `MAROON`, `DARKGRAY`, `LIGHTGRAY`, `SKYBLUE`, `LIME`, `GOLD`, `BEIGE`, `BROWN`, `DARKBLUE`, `DARKGREEN`, `DARKPURPLE`, `DARKBROWN`, etc.

You can also define custom colors:
```c
Color myColor = (Color){ 255, 100, 50, 255 }; // R, G, B, A
```

## Building Characters from Shapes

A character is just a composition of primitives anchored to a common origin. The trick is to define a single `Vector2 pos` for the character and offset every shape relative to it. For example, a humanoid:

- **Head** → `DrawCircleV` or `DrawRectangleRounded`
- **Eyes** → small `DrawCircleV` pairs
- **Mouth** → `DrawRectangle` or `DrawLine`
- **Body** → `DrawRectangleRounded`
- **Arms & legs** → thin `DrawRectangleRounded` or `DrawLineEx`
- **Details** → `DrawCircleV`, `DrawTriangle`, `DrawPoly`

Wrap it all in one function like `DrawRobot(Vector2 pos)` so you can place multiple copies on screen by changing `pos`.

## Variations

**Animated (pulsing) circle:**
```c
float t = (float)GetTime();
float pulse = 60.0f + 20.0f * sinf(t * 2.0f);
DrawCircleV(center, pulse, MAROON);
```
*(Remember to `#include <math.h>` and link with `-lm`.)*

**Blinking robot eye:**
```c
if (((int)(GetTime() * 2)) % 2 == 0)
    DrawCircleV(eyePos, 7, BLACK);   // open
else
    DrawLineEx((Vector2){ eyePos.x - 7, eyePos.y },
               (Vector2){ eyePos.x + 7, eyePos.y }, 2.0f, BLACK); // closed
```

## Notes

- Coordinate `(0, 0)` is the **top-left** of the window; Y increases **downward**.
- You can only call drawing functions between `BeginDrawing()` and `EndDrawing()`.
- Call `ClearBackground()` each frame to avoid smearing.
- Draw order matters — later shapes appear on top of earlier ones. Sketch back-to-front (body, then details, then highlights).
- `DrawStar` in the example above is not a Raylib built-in — define it yourself, or use `DrawPoly`/triangles to compose stars.

### Quick custom `DrawStar` implementation

```c
void DrawStar(Vector2 center, int points, float innerRadius, float outerRadius, Color color)
{
    for (int i = 0; i < points * 2; i++)
    {
        float r1 = (i % 2 == 0) ? outerRadius : innerRadius;
        float r2 = ((i + 1) % 2 == 0) ? outerRadius : innerRadius;
        float a1 = (float)i       / (points * 2) * 2.0f * PI - PI / 2;
        float a2 = (float)(i + 1) / (points * 2) * 2.0f * PI - PI / 2;

        DrawTriangle(
            center,
            (Vector2){ center.x + cosf(a1) * r1, center.y + sinf(a1) * r1 },
            (Vector2){ center.x + cosf(a2) * r2, center.y + sinf(a2) * r2 },
            color
        );
    }
}
```

## References

- [Raylib Cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html)
- [Raylib Examples – Shapes](https://www.raylib.com/examples.html)
- [Raylib GitHub](https://github.com/raysan5/raylib)

## License

This example is public domain (Unlicense). Do whatever you want with it.
