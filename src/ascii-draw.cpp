#include <Rcpp.h>

#include "ascii-device.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set an individual pixel in the image
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void set_pixel(unsigned int x, unsigned int y, int colour, ascii_struct *context) {

  // Keep the coordinates within range
  if (x > context->ncol - 1) { x = context->ncol - 1; }
  if (y > context->nrow - 1) { y = context->nrow - 1; }

  int *mat = context->data;

  if (x >= context->xlower && x <= context->xupper && y >= context->ylower && y <= context->yupper) {
    mat[x * context->nrow + y] = colour;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Bresenham's line algorithm
// No antialiasing!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_line(float x1,  float y1,  float x2,  float y2,  int colour, ascii_struct *context) {

  // Ensure end points get plotted
  set_pixel(x1, y1, colour, context);
  set_pixel(x2, y2, colour, context);

  const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));

  if(steep) {
    std::swap(x1, y1);
    std::swap(x2, y2);
  }

  if(x1 > x2) {
    std::swap(x1, x2);
    std::swap(y1, y2);
  }

  const float dx = x2 - x1;
  const float dy = fabs(y2 - y1);

  float error = dx / 2.0f;
  const int ystep = (y1 < y2) ? 1 : -1;
  unsigned int y = (unsigned int)y1;

  const int maxX = (int)x2;

  for(unsigned int x=(unsigned int)x1; x < maxX; x++) {
    if(steep) {
      set_pixel(y,x, colour, context);
    } else {
      set_pixel(x,y, colour, context);
    }

    error -= dy;
    if(error < 0) {
        y += ystep;
        error += dx;
    }
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw rectangle outline
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void draw_rect(float x0,  float y0,  float x1,  float y1,  int colour, ascii_struct *context) {

  if (colour == ' ') {
    return;
  }

  draw_line(x0, y0, x1, y0, '-', context);
  draw_line(x1, y0, x1, y1, '|', context);
  draw_line(x1, y1, x0, y1, '-', context);
  draw_line(x0, y1, x0, y0, '|', context);

  set_pixel(x0, y0, '+', context);
  set_pixel(x0, y1, '+', context);
  set_pixel(x1, y1, '+', context);
  set_pixel(x1, y0, '+', context);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Filled rectangle of the given colour
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fill_rect(float x0,  float y0,  float x1,  float y1,  int colour, ascii_struct *context) {


  if(x0 > x1) { std::swap(x0, x1); }
  if(y0 > y1) { std::swap(y0, y1); }

  for (unsigned int y = (unsigned int)y0; y < y1; y++) {
    for (unsigned int x = (unsigned int)x0; x < x1; x++) {
      set_pixel(x,y, colour, context);
    }
  }

  set_pixel(x0, y0, colour, context);
}



