#include "painter.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include "matrix.h"
#include "lib/cbmp.c"

#ifdef DEBUG
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define eprintf(...) do {} while(0)
#endif

#define LINE_WIDTH 2
#define SPIKE_LENGTH 20 

char* getCol(Canvas *c, int x, int y) {
  return c->map + (y * c->width + x) * 3;
}
#include "lib/BMPFile.h"
void saveCanvas(Canvas * c, const char * filename) {
  createImage(filename, c->width, c->height, c->map);
}
double dist(double x1, double y1, double x2, double y2) {
  return hypot(x1 - x2, y1 - y2);
}
unsigned char fade(unsigned char orig, double rate) {
  // towards white
  return 0xFF ^ (unsigned char)((0xFF ^ orig) * rate);
}

void mixColor(unsigned char * orig, unsigned char col) {
  int fin = (int)(0xFF ^ *orig) + (0xFF ^ col);
  if (fin > 0xFF) fin = 0xFF;
  *orig = 0xFF ^ fin;
}
void setColorRGB(Canvas *c, int px, int py, unsigned char R, unsigned char G, unsigned char B, double faderate) {
  if (faderate < 1) {
    // eprintf("orig = %d %d %d, rate = %.3lf\n", (int)R, (int)G, (int)B, faderate);
    R = 0xFF ^ (unsigned char)((0xFF ^ R) * faderate);
    G = 0xFF ^ (unsigned char)((0xFF ^ G) * faderate);
    B = 0xFF ^ (unsigned char)((0xFF ^ B) * faderate);
    // eprintf("newo = %d %d %d\n", (int)R, (int)G, (int)B);
  }
  // Caution: BMP prefers BGR in that order
  mixColor(getCol(c, px, py)    , B);
  mixColor(getCol(c, px, py) + 1, G);
  mixColor(getCol(c, px, py) + 2, R);
}
void setColor(Canvas *c, int px, int py, int col, double faderate) {
  setColorRGB(c, px, py, col >> 16 & 0xFF, col >> 8 & 0xFF, col & 0xFF, faderate);
}


// faderate = 1 means do nothing
// faderate = 0 means pure white

// in grid coordinates
void drawLineProto(Canvas * c, double sx, double sy, double tx, double ty, int col, double dist1, double dist2, int restricted) {
  int endX = c->startX + (c->maxX - c->minX) * c->ratioX;
  int endY = c->startY + (c->maxY - c->minY) * c->ratioY;

  for (int px = 0; px < c->width; ++px) {
    for (int py = 0; py < c->height; ++py) {
      // dist from (x, y) to the given line
      if (restricted && !(c->startX <= px && px < endX && c->startY <= py && py < endY)) continue;
      if (!(fmin(sx, tx) - dist1 <= px && px <= fmax(sx, tx) + dist1 && fmin(sy, ty) - dist1 <= py && py <= fmax(sy, ty) + dist1)) continue;
      // this makes the two ends square and strange ... or it appears pretty for certain people?

      double len = dist(sx, sy, tx, ty);
      double ls = dist(sx, sy, px, py);
      double lt = dist(tx, ty, px, py);
      double p = (len + ls + lt) / 2;
      double S = sqrt(p * (p - len) * (p - ls) * (p - lt));
      double h = 2 * S / len;
      if (isnan(S)) h = 0; // sometimes errors happen, making my lines hollow
      
      if (h < dist1 + 0.01) {
        setColor(c, px, py, col, 1);
      } else if (h < dist2) {
        double faderate = (h - dist2) / (dist1 - dist2);
        // eprintf("%lf %lf %lf %lf\n", h, dist1, dist2, faderate);
        setColor(c, px, py, col, faderate);
      }
    }
  }
}

