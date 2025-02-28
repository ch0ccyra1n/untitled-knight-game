#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "grid.h"
#include "raylib.h"

struct Grid* InitializeGrid(int rows, int cols) {
  struct Grid* grid = malloc(sizeof(struct Grid));

  grid->rows = rows;
  grid->cols = cols;
  grid->data = (int**)malloc(rows * sizeof(int*));

  for (int row = 0; row < rows; row++) {

    grid->data[row] = (int*)malloc(cols * sizeof(int));

    for (int col = 0; col < cols; col++) {
      grid->data[row][col] = 8; // This is the equivalent of having each tile empty, since sprite 8 is intentionally left blank
      if (row == rows - 1) {
        grid->data[row][col] = 1;
      }
    }
  }

  return grid;
}

void PrintGrid(struct Grid *grid) {
  for (int row = 0; row < grid->rows; row++) {
    for (int col = 0; col < grid->cols; col++) {
      printf("%d ", grid->data[row][col]);
    }
    printf("\n");
  }
}

void FreeGrid(struct Grid *grid) {
  free(grid->data);
  free(grid);
}

void GetNewGems(int *currentGems) {
  currentGems[0] = rand() % 5;
  currentGems[1] = rand() % 5;
  currentGems[2] = rand() % 5;
}

// This may potentially need future optimizations
int UpdateGameGrid(struct Grid *grid, int *currentGems) {
  for (int row = 2; row < grid->rows - 1; row++) {
    for (int col = 0; col < grid->cols; col++) {
      // If the current gems are located above another gem OR current gems are at the bottom row
      if (grid->data[row][col] == currentGems[2] && grid->data[row-1][col] == currentGems[1] && grid->data[row-2][col] == currentGems[0] && grid->data[row+1][col] != 8) {
        // Declare the marked gems list
        struct Grid* markedGems = InitializeGrid(grid->rows, grid->cols);
        // Do four passes, checking for any match of 3 in that direction and marking gems to destroy
        for (int row1 = 0; row1 < grid->rows - 2; row1++) {
          for (int col1 = 0; col1 < grid->cols; col1++) {
            // First pass: horizontal
            if (grid->data[row1][col1] == grid->data[row1][col1+1] && grid->data[row1][col1] == grid->data[row1][col1-1]) {
              // Add all three locations to the marked gems list
              markedGems->data[row1][col1] = 1;
              markedGems->data[row1][col1+1] = 1;
              markedGems->data[row1][col1-1] = 1;
            }
            // Second pass: vertical
            if (grid->data[row1][col1] == grid->data[row1+1][col1] && grid->data[row1][col1] == grid->data[row1+2][col1]) {
              // Add all three locations to the marked gems list
              markedGems->data[row1][col1] = 1;
              markedGems->data[row1+1][col1] = 1;
              markedGems->data[row1+2][col1] = 1;
            }
            // Third pass: diagonal
            // Fourth pass: alternative diagonal
          }
        }
        // Destroy all marked gems, adding to the player's score for each gem
        for (int row2 = 0; row2 < grid->rows-1; row2++) {
          for (int col2 = 0; col2 < grid->cols; col2++) {
            if (markedGems->data[row2][col2] == 1) {
              grid->data[row2][col2] = 8;
              // Add amount to player score
            }
          }
        }
        // Check if it is possible to put new gems in the middle at the top
        if (grid->data[0][3] == 8 && grid->data[1][3] == 8 && grid->data[2][3] == 8) {
          // Put new gems in middle at the top
          GetNewGems(currentGems);
          grid->data[0][3] = currentGems[0];
          grid->data[1][3] = currentGems[1];
          grid->data[2][3] = currentGems[2];
          // End the for loops
          FreeGrid(markedGems);
          return 0;
        } else {
          // Game over!
          FreeGrid(markedGems);
          return 1;
        }
      }
    }
  }
  return 0;
}

void DrawGameGrid(struct Grid *grid, int cellSize, Texture2D gemTexture) {
  for (int row = 0; row < grid->rows; row++) {
    for (int col = 0; col < grid->cols; col++) {
      int cellValue = grid->data[row][col];
      Vector2 texturePosition = { col * cellSize + 1 + 215 , row * cellSize + 1 };
      Rectangle gemRec = {cellValue * 32,0,cellSize - 1,cellSize - 1};
      DrawRectangle(col * cellSize+1+215, row * cellSize+1, cellSize-1, cellSize-1, DARKBLUE);
      DrawTextureRec(gemTexture, gemRec, texturePosition, WHITE);
    }
  }
}
