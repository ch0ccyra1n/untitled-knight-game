#include "raylib.h"
#include "grid.h"

#define ROWS 15
#define COLS 6
#define CELL_SIZE 32

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

const unsigned int screenWidth = 640;
const unsigned int screenHeight = 480;
const char *title = "untitled knight game";

int main(int argc, char *argv[]) {
  Color darkBlue = {44, 44, 127, 255};

  struct Grid* grid = InitializeGrid(ROWS, COLS);
  struct Grid* temp_grid = InitializeGrid(ROWS, COLS);

  PrintGrid(grid);

  InitWindow(screenWidth, screenHeight, title); // Initializes the game window
  SetTargetFPS(60);       // Set our game to run at 60 frames-per-second

  InitAudioDevice(); // Currently unused, but will be useful later when we add music and sfx

  while (WindowShouldClose() == false) {
    BeginDrawing();
    ClearBackground(darkBlue); // Dark blue background
    DrawGameGrid(grid, CELL_SIZE);
    EndDrawing();
  }

  FreeGrid(grid);

  CloseWindow();

  return 0;
}
