#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "grid.h"
#include "colors.h"
#include "raylib.h"

struct Grid* InitializeGrid(int rows, int cols) {
  struct Grid* grid = malloc(sizeof(struct Grid));

  grid->rows = rows;
  grid->cols = cols;
  grid->data = (int**)malloc(rows * sizeof(int*));

  for (int row = 0; row < rows; row++) {

    grid->data[row] = (int*)malloc(cols * sizeof(int));

    for (int col = 0; col < cols; col++) {
      grid->data[row][col] = 0;
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

void DrawGameGrid(struct Grid *grid, int cellSize) {
  for (int row = 0; row < grid->rows; row++) {
    for (int col = 0; col < grid->cols; col++) {
      int cellValue = grid->data[row][col];
      DrawRectangle(col * cellSize+1+220, row * cellSize+1, cellSize-1, cellSize-1, colors[cellValue]);
    }
  }
}
