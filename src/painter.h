#ifndef PAINTER_PAINTER_H
#define PAINTER_PAINTER_H
#include "matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Canvas_data {
  int height, width;
  int startX, startY;
  int minX, maxX, minY, maxY;
  double ratioX, ratioY;
  unsigned char * map;
} Canvas;


Canvas * createCanvas(
  int height, int width, int startX, int startY,
  int minX, int maxX, int minY, int maxY,
  double ratioX, double ratioY);
void saveCanvas(Canvas * c, const char * filename);
void drawAxis(Canvas * c, int gridX, int gridY, int color);
void drawLineChart(Canvas * c, Matrix * x, Matrix * y, int color);
void drawScatterChart(Canvas * c, Matrix * x, Matrix * y, int color);
void drawBarChart(Canvas * c, Matrix * x, Matrix * y, int color);

#ifdef __cplusplus
}
#endif

#endif // PAINTER_PAINTER_H
