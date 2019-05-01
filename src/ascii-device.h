

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Struct of information about the drawing context
// Other info to add e.g.
//   - current clipping region
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct ascii_struct {
  std::string filename;
  int *data;
  unsigned int nrow;
  unsigned int ncol;
  double xlower; // Clipping
  double xupper;
  double ylower;
  double yupper;
  int verbosity;
};
