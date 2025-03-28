#include "raylib.h"
#include "grid.h"

#define ROWS 16
#define COLS 6
#define CELL_SIZE 32

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

typedef enum GameScreen { LOGO = 0, TITLE, ENDLESS, GAMEOVER } GameScreen;

const unsigned int screenWidth = 640;
const unsigned int screenHeight = 480;
const char *title = "untitled knight game (demo)";

int gems[] = {0, 0, 0};

int main(int argc, char *argv[]) {
  Color darkBlue = {44, 44, 127, 255};

  struct Grid* grid = InitializeGrid(ROWS, COLS);
  struct Grid* temp_grid = InitializeGrid(ROWS, COLS);

  GetNewGems(gems);
  // Put the gems at the top-center of the board
  grid->data[0][3] = gems[0];
  grid->data[1][3] = gems[1];
  grid->data[2][3] = gems[2];

  PrintGrid(grid);

  InitWindow(screenWidth, screenHeight, title); // Initializes the game window. Don't load in textures until afterwards or you WILL get a segfault!
  GameScreen currentScreen = LOGO;
  SetTargetFPS(60);       // Set our game to run at 60 frames-per-second

  InitAudioDevice(); // Currently unused, but will be useful later when we add music and sfx

  Music titleMusic = LoadMusicStream("resources/music/ida_game-disco.xm");
  Music endlessMusic = LoadMusicStream("resources/music/ch0ccy_plainsloop.xm");

  Texture2D gemTexture = LoadTexture("resources/sprites/gems_dennis.png");

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
        if (state == 0) {                 // State 0: Small box blinking
            framesCounter++;

            if (framesCounter == 120)
            {
                state = 1;
                framesCounter = 0;      // Reset counter... will be used later...
            }
        }
        else if (state == 1) {            // State 1: Top and left bars growing
            topSideRecWidth += 4;
            leftSideRecHeight += 4;

            if (topSideRecWidth == 256) state = 2;
        }
        else if (state == 2) {           // State 2: Bottom and right bars growing
            bottomSideRecWidth += 4;
            rightSideRecHeight += 4;

            if (bottomSideRecWidth == 256) state = 3;
        }
        else if (state == 3) {           // State 3: Letters appearing (one by one)
            framesCounter++;

            if (framesCounter/12) {      // Every 12 frames, one more letter!
                lettersCount++;
                framesCounter = 0;
            }

            if (lettersCount >= 10) {    // When all letters have appeared, just fade out everything
                alpha -= 0.02f;

                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    state = 4;
                }
            }
        }
        else if (state == 4) {           // State 4: Go to title screen
          framesCounter = 0;
          currentScreen = TITLE;
        }

        if (state == 0) {
          if ((framesCounter/15)%2) DrawRectangle(logoPositionX, logoPositionY, 16, 16, BLACK);
        } else if (state == 1) {
          DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
          DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);
        } else if (state == 2) {
          DrawRectangle(logoPositionX, logoPositionY, topSideRecWidth, 16, BLACK);
          DrawRectangle(logoPositionX, logoPositionY, 16, leftSideRecHeight, BLACK);

          DrawRectangle(logoPositionX + 240, logoPositionY, 16, rightSideRecHeight, BLACK);
          DrawRectangle(logoPositionX, logoPositionY + 240, bottomSideRecWidth, 16, BLACK);
        } else if (state == 3) {
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
      } break;
      case GAMEOVER: {
        StopMusicStream(endlessMusic);
        ClearBackground(WHITE);
        DrawText("Game Over!", 20, 20, 40, RED);
        // Draw score right below it
        DrawText("Your score was: UNIMPLEMENTED", 20, 220, 20, BLUE);
        DrawText("Press Enter, Tap, or Press Start to return to title", 20, 240, 20, LIGHTGRAY);
        if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP) || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
          currentScreen = TITLE;
        } break;
      } break;
      case ENDLESS: {
        framesCounter++;
        StopMusicStream(titleMusic);
        PlayMusicStream(endlessMusic);
        ClearBackground(darkBlue);
        // TODO: Check for user input, move gems accordingly
        // Update grid at set interval
        if (framesCounter == 30) { // the number here should be dynamically changed over time
          // Update Grid
          int gameState = UpdateGameGrid(grid, gems);
          if (gameState == 1) {
            // Game Over!
            framesCounter = 0;
            currentScreen = GAMEOVER;
            break;
          } else if (gameState == 0) {
          // Move current gems down
          for (int row3 = 2; row3 < grid->rows; row3++) {
            for (int col3 = 0; col3 < grid->cols; col3++) {
              if (grid->data[row3][col3] == gems[2] && grid->data[row3-1][col3] == gems[1] && grid->data[row3-2][col3] == gems[0]) {
                grid->data[row3-2][col3] = 8;
                grid->data[row3-1][col3] = gems[0];
                grid->data[row3][col3] = gems[1];
                grid->data[row3+1][col3] = gems[2];
                goto exit_loop;
              }
            }
          }
          exit_loop:
            framesCounter = 0;
        }
        }
        DrawGameGrid(grid, CELL_SIZE, gemTexture);
      } break;
      default: break;
    }
    EndDrawing();
  }

  UnloadTexture(gemTexture);

  FreeGrid(grid);

  CloseAudioDevice();

  CloseWindow();

  return 0;
}
