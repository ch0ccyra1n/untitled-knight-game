#include "raylib.h"
#include "grid.h"

#define ROWS 15
#define COLS 6
#define CELL_SIZE 32

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

typedef enum GameScreen { LOGO = 0, TITLE, ENDLESS } GameScreen;

const unsigned int screenWidth = 640;
const unsigned int screenHeight = 480;
const char *title = "untitled knight game (demo)";


int main(int argc, char *argv[]) {
  Color darkBlue = {44, 44, 127, 255};

  struct Grid* grid = InitializeGrid(ROWS, COLS);
  struct Grid* temp_grid = InitializeGrid(ROWS, COLS);

  PrintGrid(grid);

  InitWindow(screenWidth, screenHeight, title); // Initializes the game window
  GameScreen currentScreen = LOGO;
  SetTargetFPS(60);       // Set our game to run at 60 frames-per-second

  InitAudioDevice(); // Currently unused, but will be useful later when we add music and sfx

  Music titleMusic = LoadMusicStream("resources/ida_chip003.xm");
  Music endlessMusic = LoadMusicStream("resources/ida_game-disco.xm");

  // Variables used for Raylib logo screen
  int logoPositionX = screenWidth/2 - 128;
  int logoPositionY = screenHeight/2 - 128;

  int framesCounter = 0;
  int lettersCount = 0;

  int topSideRecWidth = 16;
  int leftSideRecHeight = 16;

  int bottomSideRecWidth = 16;
  int rightSideRecHeight = 16;

  int state = 0;                  // Tracking animation states (State Machine)
  float alpha = 1.0f;             // Useful for fading

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    UpdateMusicStream(titleMusic);
    UpdateMusicStream(endlessMusic);
    switch (currentScreen) {
      case LOGO: {
        // Update
        //----------------------------------------------------------------------------------
        if (state == 0)                 // State 0: Small box blinking
        {
            framesCounter++;

            if (framesCounter == 120)
            {
                state = 1;
                framesCounter = 0;      // Reset counter... will be used later...
            }
        }
        else if (state == 1)            // State 1: Top and left bars growing
        {
            topSideRecWidth += 4;
            leftSideRecHeight += 4;

            if (topSideRecWidth == 256) state = 2;
        }
        else if (state == 2)            // State 2: Bottom and right bars growing
        {
            bottomSideRecWidth += 4;
            rightSideRecHeight += 4;

            if (bottomSideRecWidth == 256) state = 3;
        }
        else if (state == 3)            // State 3: Letters appearing (one by one)
        {
            framesCounter++;

            if (framesCounter/12)       // Every 12 frames, one more letter!
            {
                lettersCount++;
                framesCounter = 0;
            }

            if (lettersCount >= 10)     // When all letters have appeared, just fade out everything
            {
                alpha -= 0.02f;

                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    state = 4;
                }
            }
        }
        else if (state == 4)            // State 4: Reset and Replay
        {
          currentScreen = TITLE;
        }

        if (state == 0)
            {
              if ((framesCounter/15)%2) DrawRectangle(logoPositionX, logoPositionY, 16, 16, BLACK);
            }
            else if (state == 1)
            {
                DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
                DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);
            }
            else if (state == 2)
            {
                DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
                DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);

                DrawRectangle(logoPositionX + 240, logoPositionY, 16, rightSideRecHeight, BLACK);
                DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, BLACK);
            }
            else if (state == 3)
            {
                DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, Fade(BLACK, alpha));
                DrawRectangle(logoPositionX, logoPositionY + 16, 16, leftSideRecHeight - 32, Fade(BLACK, alpha));

                DrawRectangle(logoPositionX + 240, logoPositionY + 16, 16, rightSideRecHeight - 32, Fade(BLACK, alpha));
                DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, Fade(BLACK, alpha));

                DrawRectangle(GetScreenWidth()/2 - 112, GetScreenHeight()/2 - 112, 224, 224, Fade(RAYWHITE, alpha));

                DrawText(TextSubtext("raylib", 0, lettersCount), GetScreenWidth()/2 - 44, GetScreenHeight()/2 + 48, 50, Fade(BLACK, alpha));
            }
      } break;
      case TITLE: {
        PlayMusicStream(titleMusic);
        DrawText("Untitled Knight Game (demo)", 20, 20, 40, LIGHTGRAY);
        DrawText("Press Enter, Tap, or Press Start", 20, 220, 20, GRAY);
        DrawText("Music by ida_deerz (CC BY-NC)", 20, 240, 20, LIGHTGRAY);
        if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
          currentScreen = ENDLESS;
        } break;
      }
      case ENDLESS: {
        StopMusicStream(titleMusic);
        PlayMusicStream(endlessMusic);
        ClearBackground(darkBlue);
        DrawGameGrid(grid, CELL_SIZE);
      } break;
      default: break;
    }
    EndDrawing();
  }

  FreeGrid(grid);

  CloseWindow();

  return 0;
}
