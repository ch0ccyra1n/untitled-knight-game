#ifndef _GRID_H
#define _GRID_H

#include "raylib.h"

struct Grid {
  int rows;
  int cols;
  int **data;
};

struct Grid* InitializeGrid(int, int);

void PrintGrid(struct Grid*);

void FreeGrid(struct Grid*);

void GetNewGems(int*);

int UpdateGameGrid(struct Grid*, int*);

void DrawGameGrid(struct Grid*, int, Texture2D); 

#endif // !_GRID_H
