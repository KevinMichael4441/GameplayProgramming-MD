# Drawing a Circle in Raylib (C)

A minimal example of how to draw a circle using [Raylib](https://www.raylib.com/) in C.

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

Save this as `circle.c`:

```c
#include "raylib.h"

int main(void)
{
    // Create a window
    const int screenWidth  = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Raylib - Draw Circle");

    // Circle properties
    Vector2 center = { (float)screenWidth / 2, (float)screenHeight / 2 };
    float radius   = 100.0f;
    Color color    = MAROON;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        // --- Update ---
        // (nothing to update in this example)

        // --- Draw ---
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // The circle
            DrawCircleV(center, radius, color);

            // Optional: outline around the circle
            DrawCircleLinesV(center, radius, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
```

## Compiling

**Linux / macOS:**
```bash
gcc circle.c -o circle -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

**macOS (with Homebrew paths):**
```bash
gcc circle.c -o circle -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib \
    -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
```

**Windows (MinGW):**
```bash
gcc circle.c -o circle.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

Then run:
```bash
./circle
```

## Key Functions

| Function | Description |
|----------|-------------|
| `DrawCircle(int cx, int cy, float radius, Color color)` | Draws a filled circle using integer coordinates. |
| `DrawCircleV(Vector2 center, float radius, Color color)` | Draws a filled circle using a `Vector2`. |
| `DrawCircleGradient(...)` | Draws a circle with a color gradient (uses vertices). |
| `DrawCircleLines(int cx, int cy, float radius, Color color)` | Draws only the circle outline. |
| `DrawCircleLinesV(Vector2 center, float radius, Color color)` | Same as above but with a `Vector2`. |
| `DrawCircleSector(...)` | Draws a pie-slice (arc) of a circle. |
| `DrawRing(...)` | Draws a ring (annulus). |

## Color Constants

Raylib includes many built-in colors you can use directly:

`RAYWHITE`, `WHITE`, `BLACK`, `RED`, `GREEN`, `BLUE`, `YELLOW`, `ORANGE`, `PURPLE`, `PINK`, `MAROON`, `DARKGRAY`, `LIGHTGRAY`, `SKYBLUE`, `LIME`, `GOLD`, `BEIGE`, `BROWN`, `DARKBLUE`, etc.

You can also define custom colors:
```c
Color myColor = (Color){ 255, 100, 50, 255 }; // R, G, B, A
```

## Variations

**Draw a circle with a custom outline color:**
```c
DrawCircleV(center, radius, BLUE);
DrawCircleLinesV(center, radius, DARKBLUE);
```

**Draw an animated (pulsing) circle:**
```c
float t = (float)GetTime();
float pulse = radius + 20.0f * sinf(t * 2.0f);
DrawCircleV(center, pulse, MAROON);
```
*(Remember to `#include <math.h>` and link with `-lm`.)*

## Notes

- Coordinate `(0, 0)` is the **top-left** of the window; Y increases **downward**.
- You can only call drawing functions between `BeginDrawing()` and `EndDrawing()`.
- Call `ClearBackground()` each frame to avoid smearing.

## References

- [Raylib Cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html)
- [Raylib Examples – Shapes](https://www.raylib.com/examples.html)
- [Raylib GitHub](https://github.com/raysan5/raylib)

## License

This example is public domain (Unlicense). Do whatever you want with it.
