#include <Rcpp.h>
#include <R_ext/GraphicsEngine.h>
#include <fstream>
#include <stdio.h>

#include "ascii-device.h"
#include "utils.h"
#include "ascii-draw.h"



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Called when 'dev.off()' is called
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_close(pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;

  char multifilename[255];
  int cx;

  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- close" << std::endl;
  }

  int *data = context->data;
  unsigned int nrow = context->nrow;
  unsigned int ncol = context->ncol;

  if (context->filename == "console" | context->filename == "") {
    for (unsigned int i = 0; i < nrow; i++) {
      for (unsigned int j = 0; j < ncol; j++) {
        Rcpp::Rcout << (char)data[j*nrow + i];
      }
      Rcpp::Rcout << std::endl;
    }
  } else {
    std::ofstream outfile;

    // Is this a format string for sprintf?
    size_t found = (context->filename).find("%");

    // If we found a % sign, then use the filename as a format string with the
    // page number as the argument
    if (found != std::string::npos) {
      cx = snprintf(multifilename, 255, (context->filename).c_str(), context->page);
      outfile.open(multifilename, std::ios::out);
    } else {
      outfile.open(context->filename, std::ios::out);
    }


    for (unsigned int i = 0; i < nrow; i++) {
      for (unsigned int j = 0; j < ncol; j++) {
        outfile << (char)data[j*nrow + i];
      }
      outfile << std::endl;
    }

    outfile.close();
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Start a new page
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_new_page(const pGEcontext gc, pDevDesc dd) {
  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;

  // Save current page
  if (context->page > 0) {
    ascii_close(dd);
  }

  // Incremenet page number
  context->page = context->page + 1;

  // zero out the canvas
  for (unsigned int row = 0; row < context->nrow; row++) {
    for (unsigned int col = 0; col < context->ncol; col++) {
      context->data[row * context->ncol + col] = ' ';
    }
  }

  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- new page: " << context->page << std::endl;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set clipping box
// @param x0,y0,x1,y1 limits of clipping
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 2) {
    Rcpp::Rcout << "- clip        " << format_coords(x0, y0, x1, y1) << std::endl;
  }

  if(x0 > x1) {
    std::swap(x0, x1);\
  }

  if(y0 > y1) {
    std::swap(y0, y1);\
  }

  context->xlower = floor(x0/72);
  context->xupper = ceil (x1/72);
  context->ylower = floor(y0/72);
  context->yupper = ceil (y1/72);

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return information about the device size
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_size(double *left, double *right, double *bottom, double *top,
              pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 2) {
    Rcpp::Rcout << "- size request" << std::endl;
  }

  *left   = dd->left;
  *right  = dd->right;
  *bottom = dd->bottom;
  *top    = dd->top;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw text
// @param x,y location of text
// @param str string
// @param rot rotation of string
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_text(double x, double y, const char *str, double rot,
              double hadj, const pGEcontext gc, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- text        " << tfm::format("%-6s", tfm::format("'%s'", str)) << "   " << format_coord(x, y) << "  rot: " << rot << " Hadj: " << hadj << "  ";
    dump_gc_with_fonts(gc, false);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Draw boxes instead of actual text
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double xc = x/72;
  double yc = y/72;
  int yoffset = (int)(strlen(str));

  if (rot == 0) {
    set_pixel(xc - 1, yc, ' ', context);
    for (unsigned int i = 0; i <strlen(str); i++) {
      set_pixel(xc + i, yc, (int)str[i], context);
    }
    set_pixel(xc + strlen(str), yc, ' ', context);
  } else {
    set_pixel(xc, yc - yoffset - 1, ' ', context);
    for (unsigned int i = 0; i <strlen(str); i++) {
      set_pixel(xc, yc - yoffset + i, (int)str[i], context);
    }
    set_pixel(xc, yc - yoffset + strlen(str), ' ', context);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Calculate and return the string width in the current context
// @param str string
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double ascii_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 2) {
    Rcpp::Rcout << "- strwidth    " << tfm::format("%-6s", tfm::format("'%s'", str)) << "  ";
    dump_gc_with_fonts(gc, false);
  }

  return (strlen(str) + 1) * 72; // * gc->cex * gc->ps * 72;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a single rectangle
// @param x0,y0,x1,y1 limits of rectangle
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_rect(double x0, double y0, double x1, double y1,
                  const pGEcontext gc, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- rect        " << format_coords(x0, y0, x1, y1) << "  ";
    dump_gc(gc, false);
  }

  if (R_ALPHA(gc->fill) > 0) {
    fill_rect(x0/72, y0/72, x1/72, y1/72, col2ascii(gc->fill), context);
  }
  if (R_ALPHA(gc->col) > 0) {
    draw_rect(x0/72, y0/72, x1/72, y1/72, col2ascii(gc->col), context);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a single circle
// @param x,y coords of centre
// @param r radius
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_circle(double x, double y, double r, const pGEcontext gc, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- circle      " << format_coord(x, y) << "  radius: " << tfm::format("%.2f", r) << "  ";
    dump_gc(gc, false);
  }

  unsigned int x0 = x/72;
  unsigned int y0 = y/72;

  int col = col2ascii(gc->col);

  // In general I've found that halving the radius improves the render
  r = r/2;

  // This is a monsterously naff filled circle implementation.
  for (int xi = (int)(x0 - r); xi <= x0 + r; xi++) {
    for (int yi = (int)(y0 - r); yi <= y0 + r; yi++) {
      if ((xi - x0) * (xi - x0) + 2 * (yi - y0) * (yi - y0) <= r) { // Sneaky factor of 2 to try and get aspect ratio right
        set_pixel(xi, yi, col, context);
      }
    }
  }


  // Ensure at least one point gets drawn for a circle
  set_pixel(x0, y0, col, context);

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a polygon
// @param n number of points in polygon
// @param x,y n sets of coordinates
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_polygon(int n, double *x, double *y, const pGEcontext gc, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- polygon     " << format_coord(x[0], y[0]) << "  ";
    dump_gc(gc, true);

    for (unsigned int i = 1; i < n; i++) {
      Rcpp::Rcout << "              " << format_coord(x[i], y[i]) << std::endl;
    }
  }


  for (unsigned int i = 1; i < n; i++) {
    draw_line(x[i-1]/72, y[i-1]/72, x[i]/72, y[i]/72, col2ascii(gc->col), context);
  }

  // Close the polygon
  draw_line(x[n-1]/72, y[n-1]/72, x[0]/72, y[0]/72, col2ascii(gc->col), context);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a polyline
// @param n number of points in polyline
// @param x,y n sets of coordinates
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_polyline(int n, double *x, double *y, const pGEcontext gc, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- polyline    " << format_coord(x[0], y[0]) << "  ";
    dump_gc(gc, false);

    for (unsigned int i = 1; i < n; i++) {
      Rcpp::Rcout << "              " << format_coord(x[i], y[i]) << std::endl;
    }
  }

  for (unsigned int i = 1; i < n; i++) {
    draw_line(x[i-1]/72, y[i-1]/72, x[i]/72, y[i]/72, col2ascii(gc->col), context);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a path.
//
// @param x,y vectors of coordiates - enough to cover all the polygons each
//            with its respective nper
// @param npoly Number of polygons = number of elements in "nper"
// @param nper Number of elements in each particular polygon
// @param winding winding order. whehter to fill or not fill.  currently unused in ascii()
//        According to svglite::svg_path():
//             0 = false = evenodd
//             1 = true  = nonzero
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_path(double *x, double *y, int npoly, int *nper,
              Rboolean winding, const pGEcontext gc, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- path      winding: " << winding << "  npoly: " << npoly << "  nper: " << *nper;
    dump_gc(gc, true);

    if (context->verbosity >= 3) {
      unsigned int ind = 0;
      for (unsigned int i = 0; i < npoly; i++) {
        // Move to the first point of the sub-path
        Rcpp::Rcout << "poly: " << i << "     " << format_coord(x[ind], y[ind]) << std::endl;
        ind++;
        // Draw the sub-path
        for (unsigned int j = 1; j < (unsigned int)(nper[i]); j++) {
          Rcpp::Rcout << "            " << format_coord(x[ind], y[ind]) << std::endl;
          ind++;
        }
      }
    }
  }

  int col = col2ascii(gc->col);

  unsigned int ind = 0;
  for (unsigned int i = 0; i < npoly; i++) {
    // Move to the first point of the sub-path
    set_pixel(x[ind]/72, y[ind]/72, col, context);
    ind++;
    // Draw the sub-path
    for (unsigned int j = 1; j < (unsigned int)(nper[i]); j++) {
      set_pixel(x[ind]/72, y[ind]/72, col, context);
      ind++;
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return information about the current font settings?
//
// @param c single character. Could be unicode
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_metric_info(int c, const pGEcontext gc, double* ascent,
                     double* descent, double* width, pDevDesc dd) {

  // Code stolen from svglite
  bool is_unicode = mbcslocale;
  if (c < 0) {
    is_unicode = true;
    c = -c;
  }

  // Code stolen from svglite
  // Convert to string - negative implies unicode code point
  char str[16];
  if (is_unicode) {
    Rf_ucstoutf8(str, (unsigned int) c);
  } else {
    str[0] = (char) c;
    str[1] = '\0';
  }

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 2) {
    Rcpp::Rcout << "- metric_info '" << str << "'" << std::endl;
  }

  // text padding
  *ascent  = 0.5 * 72;
  *descent = 0.5 * 72;
  *width   = 1.5 * 72;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Add a raster (image)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_raster(unsigned int *raster, int w, int h,
                   double x, double y, double width, double height,
                   double rot, Rboolean interpolate,
                   const pGEcontext gc, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- raster" << std::endl;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a single line
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void ascii_line(double x1, double y1, double x2, double y2,
              const pGEcontext gc, pDevDesc dd) {

  ascii_struct *context = (ascii_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- line        " << format_coords(x1, y2, x2, y2) << "  ";
    dump_gc(gc, false);
  }

  draw_line(x1/72, y1/72, x2/72, y2/72, col2ascii(gc->col), context);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Create and initialise the device description
//
// - Create a DevDesc (Device Description structure)
// - Populate it with functions to call for each of the primitive elements
//   that need to be created
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pDevDesc ascii_device_new(std::string filename, Rcpp::IntegerMatrix &mat, int verbosity) {

  double pointsize = 1;

  pDevDesc dd = (DevDesc*) calloc(1, sizeof(DevDesc));
  if (dd == NULL)
    return dd;

  double width  = mat.ncol();
  double height = mat.nrow();

  if (width < 10 | height < 10) {
    Rcpp::stop("size must be at least 10x10");
  }

  dd->startfill  = R_RGB(0, 0, 0);
  dd->startcol   = R_RGB(0, 0, 0);
  dd->startps    = pointsize;
  dd->startlty   = 0;
  dd->startfont  = 1;
  dd->startgamma = 1;


  // Callbacks
  dd->activate   = NULL; // null
  dd->deactivate = NULL; // null
  dd->close      = ascii_close;
  dd->clip       = ascii_clip;
  dd->size       = ascii_size;
  dd->newPage    = ascii_new_page;
  dd->line       = ascii_line;
  dd->text       = ascii_text;
  dd->strWidth   = ascii_strwidth;
  dd->rect       = ascii_rect;
  dd->circle     = ascii_circle;
  dd->polygon    = ascii_polygon;
  dd->polyline   = ascii_polyline;
  dd->path       = ascii_path;
  dd->mode       = NULL; // null
  dd->metricInfo = ascii_metric_info;
  dd->cap        = NULL; // null
  dd->raster     = ascii_raster;

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8    = (Rboolean) 1;
  dd->textUTF8       = ascii_text;
  dd->strWidthUTF8   = ascii_strwidth;

  // Screen Dimensions in pts
  dd->left   = 0;
  dd->top    = 0;
  dd->right  = width  * 72;
  dd->bottom = height * 72;

  // Magic constants copied from other graphics devices
  // nominal character sizes in pts
  dd->cra[0] = 0.9 * pointsize;
  dd->cra[1] = 1.2 * pointsize;

  // character alignment offsets
  dd->xCharOffset = 0.4900;
  dd->yCharOffset = 0.3333;
  dd->yLineBias   = 0.2;

  // inches per pt
  dd->ipr[0] = 1.0 / 72.0;
  dd->ipr[1] = 1.0 / 36.0;

  // Capabilities
  dd->canClip           = TRUE;
  dd->canHAdj           = 0;
  dd->canChangeGamma    = FALSE;
  dd->displayListOn     = FALSE;
  dd->haveTransparency  = 2;
  dd->haveTransparentBg = 2;

  //--------------------------------------------------------------------------
  // Create device-specific data structure to store state/context info
  //--------------------------------------------------------------------------
  ascii_struct *context;
  context = (ascii_struct *)calloc(1, sizeof(ascii_struct));
  context->filename         = filename;
  context->data             = mat.begin();
  context->nrow             = mat.nrow();
  context->ncol             = mat.ncol();
  context->verbosity        = verbosity;
  context->xlower           = 0;
  context->xupper           = width;
  context->ylower           = 0;
  context->yupper           = height;
  context->page             = 0;

  dd->deviceSpecific = context;

  return dd;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialise a device descriptor and tell R about it
//
// This function is mostly cloned from svglite/src/devSVG.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void make_ascii_device(std::string filename, Rcpp::IntegerMatrix mat, int verbosity) {

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = ascii_device_new(filename, mat, verbosity);
    if (dev == NULL)
      Rcpp::stop("Failed to start 'ascii' device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "ascii");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//' Create an ascii graphics device
//'
//' @param filename if given, write ascii to this file, otherwise write to console.
//'        If file is a format string e.g. "output%03i.txt", then this will be used
//'        to create a unique filename if multiple pages are output within the same
//'        \code{ascii()} call.
//' @param width,height dimensions of text output (in characters)
//' @param verbosity level. default: 0
//'
// [[Rcpp::export]]
Rcpp::IntegerMatrix ascii_(std::string filename, unsigned int width, unsigned int height,
                           int verbosity = 0) {


  Rcpp::IntegerMatrix mat(height, width);
  std::fill(mat.begin(), mat.end(), ' ');

  make_ascii_device(filename, mat, verbosity);

  return mat;
}




