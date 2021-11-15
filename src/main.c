#include <stdio.h>
#include <string.h>
#include "matrix.h"
#include "painter.h"

int main() {
  printf("Entering\n");
  Canvas * c;
  Matrix * xs = createMatrix(1, 8);
  Matrix * ys = createMatrix(1, 8);
  double pxs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  double pys[] = {-2, 3, 4, -1, -2, 0, 1, 3, 2};
  memcpy(xs->data[0], pxs, sizeof(double) * 8);
  memcpy(ys->data[0], pys, sizeof(double) * 8);

  c = createCanvas(1200, 1200, 100, 100, 0, 10, -5, 5, 100, 100);
  drawAxis(c, 1, 1, 0xB0B0B0);
  drawLineChart(c, xs, ys, 0x0000FF);
  saveCanvas(c, "./output/chart_line.bmp");

  c = createCanvas(1200, 1200, 100, 100, 0, 10, -5, 5, 100, 100);
  drawAxis(c, 1, 1, 0xB0B0B0);
  drawScatterChart(c, xs, ys, 0x0000FF);
  saveCanvas(c, "./output/chart_scatter.bmp");
  
  c = createCanvas(1200, 1200, 100, 100, 0, 10, -5, 5, 100, 100);
  drawAxis(c, 1, 1, 0xB0B0B0);
  drawBarChart(c, xs, ys, 0x0000FF);
  saveCanvas(c, "./output/chart_bar.bmp");
  
  return 0;
}
