#include <Rcpp.h>
#include <R_ext/GraphicsEngine.h>

#include "utils.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Struct of information about the drawing context
// Other info to add
//   - current clipping region
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct context_struct {
  int verbosity;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Called when 'dev.off()' is called
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_close(pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- close" << std::endl;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set clipping box
// @param x0,y0,x1,y1 limits of clipping
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 2) {
    Rcpp::Rcout << "- clip        " << format_coords(x0, y0, x1, y1) << std::endl;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return information about the device size
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_size(double *left, double *right, double *bottom, double *top,
              pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 2) {
    Rcpp::Rcout << "- size request" << std::endl;
  }

  *left   = dd->left;
  *right  = dd->right;
  *bottom = dd->bottom;
  *top    = dd->top;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Start a new page
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_new_page(const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- new page" << std::endl;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw text
// @param x,y location of text
// @param str string
// @param rot rotation of string
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_text(double x, double y, const char *str, double rot,
              double hadj, const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- text        " << tfm::format("%-6s", tfm::format("'%s'", str)) << "   " << format_coord(x, y) << "  rot: " << rot << " Hadj: " << hadj << "  ";
    dump_gc_with_fonts(gc, false);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Calculate and return the string width in the current context
// @param str string
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double descriptive_strwidth(const char *str, const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 2) {
    Rcpp::Rcout << "- strwidth    " << tfm::format("%-6s", tfm::format("'%s'", str)) << "  ";
    dump_gc_with_fonts(gc, false);
  }

  return strlen(str) * gc->cex * gc->ps * 72;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a single rectangle
// @param x0,y0,x1,y1 limits of rectangle
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_rect(double x0, double y0, double x1, double y1,
                  const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- rect        " << format_coords(x0, y0, x1, y1) << "  ";
    dump_gc(gc, false);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a single circle
// @param x,y coords of centre
// @param r radius
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_circle(double x, double y, double r, const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- circle      " << format_coord(x, y) << "  radius: " << tfm::format("%.2f", r) << "  ";
    dump_gc(gc, false);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a polygon
// @param n number of points in polygon
// @param x,y n sets of coordinates
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_polygon(int n, double *x, double *y, const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- polygon" << std::endl;
    dump_gc(gc, true);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a polyline
// @param n number of points in polyline
// @param x,y n sets of coordinates
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_polyline(int n, double *x, double *y, const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- polyline    " << format_coord(x[0], y[0]) << "  ";
    dump_gc(gc, false);

    for (unsigned int i = 1; i < n; i++) {
      Rcpp::Rcout << "              " << format_coord(x[i], y[i]) << std::endl;
    }
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
void descriptive_path(double *x, double *y, int npoly, int *nper,
              Rboolean winding, const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
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
        // Close the sub-path
      }
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return information about the current font settings?
//
// @param c single character. Could be unicode
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_metric_info(int c, const pGEcontext gc, double* ascent,
                     double* descent, double* width, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;

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

  if (context->verbosity >= 2) {
    Rcpp::Rcout << "- metric_info '" << str << "'" << std::endl;
  }

  // Making up some sizes as a placeholder
  *ascent  = gc->cex * gc->ps * 72;
  *descent = gc->cex * gc->ps * 72;
  *width   = gc->cex * gc->ps * 72;

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Add a raster (image)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_raster(unsigned int *raster, int w, int h,
                   double x, double y, double width, double height,
                   double rot, Rboolean interpolate,
                   const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- raster" << std::endl;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a single line
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void descriptive_line(double x1, double y1, double x2, double y2,
              const pGEcontext gc, pDevDesc dd) {

  context_struct *context = (context_struct *)dd->deviceSpecific;
  if (context->verbosity >= 1) {
    Rcpp::Rcout << "- line        " << format_coords(x1, y2, x2, y2) << "  ";
    dump_gc(gc, false);
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Create and initialise the device description
//
// - Create a DevDesc (Device Description structure)
// - Populate it with functions to call for each of the primitive elements
//   that need to be created
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pDevDesc descriptive_device_new(int width,
                           int height,
                           int verbosity) {

  double pointsize = 12;

  pDevDesc dd = (DevDesc*) calloc(1, sizeof(DevDesc));
  if (dd == NULL)
    return dd;


  dd->startfill  = R_RGB(0, 0, 0);
  dd->startcol   = R_RGB(0, 0, 0);
  dd->startps    = pointsize;
  dd->startlty   = 0;
  dd->startfont  = 1;
  dd->startgamma = 1;


  // Callbacks
  dd->activate   = NULL; // null
  dd->deactivate = NULL; // null
  dd->close      = descriptive_close;
  dd->clip       = descriptive_clip;
  dd->size       = descriptive_size;
  dd->newPage    = descriptive_new_page;
  dd->line       = descriptive_line;
  dd->text       = descriptive_text;
  dd->strWidth   = descriptive_strwidth;
  dd->rect       = descriptive_rect;
  dd->circle     = descriptive_circle;
  dd->polygon    = descriptive_polygon;
  dd->polyline   = descriptive_polyline;
  dd->path       = descriptive_path;
  dd->mode       = NULL; // null
  dd->metricInfo = descriptive_metric_info;
  dd->cap        = NULL; // null
  dd->raster     = descriptive_raster;

  // UTF-8 support
  dd->wantSymbolUTF8 = (Rboolean) 1;
  dd->hasTextUTF8    = (Rboolean) 1;
  dd->textUTF8       = descriptive_text;
  dd->strWidthUTF8   = descriptive_strwidth;

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
  dd->ipr[1] = 1.0 / 72.0;

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
  context_struct *context;
  context = (context_struct *)calloc(1, sizeof(context_struct));
  context->verbosity = verbosity;

  dd->deviceSpecific = context;

  return dd;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialise a device descriptor and tell R about it
//
// This function is mostly cloned from svglite/src/devSVG.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void make_descriptive_device(int width,
                        int height,
                        int verbosity) {

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = descriptive_device_new(width, height, verbosity);
    if (dev == NULL)
      Rcpp::stop("Failed to start 'descriptive' device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "descriptive");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//' Create a descriptive graphics device
//'
//' @param verbosity level. default: 1
//' @param width,height nominal size of drawing area in pixels.
//'
// [[Rcpp::export]]
bool descriptive_(int verbosity = 1, int width  = 400, int height = 300) {
  make_descriptive_device(width, height, verbosity);
  return true;
}




