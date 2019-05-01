
#include <Rcpp.h>
#include <R_ext/GraphicsEngine.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Nice formatting for a pair of coordinates
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string format_coord(double x, double y) {
  return tfm::format("(%6.1f, %6.1f)", x/72, y/72);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Nice formatting for a two pairs of coordinates
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string format_coords(double x0, double y0, double x1, double y1) {
  return tfm::format("(%6.1f, %6.1f) - (%6.1f, %6.1f)", x0/72, y0/72, x1/72, y1/72);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Naively converting a colour to grey level = 0.3R + 0.59G + 0.11B
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double col2grey(int col) {
  return (R_RED(col) * 0.3 + R_GREEN(col) * 0.59 + R_BLUE(col) * 0.11)/255;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert a colour to an ascii character
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int col2ascii(int col) {
  double grey = col2grey(col);
  // int index = (int)(grey * 4);
  // int table[5] = {'#', '$', 'x', '.', ' '};

  int index = (int)(grey * 13);
  int table[14] = {'#', '*',
                   'O',
                   'X', 'x', 'o', 'a',
                   '-', '+', '~',
                   ':', '.', '.', ' '};

  return table[index];
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Output colour as hex
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string format_col(int col) {
  return tfm::format("#%02X%02X%02X%02X (%c)", R_RED(col), R_GREEN(col), R_BLUE(col), R_ALPHA(col), col2ascii(col));
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Dump the graphical config (without font information)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void dump_gc(pGEcontext gc, bool indent = true) {
  if (indent) {
    Rcpp::Rcout << "             ";
  }
  Rcpp::Rcout << " col:"        << format_col(gc->col);
  Rcpp::Rcout << " fill:"       << format_col(gc->fill);
  Rcpp::Rcout << " lwd:"        << tfm::format("%.2f", gc->lwd);
  Rcpp::Rcout << " lty:"        << gc->lty;
  Rcpp::Rcout << " cex:"        << gc->cex;
  Rcpp::Rcout << std::endl;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Dump the graphical config, and font information.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void dump_gc_with_fonts(pGEcontext gc, bool indent = true) {
  if (indent) {
    Rcpp::Rcout << "             ";
  }
  Rcpp::Rcout << " col:"        << format_col(gc->col);
  Rcpp::Rcout << " fill:"       << format_col(gc->fill);
  Rcpp::Rcout << " lwd:"        << tfm::format("%.2f", gc->lwd);
  Rcpp::Rcout << " lty:"        << gc->lty;
  Rcpp::Rcout << " cex:"        << gc->cex;
  Rcpp::Rcout << " fontface:"   << gc->fontface;
  Rcpp::Rcout << " fontsize:"   << gc->cex * gc->ps;
  Rcpp::Rcout << " fontfamily:" << gc->fontfamily;
  Rcpp::Rcout << " lineheight:" << gc->lineheight;
  Rcpp::Rcout << std::endl;
}