// in cartesian coordinates
void drawDataLineProto(Canvas * c, double sx, double sy, double tx, double ty, int col, double dist1, double dist2) {
  sx = (sx - c->minX) * c->ratioX + c->startX;
  tx = (tx - c->minX) * c->ratioX + c->startX;
  sy = (sy - c->minY) * c->ratioY + c->startY;
  ty = (ty - c->minY) * c->ratioY + c->startY;
  drawLineProto(c, sx, sy, tx, ty, col, dist1, dist2, 1);
  // eprintf("drawing %lf %lf %lf %lf in col %d %lf %lf\n", sx, sy, tx, ty, col, dist1, dist2);
}
void drawDataLine(Canvas * c, double sx, double sy, double tx, double ty, int col, double dist) {
  drawDataLineProto(c, sx, sy, tx, ty, col, dist, dist);
}
void drawDataLineSmooth(Canvas * c, double sx, double sy, double tx, double ty, int col, double dist) {
  // antialiasing
  drawDataLineProto(c, sx, sy, tx, ty, col, dist * 0.7, dist * 1.5);
}
// in cartesian coordinates
void drawRect(Canvas * c, double sx, double sy, double tx, double ty, int col) {
  // coordinates on the grid
  int endX = c->startX + (c->maxX - c->minX) * c->ratioX;
  int endY = c->startY + (c->maxY - c->minY) * c->ratioY;
  // sometimes the bar goes downwards from zero
  if (sx > tx) {
    double tmp = sx; sx = tx; tx = tmp;
  }
  if (sy > ty) {
    double tmp = sy; sy = ty; ty = tmp;
  }

  // borders
  drawDataLine(c, sx, sy, sx, ty, 0x0000, LINE_WIDTH);
  drawDataLine(c, sx, sy, tx, sy, 0x0000, LINE_WIDTH);
  drawDataLine(c, sx, ty, tx, ty, 0x0000, LINE_WIDTH);
  drawDataLine(c, tx, sy, tx, ty, 0x0000, LINE_WIDTH);

  // the inside
  sx = (sx - c->minX) * c->ratioX + c->startX;
  tx = (tx - c->minX) * c->ratioX + c->startX;
  sy = (sy - c->minY) * c->ratioY + c->startY;
  ty = (ty - c->minY) * c->ratioY + c->startY;
  for (int px = 0; px < c->width; ++px) {
    for (int py = 0; py < c->height; ++py) {
      if (!(c->startX <= px && px < endX && c->startY <= py && py < endY)) continue;
      if (sx <= px && px <= tx && sy <= py && py <= ty)
        setColor(c, px, py, col, 1);
    }
  }
}

// in grid coordinates
void drawGridLine(Canvas * c, double sx, double sy, double tx, double ty, int col, double dist) {
  drawLineProto(c, sx, sy, tx, ty, col, dist, dist, 0);
}

Canvas * createCanvas(
  int height, int width, int startX, int startY,
  int minX, int maxX, int minY, int maxY,
  double ratioX, double ratioY) {
    Canvas * c = malloc(sizeof(Canvas));
    c->height = height;
    c->width = width;
    c->startX = startX;
    c->startY = startY;
    c->minX = minX;
    c->maxX = maxX;
    c->minY = minY;
    c->maxY = maxY;
    c->ratioX = ratioX;
    c->ratioY = ratioY;
    c->map = malloc(sizeof(unsigned char) * 3 * height * width);
    // pure white background
    memset(c->map, 0xFF, sizeof(unsigned char) * 3 * height * width);
    
    // borderlines
    double sx = c->startX, tx = sx + (c->maxX - c->minX) * c->ratioX;
    double sy = c->startX, ty = sy + (c->maxY - c->minY) * c->ratioY;
    drawGridLine(c, sx, sy, tx, sy, 0x000000, LINE_WIDTH);
    drawGridLine(c, sx, sy, sx, ty, 0x000000, LINE_WIDTH);
    drawGridLine(c, sx, ty, tx, ty, 0x000000, LINE_WIDTH);
    drawGridLine(c, tx, sy, tx, ty, 0x000000, LINE_WIDTH);
    return c;
  }
