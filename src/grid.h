#ifndef _GRID_H
#define _GRID_H

struct Grid {
  int rows;
  int cols;
  int **data;
};

struct Grid* InitializeGrid(int, int);

void PrintGrid(struct Grid*);

void FreeGrid(struct Grid*);

void DrawGameGrid(struct Grid*, int); 

#endif // !_GRID_H
