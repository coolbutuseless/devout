
std::string format_coord(double x, double y);
std::string format_coords(double x0, double y0, double x1, double y1);
std::string format_col(int col);
void dump_gc(pGEcontext gc, bool indent);
void dump_gc_with_fonts(pGEcontext gc, bool indent = true);
double col2grey(int col);
int col2ascii(int col);