void applyNumber(Canvas*, int, int, int, int, int);
void drawAxis(Canvas * c, int gridX, int gridY, int color) {
  // draw spikes & gridlines
  for (int x = (c->minX / gridX - 2) * gridX; x <= c->maxX; x += gridX) if (c->minX <= x) {
    drawDataLine(c, x, c->minY, x, c->minY + SPIKE_LENGTH / c->ratioY, 0x00000, LINE_WIDTH);
    drawDataLine(c, x, c->maxY, x, c->maxY - SPIKE_LENGTH / c->ratioY, 0x00000, LINE_WIDTH);
    drawDataLine(c, x, c->minY, x, c->maxY, color, LINE_WIDTH);
    applyNumber(c, (x - c->minX) * c->ratioX + c->startX, c->startY / 2, gridX * c->ratioX * 0.8, c->startY * 0.8, x);
  }
  for (int y = (c->minY / gridY - 2) * gridY; y <= c->maxY; y += gridY) if (c->minY <= y) {
    drawDataLine(c, c->minX, y, c->minX + SPIKE_LENGTH / c->ratioX, y, 0x00000, LINE_WIDTH);
    drawDataLine(c, c->maxX, y, c->maxX - SPIKE_LENGTH / c->ratioX, y, 0x00000, LINE_WIDTH);
    drawDataLine(c, c->minX, y, c->maxX, y, color, LINE_WIDTH);
    applyNumber(c, c->startX / 2, (y - c->minY) * c->ratioY + c->startY, c->startX * 0.8, gridY * c->ratioY * 0.8, y);
  }
}
void drawLineChart(Canvas * c, Matrix * x, Matrix * y, int color) {
  assert(x->row == 1 && y->row == 1);
  assert(x->col == y->col);
  for (int i = 0; i + 1 < x->col; ++i) {
    drawDataLineSmooth(c, x->data[0][i], y->data[0][i], x->data[0][i + 1], y->data[0][i + 1], color, LINE_WIDTH);
  }
}
void drawScatterChart(Canvas * c, Matrix * x, Matrix * y, int color) {
  assert(x->row == 1 && y->row == 1);
  assert(x->col == y->col);
  for (int i = 0; i < x->col; ++i) {
    double px = x->data[0][i], py = y->data[0][i];
    double dx = SPIKE_LENGTH / c->ratioX;
    double dy = SPIKE_LENGTH / c->ratioY;
    // rectangular
    drawDataLineSmooth(c, px - dx, py, px + dx, py, color, LINE_WIDTH);
    drawDataLineSmooth(c, px, py - dy, px, py + dy, color, LINE_WIDTH);
    // diagonal
    drawDataLineSmooth(c, px - dx * 0.7, py - dy * 0.7, px + dx * 0.7, py + dy * 0.7, color, LINE_WIDTH);
    drawDataLineSmooth(c, px - dx * 0.7, py + dy * 0.7, px + dx * 0.7, py - dy * 0.7, color, LINE_WIDTH);
  }
}

double roughlysame(double x, double y) {
  return fabs(x - y) < 1e-6;
}
void drawBarChart(Canvas * c, Matrix * x, Matrix * y, int color) {
  assert(x->row == 1 && y->row == 1);
  assert(x->col == y->col);
  int n = x->col;
  assert(n >= 2); // i just like it huh
  // ensuring the x's are evenly spaced
  double apart = round(x->data[0][1]) - round(x->data[0][0]);
  assert(!roughlysame(apart, 0));
  for (int i = 0; i + 1 < n; ++i) assert(roughlysame(round(x->data[0][i + 1]) - round(x->data[0][i]), apart));
  for (int i = 0; i < n; ++i) {
    double px = x->data[0][i], py = y->data[0][i];
    drawRect(c, px - apart * 0.4, 0, px + apart * 0.4, py, color); // 80% width as instructed
  }
}


