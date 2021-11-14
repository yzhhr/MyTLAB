# MyTLAB
A big hw of Basic Programming in Year 1. I am assigned with Part 2. Part 2 is completed. The whole thing is not.

## Additional Features

1. Antialiasing is implemented when drawing numbers and drawing lines.
2. Bar charts support negative heights. In that case the bar is going downwards from zero.

## Something random

IDK why they let us do the three parts separately. I have to complete some of others' job to make my code run.

However, I copied code from the Internet(details below) to make up some of Part 3. I hope that won't win me a penalty.

The three parts are totally NOT equal. My part is the most troublesome.

## Examples

![](./chart_bar.bmp) ![](./chart_line.bmp) ![](./chart_scatter.bmp)

## How to use

1. Clone this repo. `BMPFILE.h, cbmp.c/h, main.c, matrix.c/h, painter.c/h, ref24.bmp` are core files.
2. `gcc main.c -o main`
3. `./main`
4. Then you get the three images`chart_bar.bmp, chart_line.bmp, chart_scatter.bmp`.

## Opensource projects involved

https://github.com/dnqrNOT/BMPFile

https://github.com/mattflow/cbmp

The CBMP library has a bug in it. The `_get_int_from_buffer` often accesses invalid memory addresses. The bug is fixed in this project.

## ~~Bugs~~ Features & Todo

1. Not well tested. Never tried on a rectangluar setup where the two sides are of different lengths and ratios of scaling.
2. IDK how to include things properly. So I `#include "xxx.c"` instead of `#include "xxx.h"`.
3. I mix the colors. So new paints won't cover the old. They **mix** with the old.
4. Two unknown fellows are going to help me complete the whole job. This is not the complete version of the whole thing.