void applyNumber(Canvas *c, int px, int py, int ew, int eh, int value) {
  BMP* bmp = bopen("./ref24.bmp");
  
  int width = get_width(bmp);
  int height = get_height(bmp);

  const int SIGMA = 12 + 2;
  int begin[SIGMA], end[SIGMA];
  // seq 1010101
  // seek for the zero segments
  // [begin, end)
  int stage = 0;
  for (int x = 0; x < width; ++x) {
    unsigned char r, g, b;
    get_pixel_rgb(bmp, x, 1, &r, &g, &b);
    if (r == 0xFF && g == 0xFF && b == 0xFF) {
      if (stage % 2 == 1) {
        ++stage;
        begin[stage / 2] = x;
      }
    } else {
      if (stage % 2 == 0) {
        end[stage / 2] = x;
        ++stage;
      }
    }
  }
  for (int i = 1; i < SIGMA - 1; ++i) eprintf("[%d, %d] ", begin[i], end[i]);
  

  int s[100];
  int len = 0, neg = 0;
  if (value < 0) value = -value, neg = 1;
  do {
    s[len++] = value % 10;
    value /= 10;
  } while (value);
  for (int i = 0, j = len - 1; i < j; ++i, --j) {
    int t = s[i]; s[i] = s[j]; s[j] = t;
  }
  //.-+0123456789
  for (int i = 0; i < len; ++i) s[i] += 4;
  if (neg) {
    for (int i = len; i >= 1; --i) s[i] = s[i - 1];
    s[0] = 2;
    ++len;
  }
  eprintf("s:");
  for (int i = 0; i < len; ++i) eprintf(" %d", s[i]);
  eprintf("\n");

  int aw = 0;
  int ah = height;
  for (int i = 0; i < len; ++i) aw += end[s[i]] - begin[s[i]];
  double scale = fmin((double)ew / aw, (double)eh / ah);
  int sx = px - scale * aw / 2;
  int sy = py - scale * ah / 2;
  eprintf("ew, eh: %d %d, aw, ah: %d %d, sx, sy: %d %d\n", ew, eh, aw, ah, sx, sy);
  eprintf("scale = %lf\n", scale);

  int i = 0, offsetx = 0;
  for (int tarx = sx; tarx <= sx + aw * scale; ++tarx) if (0 <= tarx && tarx < c->width) {
    while ((tarx - sx) / scale - offsetx > end[s[i]] - begin[s[i]]) {
      eprintf("tar = %d, convert = %d\n", tar, (int)((tarx - sx) / scale - offsetx));
      offsetx += end[s[i]] - begin[s[i]], ++i;
    }
    for (int tary = sy; tary <= sy + ah * scale; ++tary) if (0 <= tary && tary < c->height) {
      int x = (tarx - sx) / scale - offsetx + begin[s[i]];
      int y = (tary - sy) / scale;
      if (!(5 < y && y < height - 5)) continue;
      unsigned char r, g, b;
      const int dx[] = {0, -1, -1, -1, 0, 1, 1, 1, 0};
      const int dy[] = {0, -1, 0, 1, 1, 1, 0, -1, -1};
      const int wt[] = {10, 1, 2, 1, 2, 1, 2, 1, 2};
      // antialiasing
      int num = 0;
      for (int j = 0; j < 9; ++j) {
        get_pixel_rgb(bmp, x + dx[j], y + dy[j], &r, &g, &b);
        num += wt[j] * ((int)r + g + b);
      }
      r = g = b = num / (22 * 3);
      if (5 < y && y < height - 5) setColor(c, tarx, tary, r << 16 | g << 8 | b, 1);
      // CAUTION: danger
    }
  }
  return;
  /*
  // int offsetx = 0;
  for (int i = 0; i < len; ++i) {
    // lets fill the characters
    eprintf("digit %d in [%d, %d)\n", s[i], begin[s[i]], end[s[i]]);
    for (int x = begin[s[i]]; x < end[s[i]]; ++x) {
      for (int y = 0; y < height; ++y) {
        unsigned char r, g, b;
        get_pixel_rgb(bmp, x, y, &r, &g, &b);
        int tarx = sx + (x - begin[s[i]] + offsetx) * scale;
        int tary = sy + y * scale;
        if (x == begin[s[i]] && y == 0) eprintf("%dth gets %d %d\n", i, tarx, tary);
        if (0 <= tarx && tarx < c->width && 0 <= tary && tary < c->height) {
          setColor(c, tarx, tary, r << 16 | g << 8 | b, 1);
        }
      }
    }
    offsetx += end[s[i]] - begin[s[i]];
  }
  */
}

#undef eprintf

