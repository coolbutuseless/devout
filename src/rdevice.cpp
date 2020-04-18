#include <Rcpp.h>
#include <R_ext/GraphicsEngine.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert a colour to RGBA
//
// @param col integer colour 32 bits
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Rcpp::IntegerVector col_to_rgba(int col) {
  Rcpp::IntegerVector rgb(4);

  rgb[0] = R_RED  (col);
  rgb[1] = R_GREEN(col);
  rgb[2] = R_BLUE (col);
  rgb[3] = R_ALPHA(col);

  return rgb;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// A structure containing graphical parameters
//
// See: /Library/Frameworks/R.framework/Versions/3.5/Resources/include/R_ext/GraphicsEngine.h
//
// This is how graphical parameters are passed from graphics systems
// to the graphics engine AND from the graphics engine to graphics
// devices.
//
// Devices are not *required* to honour graphical parameters
// (e.g., alpha transparency is going to be tough for some)
//
// typedef enum {
//   GE_ROUND_CAP  = 1,
//   GE_BUTT_CAP   = 2,
//   GE_SQUARE_CAP = 3
// } R_GE_lineend;
//
// typedef enum {
//   GE_ROUND_JOIN = 1,
//   GE_MITRE_JOIN = 2,
//   GE_BEVEL_JOIN = 3
// } R_GE_linejoin;
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Rcpp::List gc_to_list(const pGEcontext gc) {
  return Rcpp::List::create(
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Colours NOTE:  Alpha transparency included in col & fill
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rcpp::Named("col")        = col_to_rgba(gc->col),   // [int] pen colour (lines, text, borders, ...)
    Rcpp::Named("fill")       = col_to_rgba(gc->fill),  // [int] fill colour (for polygons, circles, rects, ...)
    Rcpp::Named("gamma")      = gc->gamma,              // [dbl] Gamma correction

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Line characteristics
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rcpp::Named("lwd")        = gc->lwd,          // [dbl] Line width (roughly number of pixels)
    Rcpp::Named("lty")        = gc->lty,          // [int] Line type (solid, dashed, dotted, ...)
    Rcpp::Named("lend")       = (int)(gc->lend),  // [enum] line end
    Rcpp::Named("ljoin")      = (int)(gc->ljoin), // [enum] line join
    Rcpp::Named("lmitre")     = gc->lmitre,       // [dbl] line mitre

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Text characteristics
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Rcpp::Named("cex")        = gc->cex,          // [dbl] Character expansion (font size = fontsize*cex)
    Rcpp::Named("ps")         = gc->ps,           // [dbl] Font size in points
    Rcpp::Named("lineheight") = gc->lineheight,   // [dbl] Line height (multiply by font size)
    Rcpp::Named("fontface")   = gc->fontface,     // [int] Font face (plain, italic, bold, ...)
    Rcpp::Named("fontfamily") = gc->fontfamily    // [chr 201] Font family
  );
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert Device Description to an R list
//
// See: /Library/Frameworks/R.framework/Versions/3.5/Resources/include/R_ext/GraphicsDevice.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Rcpp::List dd_to_list(pDevDesc dd) {

  Rcpp::List dd_list = Rcpp::List::create();

  dd_list["left"]                    = dd->left                   ;
  dd_list["top"]                     = dd->top                    ;
  dd_list["right"]                   = dd->right                  ;
  dd_list["bottom"]                  = dd->bottom                 ;

  dd_list["clipLeft"]                = dd->clipLeft               ;
  dd_list["clipTop"]                 = dd->clipTop                ;
  dd_list["clipRight"]               = dd->clipRight              ;
  dd_list["clipBottom"]              = dd->clipBottom             ;

  dd_list["xCharOffset"]             = dd->xCharOffset                    ; // [dbl] x character addressing offset - unused
  dd_list["yCharOffset"]             = dd->yCharOffset                    ; // [dbl] y character addressing offset
  dd_list["yLineBias"]               = dd->yLineBias                      ; // [dbl] 1/2 interline space as frac of line height

  dd_list["ipr"]                     = std::vector<double>(&(dd->ipr[0]), &(dd->ipr[0]) + 2); // [dbl] inches per raster (raster = pt)
  dd_list["cra"]                     = std::vector<double>(&(dd->cra[0]), &(dd->cra[0]) + 2); // [dbl] Character size in rasters; [0]=x, [1]=y

  dd_list["gamma"]                   = dd->gamma                          ; // [dbl] gamma correction factor

  dd_list["canClip"]                 = (bool)(dd->canClip)                ; // [lgl] Device-level clipping
  dd_list["canHAdj"]                 =        dd->canHAdj                 ; // [int] Can do at least some horiz adjust of text. 0 = none, 1 = {0,0.5,1}, 2 = [0,1]
  dd_list["canChangeGamma"]          = (bool)(dd->canChangeGamma)         ; // [lgl] can the gamma factor be modified?
  dd_list["displayListOn"]           = (bool)(dd->displayListOn)          ; // [lgl] toggle for initial display list status
  dd_list["haveTransparency"]        =        dd->haveTransparency        ; // [int] 0 = NA, 1 = no, 2 = yes
  dd_list["haveTransparentBg"]       =        dd->haveTransparentBg       ; // [int] 0 = NA, 1 = no, 2 = fully, 3 = semi
  dd_list["haveRaster"]              =        dd->haveRaster              ; // [int] 0 = NA, 1 = no, 2 = yes, 3 = except for missing values
  dd_list["haveCapture"]             =        dd->haveCapture             ; // [int] 0 = NA, 1 = no, 2 = yes
  dd_list["haveLocator"]             =        dd->haveLocator             ; // [int] 0 = NA, 1 = no, 2 = yes

  dd_list["startfill"]               = col_to_rgba(dd->startfill)         ; // [int] sets par("bg") and gpar("fill")
  dd_list["startcol"]                = col_to_rgba(dd->startcol)          ; // [int] sets par("fg"), par("col"), gpar("col")
  dd_list["startps"]                 = dd->startps                        ; // [dbl]
  dd_list["startlty"]                = dd->startlty                       ; // [int]
  dd_list["startfont"]               = dd->startfont                      ; // [int]
  dd_list["startgamma"]              = dd->startgamma                     ; // [dbl]

  dd_list["wantSymbolUTF8"]          = (bool)(dd->wantSymbolUTF8)         ; // [lgl]
  dd_list["hasTextUTF8"]             = (bool)(dd->hasTextUTF8)            ; // [lgl]
  dd_list["useRotatedTextInContour"] = (bool)(dd->useRotatedTextInContour); // [lgl] Is rotated text good enough to be preferable to Hershey in contour labels?

  dd_list["canGenMouseDown"]         = (bool)(dd->canGenMouseDown)        ; // [lgl] can the device generate mousedown events
  dd_list["canGenMouseMove"]         = (bool)(dd->canGenMouseMove)        ; // [lgl] can the device generate mousemove events
  dd_list["canGenMouseUp"]           = (bool)(dd->canGenMouseUp)          ; // [lgl] can the device generate mouseup events
  dd_list["canGenKeybd"]             = (bool)(dd->canGenKeybd)            ; // [lgl] can the device generate keyboard events
  dd_list["canGenIdle"]              = (bool)(dd->canGenIdle)             ; // [lgl] can the device generate idle events
  dd_list["gettingEvent"]            = (bool)(dd->gettingEvent)           ; // [lgl] This is set while getGraphicsEvent is actively looking for events

  return dd_list;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Convert an R list back to dd
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void list_to_dd(Rcpp::List dd_list, pDevDesc dd) {

  if (dd_list.containsElementNamed("left"       ))  dd->left        = Rcpp::as<double>(dd_list["left"      ]) ;
  if (dd_list.containsElementNamed("top"        ))  dd->top         = Rcpp::as<double>(dd_list["top"       ]) ;
  if (dd_list.containsElementNamed("right"      ))  dd->right       = Rcpp::as<double>(dd_list["right"     ]) ;
  if (dd_list.containsElementNamed("bottom"     ))  dd->bottom      = Rcpp::as<double>(dd_list["bottom"    ]) ;
  //
  if (dd_list.containsElementNamed("clipLeft"   ))  dd->clipLeft    = Rcpp::as<double>(dd_list["clipLeft"  ]) ;
  if (dd_list.containsElementNamed("clipTop"    ))  dd->clipTop     = Rcpp::as<double>(dd_list["clipTop"   ]) ;
  if (dd_list.containsElementNamed("clipRight"  ))  dd->clipRight   = Rcpp::as<double>(dd_list["clipRight" ]) ;
  if (dd_list.containsElementNamed("clipBottom" ))  dd->clipBottom  = Rcpp::as<double>(dd_list["clipBottom"]) ;

  if (dd_list.containsElementNamed("xCharOffset")) dd->xCharOffset  = Rcpp::as<double>(dd_list["xCharOffset"]) ; // [dbl]
  if (dd_list.containsElementNamed("yCharOffset")) dd->yCharOffset  = Rcpp::as<double>(dd_list["yCharOffset"]) ; // [dbl]
  if (dd_list.containsElementNamed("yLineBias"  )) dd->yLineBias    = Rcpp::as<double>(dd_list["yLineBias"  ]) ; // [dbl]

  if (dd_list.containsElementNamed("ipr")) {
    dd->ipr[0] = Rcpp::as<Rcpp::NumericVector>(dd_list["ipr"])[0];
    dd->ipr[1] = Rcpp::as<Rcpp::NumericVector>(dd_list["ipr"])[1];
  }

  if (dd_list.containsElementNamed("cra")) {
    dd->cra[0] = Rcpp::as<Rcpp::NumericVector>(dd_list["cra"])[0];
    dd->cra[1] = Rcpp::as<Rcpp::NumericVector>(dd_list["cra"])[1];
  }

  if (dd_list.containsElementNamed("gamma")) dd->gamma = Rcpp::as<double>(dd_list["gamma"  ]) ; // [dbl]

  if (dd_list.containsElementNamed("canClip"          )) dd->canClip           = (Rboolean)Rcpp::as<int>(dd_list["canClip"          ]); // [lgl]
  if (dd_list.containsElementNamed("canHAdj"          )) dd->canHAdj           =           Rcpp::as<int>(dd_list["canHAdj"          ]); // [int]
  if (dd_list.containsElementNamed("canChangeGamma"   )) dd->canChangeGamma    = (Rboolean)Rcpp::as<int>(dd_list["canChangeGamma"   ]); // [lgl]
  if (dd_list.containsElementNamed("displayListOn"    )) dd->displayListOn     = (Rboolean)Rcpp::as<int>(dd_list["displayListOn"    ]); // [lgl]
  if (dd_list.containsElementNamed("haveTransparency" )) dd->haveTransparency  =           Rcpp::as<int>(dd_list["haveTransparency" ]); // [int]
  if (dd_list.containsElementNamed("haveTransparentBg")) dd->haveTransparentBg =           Rcpp::as<int>(dd_list["haveTransparentBg"]); // [int]
  if (dd_list.containsElementNamed("haveRaster"       )) dd->haveRaster        =           Rcpp::as<int>(dd_list["haveRaster"       ]); // [int]
  if (dd_list.containsElementNamed("haveCapture"      )) dd->haveCapture       =           Rcpp::as<int>(dd_list["haveCapture"      ]); // [int]
  if (dd_list.containsElementNamed("haveLocator"      )) dd->haveLocator       =           Rcpp::as<int>(dd_list["haveLocator"      ]); // [int]

  // Need to convert colour as a 4 element integer matrix into a single 32bit colour value. i.e. rgba_to_col(). TODO
  // if (dd_list.containsElementNamed("startfill"        )) dd->startfill         = Rcpp::as<int>   (dd_list["startfill" ]) ; // [int]
  // if (dd_list.containsElementNamed("startcol"         )) dd->startcol          = Rcpp::as<int>   (dd_list["startcol"  ]) ; // [int]

  if (dd_list.containsElementNamed("startps"          )) dd->startps           = Rcpp::as<double>(dd_list["startps"   ]) ; // [dbl]
  if (dd_list.containsElementNamed("startlty"         )) dd->startlty          = Rcpp::as<int>   (dd_list["startlty"  ]) ; // [int]
  if (dd_list.containsElementNamed("startfont"        )) dd->startfont         = Rcpp::as<int>   (dd_list["startfont" ]) ; // [int]
  if (dd_list.containsElementNamed("startgamma"       )) dd->startgamma        = Rcpp::as<double>(dd_list["startgamma"]) ; // [dbl]


  if (dd_list.containsElementNamed("wantSymbolUTF8"         )) dd->wantSymbolUTF8          = (Rboolean)Rcpp::as<int>(dd_list["wantSymbolUTF8"         ]) ; // [lgl]
  if (dd_list.containsElementNamed("hasTextUTF8"            )) dd->hasTextUTF8             = (Rboolean)Rcpp::as<int>(dd_list["hasTextUTF8"            ]) ; // [lgl]
  if (dd_list.containsElementNamed("useRotatedTextInContour")) dd->useRotatedTextInContour = (Rboolean)Rcpp::as<int>(dd_list["useRotatedTextInContour"]) ; // [lgl]

  if (dd_list.containsElementNamed("canGenMouseDown" )) dd->canGenMouseDown    = (Rboolean)Rcpp::as<int>(dd_list["canGenMouseDown"]); // [lgl]
  if (dd_list.containsElementNamed("canGenMouseMove" )) dd->canGenMouseMove    = (Rboolean)Rcpp::as<int>(dd_list["canGenMouseMove"]); // [lgl]
  if (dd_list.containsElementNamed("canGenMouseUp"   )) dd->canGenMouseUp      = (Rboolean)Rcpp::as<int>(dd_list["canGenMouseUp"  ]); // [lgl]
  if (dd_list.containsElementNamed("canGenKeybd"     )) dd->canGenKeybd        = (Rboolean)Rcpp::as<int>(dd_list["canGenKeybd"    ]); // [lgl]
  if (dd_list.containsElementNamed("canGenIdle"      )) dd->canGenIdle         = (Rboolean)Rcpp::as<int>(dd_list["canGenIdle"     ]); // [lgl]
  if (dd_list.containsElementNamed("gettingEvent"    )) dd->gettingEvent       = (Rboolean)Rcpp::as<int>(dd_list["gettingEvent"   ]); // [lgl]
}





//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Struct of information about the graphics device
//  - rfunction - name of R function to call
//  - rdata  - list of information for R e.g. actual plotting canvas
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct cdata_struct {
  std::string rcallback;
  std::string rfunction;
  SEXP rdata;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Parse return values from R back into the device description and current cdata
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void handle_return_values_from_R(Rcpp::List res, pDevDesc dd) {

  // If a 'dd' was returned, then parse it back into the device description
  if (res.containsElementNamed("dd")) {
    if (TYPEOF(res["dd"]) == VECSXP) {
      list_to_dd(res["dd"], dd);
    } else {
      Rcpp::warning("Returned 'dd' from R is not a list. Ignoring");
    }
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// device_Activate is called when a device becomes the
// active device.  For example, it can be used to change the
// title of a window to indicate the active status of
// the device to the user.  Not all device types will
// do anything.
// The only parameter is a device driver structure.
//
// As from R 2.14.0 this can be omitted or set to NULL.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_activate(pDevDesc dd) {
  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "activate",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List()
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_activate: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// device_Cap should return an integer matrix (R colors)
// representing the current contents of the device display.
//
// The result is expected to be ROW FIRST.
//
// This will only make sense for raster devices and can
// probably only be implemented for screen devices.
//
// added 2010-06-27
//
// As from R 2.13.2 this can be left unimplemented as NULL.
// For earlier versions of R it should return R_NilValue.
//
// @return optional integer matrix of colours. Otherwise returns a dummy 5x5 matrix
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SEXP rdevice_cap(pDevDesc dd) {
  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "cap",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List()
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_cap: " + cdata->rfunction + "() " + ex_str);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If user has supplied a return value, then use that
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (res.containsElementNamed("contents")) {
    return res["contents"];
  } else {
    Rcpp::IntegerMatrix contents(5, 5);
    return contents;
  }
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a single circle

// device_Circle should have the side-effect that a
// circle is drawn, centred at the given location, with
// the given radius.
// (If the device has non-square pixels, 'radius' should
// be interpreted in the units of the x direction.)
// The border of the circle should be
// drawn in the given "col", and the circle should be
// filled with the given "fill" colour.
// If "col" is NA_INTEGER then no border should be drawn
// If "fill" is NA_INTEGER then the circle should not
// be filled.
//
// graphics parameters that should be honoured (if possible):
//   col, fill, gamma, lty, lwd
//
// @param x,y coords of centre
// @param r radius
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_circle(double x, double y, double r, const pGEcontext gc, pDevDesc dd) {
  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "circle",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("x")         = x,
        Rcpp::Named("y")         = y,
        Rcpp::Named("r")         = r
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_circle: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Set clipping box
//
// device_Clip is given the left, right, bottom, and
// top of a rectangle (in DEVICE coordinates).
// It should have the side-effect that subsequent output
// is clipped to the given rectangle.
// NOTE that R's graphics engine already clips to the
// extent of the device.
// NOTE also that this will probably only be called if
// the flag canClip is true.
//
// @param x0,y0,x1,y1 limits of clipping
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_clip(double x0, double x1, double y0, double y1, pDevDesc dd) {
  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")    = "clip",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")   = cdata->rdata,
        Rcpp::Named("dd")      = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("x0")      = x0,
        Rcpp::Named("y0")      = y0,
        Rcpp::Named("x1")      = x1,
        Rcpp::Named("y1")      = y1
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_clip: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Called when 'dev.off()' is called
//
// device_Close is called when the device is killed.
// This function is responsible for destroying any
// device-specific resources that were created in
// device_Open and for FREEing the device-specific
// parameters structure.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_close(pDevDesc dd) {
  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "close",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List()
    );
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_close: " + cdata->rfunction + "() " + ex_str);
  }


  // Release the SEXP object to be garbage collected.
  R_ReleaseObject(cdata->rdata);

  // free the memory we had assigned for the cdata
  delete(cdata);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// device_Deactivate is called when a device becomes
// inactive.
// This allows the device to undo anything it did in
// dev_Activate.
// Not all device types will do anything.
//
// As from R 2.14.0 this can be omitted or set to NULL.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_deactivate(pDevDesc dd) {
  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "deactivate",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List()
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_deactivate: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// eventHelper(dd, 1) is called by do_getGraphicsEvent before looking for a
// graphics event.  It will then call R_ProcessEvents() and eventHelper(dd, 2)
// until this or another device returns sets a non-null result value in eventEnv,
// at which time eventHelper(dd, 0) will be called.
//
// Can be left unimplemented as NULL
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_eventHelper(pDevDesc dd, int code) {
  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "eventHelper",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("code")      = code
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_eventHelper: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// added in 2.14.0, only used by screen devices.
//
// Allows graphics devices to have multiple levels of suspension:
// when this reaches zero output is flushed.
//
// Can be left unimplemented as NULL.
//
// @return optional 'level' (integer). Be careful, I don't have any idea on
// what this does!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int rdevice_holdflush(pDevDesc dd, int level) {
  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "holdflush",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("level")     = level
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_holdflush: " + cdata->rfunction + "() " + ex_str);
  }



  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If user has supplied a return value, then use that
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (res.containsElementNamed("level")) {
    return Rcpp::as<int>(res["level"]);
  } else {
    return 0;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a single line
//
// device_Line should have the side-effect that a single
// line is drawn (from x1,y1 to x2,y2)
//
// R_GE_gcontext parameters that should be honoured (if possible):
//   col, gamma, lty, lwd
//
// @param x1,y1,x2,y2 endpoint coordinates (double)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_line(double x1, double y1, double x2, double y2,
                  const pGEcontext gc, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "line",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("x1")        = x1,
        Rcpp::Named("y1")        = y1,
        Rcpp::Named("x2")        = x2,
        Rcpp::Named("y2")        = y2
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_line: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// device_Locator should return the location of the next
// mouse click (in DEVICE coordinates)
// Not all devices will do anything (e.g., postscript)
//
// As from R 2.14.0 this can be omitted or set to NULL.
//
// @return x,y location in device coordinates (double).
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Rboolean rdevice_locator(double *x, double *y, pDevDesc dd) {
  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "locator",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List()
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_locator: " + cdata->rfunction + "() " + ex_str);
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If user has supplied a return value, then use that
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (res.containsElementNamed("x") && res.containsElementNamed("y")) {
    *x = Rcpp::as<double>(res["x"]);
    *y = Rcpp::as<double>(res["y"]);
    return TRUE;
  } else {
    *x = 0;
    *y = 0;
    return FALSE;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return information about the current font settings?
//
// device_MetricInfo should return height, depth, and
// width information for the given character in DEVICE
// units.
// Note: in an 8-bit locale, c is 'char'.
// In an mbcslocale, it is wchar_t, and at least some
// of code assumes that is UCS-2 (Windows, true) or UCS-4.
// This is used for formatting mathematical expressions
// and for exact centering of text (see GText)
// If the device cannot provide metric information then
// it MUST return 0.0 for ascent, descent, and width.
//
// R_GE_gcontext parameters that should be honoured (if possible):
//   font, cex, ps
//
// @param c single character. Could be unicode
//
// @return ascent, descent and width (double)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_metricInfo(int c, const pGEcontext gc, double* ascent,
                        double* descent, double* width, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "metricInfo",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("c")         = c
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_metricInfo: " + cdata->rfunction + "() " + ex_str);
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If user has supplied a return value, then use that, otherwise use
  // a dodgy default.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (res.containsElementNamed("ascent")) {
    *ascent = (Rcpp::as<double>(res["ascent"]));
  } else {
    *ascent  = 0.0 * 72;
  }

  if (res.containsElementNamed("descent")) {
    *descent = (Rcpp::as<double>(res["descent"]));
  } else {
    *descent  = 0.0 * 72;
  }

  if (res.containsElementNamed("width")) {
    *width = (Rcpp::as<double>(res["width"]));
  } else {
    *width  = 0.0 * 72;
  }

}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// device_Mode is called whenever the graphics engine
// starts drawing (mode=1) or stops drawing (mode=0)
// GMode (in graphics.c) also says that
// mode = 2 (graphical input on) exists.
// The device is not required to do anything
//
// static void X11_Mode(int mode, pDevDesc dd);
//
// As from R 2.14.0 this can be omitted or set to NULL.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_mode(int mode, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "mode",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("mode")      = mode
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_mode: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Does the device have a device-specific way to confirm a
// new frame (for e.g. par(ask=TRUE))?
// This should be NULL if it does not.
// If it does, it returns TRUE if the device handled this, and
// FALSE if it wants the engine to do so.
//
// There is an example in the windows() device.
//
// Can be left unimplemented as NULL.
//
// @return Option: logical value. Defaults to FALSE if no return value supplied.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Rboolean rdevice_newFrameConfirm(pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "newFrameConfirm",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List()
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_newFrameConfirm: " + cdata->rfunction + "() " + ex_str);
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If user has supplied a return value, then use that, otherwise use
  // a dodgy default.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (res.containsElementNamed("is_device_specific")) {
    return (Rboolean) (Rcpp::as<int>(res["is_device_specific"]));
  } else {
    return (Rboolean) 0;
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Start a new page
//
// device_NewPage is called whenever a new plot requires
// a new page.
// A new page might mean just clearing the
// device (e.g., X11) or moving to a new page
// (e.g., postscript)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_newPage(const pGEcontext gc, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "newPage",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List()
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("newPage: " + cdata->rfunction + "() " + ex_str);
  }

}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// device_onExit is called by GEonExit when the user has aborted
// some operation, and so an R_ProcessEvents call may not return normally.
// It need not be set to any value; if null, it will not be called.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_onExit(pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "onExit",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List()
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("onExit: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a path.
//
// device_Path should draw one or more sets of points
// as a single path
//
// 'x' and 'y' give the points
//
// 'npoly' gives the number of polygons in the path
// MUST be at least 1
//
// 'nper' gives the number of points in each polygon
// each value MUST be at least 2
//
// 'winding' says whether to fill using the nonzero
// winding rule or the even-odd rule
//
// Added 2010-06-27
//
// As from R 2.13.2 this can be left unimplemented as NULL.
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
void rdevice_path(double *x, double *y, int npoly, int *nper,
                  Rboolean winding, const pGEcontext gc, pDevDesc dd) {

  int total_coords = 0;
  for (int i = 0; i < npoly; i++) {
    total_coords += nper[i];
  }

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "path",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("x")         = std::vector<double>(x, x+total_coords),
        Rcpp::Named("y")         = std::vector<double>(y, y+total_coords),
        Rcpp::Named("npoly")     = npoly,
        Rcpp::Named("nper")      = std::vector<int>(nper, nper+npoly),
        Rcpp::Named("winding")   = (bool)winding
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_path: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a polygon
//
// device_Polygon should have the side-effect that a
// polygon is drawn using the given x and y values
// the polygon border should be drawn in the "col"
// colour and filled with the "fill" colour.
// If "col" is NA_INTEGER don't draw the border
// If "fill" is NA_INTEGER don't fill the polygon
// An example is ...
//
// static void X11_Polygon(int n, double *x, double *y,
//                         const pGEcontext gc,
//                         pDevDesc dd);
//
// R_GE_gcontext parameters that should be honoured (if possible):
//   col, fill, gamma, lty, lwd
//
// @param n number of points in polygon
// @param x,y n sets of coordinates
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_polygon(int n, double *x, double *y, const pGEcontext gc, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")    = "polygon",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")   = cdata->rdata,
        Rcpp::Named("gc")      = gc_to_list(gc),
        Rcpp::Named("dd")      = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("n")       = n,
        Rcpp::Named("x")       = std::vector<double>(x, x+n),
        Rcpp::Named("y")       = std::vector<double>(y, y+n)
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_polygon: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a polyline
//
// device_Polyline should have the side-effect that a
// series of line segments are drawn using the given x
// and y values.
// An example is ...
//
// static void X11_Polyline(int n, double *x, double *y,
//                          const pGEcontext gc,
//                          pDevDesc dd);
//
// R_GE_gcontext parameters that should be honoured (if possible):
//   col, gamma, lty, lwd
//
// @param n number of points in polyline
// @param x,y n sets of coordinates
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_polyline(int n, double *x, double *y, const pGEcontext gc, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "polyline",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("n")         = n,
        Rcpp::Named("x")         = std::vector<double>(x, x+n),
        Rcpp::Named("y")         = std::vector<double>(y, y+n)
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("polyline: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Add a raster (image)
//
// device_Raster should draw a raster image justified
// at the given location,
// size, and rotation (not all devices may be able to rotate?)
//
// 'raster' gives the image data BY ROW, with every four bytes
// giving one R colour (ABGR).
//
// 'x and 'y' give the bottom-left corner.
//
// 'rot' is in degrees (as per device_Text), with positive
// rotation anticlockwise from the positive x-axis.
//
// As from R 2.13.2 this can be left unimplemented as NULL.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_raster(unsigned int *raster, int w, int h,
                    double x, double y, double width, double height,
                    double rot, Rboolean interpolate,
                    const pGEcontext gc, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction")   = cdata->rfunction,
      Rcpp::Named("device_call")       = "raster",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")       = cdata->rdata,
        Rcpp::Named("gc")          = gc_to_list(gc),
        Rcpp::Named("dd")          = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("raster")      = std::vector<int>(raster, raster + w*h),
        Rcpp::Named("w")           = w,
        Rcpp::Named("h")           = h,
        Rcpp::Named("x")           = x,
        Rcpp::Named("y")           = y,
        Rcpp::Named("width")       = width,
        Rcpp::Named("height")      = height,
        Rcpp::Named("rot")         = rot,
        Rcpp::Named("interpolate") = (bool)interpolate
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_raster: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw a single rectangle
//
// device_Rect should have the side-effect that a
// rectangle is drawn with the given locations for its
// opposite corners.  The border of the rectangle
// should be in the given "col" colour and the rectangle
// should be filled with the given "fill" colour.
// If "col" is NA_INTEGER then no border should be drawn
// If "fill" is NA_INTEGER then the rectangle should not
// be filled.
//
// @param x0,y0,x1,y1 limits of rectangle
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_rect(double x0, double y0, double x1, double y1,
                  const pGEcontext gc, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "rect",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("x0")        = x0,
        Rcpp::Named("y0")        = y0,
        Rcpp::Named("x1")        = x1,
        Rcpp::Named("y1")        = y1
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_rect: " + cdata->rfunction + "() " + ex_str);
  }

}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Return information about the device size
//
// device_Size is called whenever the device is resized.
// The function returns (left, right, bottom, and top) for the
// new device size.
// This is not usually called directly by the graphics
// engine because the detection of device resizes
// (e.g., a window resize) are usually detected by
// device-specific code.
//
// As from R 2.13.2 this can be left unimplemented as NULL.
//
// @return left, right, bottom, top. numeric values representing the size of the device
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_size(double *left, double *right, double *bottom, double *top,
                  pDevDesc dd) {


  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "size",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List()
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_size: " + cdata->rfunction + "() " + ex_str);
  }



  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If user has supplied a return value, then use that, otherwise use
  // a dodgy default.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (res.containsElementNamed("left")) {
    *left = (Rcpp::as<double>(res["left"]));
  } else {
    *left   = dd->left;
  }

  if (res.containsElementNamed("right")) {
    *right = (Rcpp::as<double>(res["right"]));
  } else {
    *right  = dd->right;
  }

  if (res.containsElementNamed("bottom")) {
    *bottom = (Rcpp::as<double>(res["bottom"]));
  } else {
    *bottom = dd->bottom;
  }

  if (res.containsElementNamed("top")) {
    *top = (Rcpp::as<double>(res["top"]));
  } else {
    *top    = dd->top;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Calculate and return the string width in the current state
//
// device_StrWidth should return the width of the given
// string in DEVICE units.
//
// R_GE_gcontext parameters that should be honoured (if possible):
//   font, cex, ps
//
// @param str string
//
// @return Optionally return 'width' the display width of the string in device units (numeric).
//         If not returned then a default value is used i.e. (strlen(str) + 2) * 72
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double rdevice_strWidth(const char *str, const pGEcontext gc, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "strWidth",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("str")       = std::string(str)
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_strWidth: " + cdata->rfunction + "() " + ex_str);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If user has supplied a return value, then use that, otherwise use
  // a dodgy default.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (res.containsElementNamed("width")) {
    return Rcpp::as<double>(res["width"]);
  } else {
    return (strlen(str) + 2) * gc->cex * gc->ps;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// str width in UTF8 directly (if 'hasTextUTF8' = TRUE)
//
// Some devices can plot UTF-8 text directly without converting
// to the native encoding, e.g. windows(), quartz() ....
//
// If this flag is true, all text *not in the symbol font* is sent
// in UTF8 to the textUTF8/strWidthUTF8 entry points.
//
// If the flag is TRUE, the metricInfo entry point should
// accept negative values for 'c' and treat them as indicating
// Unicode points (as well as positive values in a MBCS locale).
//
// @param str string
//
// @return 'width' the display width of the string in device units (numeric)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double rdevice_strWidthUTF8(const char *str, const pGEcontext gc, pDevDesc dd) {

  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "strWidthUTF8",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("str")       = std::string(str)
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_strWidthUTF8: " + cdata->rfunction + "() " + ex_str);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If user has supplied a return value, then use that, otherwise use
  // a dodgy default.
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (res.containsElementNamed("width")) {
    return Rcpp::as<double>(res["width"]);
  } else {
    return (strlen(str) + 2) * gc->cex * gc->ps;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw text
//
// device_Text should have the side-effect that the
// given text is drawn at the given location.
// The text should be rotated according to rot (degrees)

//
// R_GE_gcontext parameters that should be honoured (if possible):
//   font, cex, ps, col, gamma
//
// @param x,y location of text (double)
// @param str string
// @param rot text rotation (degrees)
// @param hadj horizontal adjustment (double)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_text(double x, double y, const char *str, double rot,
                  double hadj, const pGEcontext gc, pDevDesc dd) {


  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "text",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("x")         = x,
        Rcpp::Named("y")         = y,
        Rcpp::Named("str")       = std::string(str),
        Rcpp::Named("rot")       = rot,
        Rcpp::Named("hadj")      = hadj
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("text: " + cdata->rfunction + "() " + ex_str);
  }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Draw text in UTF8 directly (if 'hasTextUTF8' = TRUE)
//
// Some devices can plot UTF-8 text directly without converting
// to the native encoding, e.g. windows(), quartz() ....
//
// If this flag is true, all text *not in the symbol font* is sent
// in UTF8 to the textUTF8/strWidthUTF8 entry points.
//
// If the flag is TRUE, the metricInfo entry point should
// accept negative values for 'c' and treat them as indicating
// Unicode points (as well as positive values in a MBCS locale).
//
// R_GE_gcontext parameters that should be honoured (if possible):
//   font, cex, ps, col, gamma
//
// @param x,y location of text (double)
// @param str string
// @param rot text rotation (degrees)
// @param hadj horizontal adjustment (double)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void rdevice_textUTF8(double x, double y, const char *str, double rot,
                      double hadj, const pGEcontext gc, pDevDesc dd) {


  cdata_struct *cdata = (cdata_struct *)dd->deviceSpecific;
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback = package_env[cdata->rcallback];
  Rcpp::List res;

  try {
    res = rcallback(
      Rcpp::Named("rfunction") = cdata->rfunction,
      Rcpp::Named("device_call")     = "textUTF8",

      Rcpp::Named("state") = Rcpp::List::create(
        Rcpp::Named("rdata")     = cdata->rdata,
        Rcpp::Named("gc")        = gc_to_list(gc),
        Rcpp::Named("dd")        = dd_to_list(dd)
      ),

      Rcpp::Named("args") = Rcpp::List::create(
        Rcpp::Named("x")         = x,
        Rcpp::Named("y")         = y,
        Rcpp::Named("str")       = std::string(str),
        Rcpp::Named("rot")       = rot,
        Rcpp::Named("hadj")      = hadj
      )
    );
    handle_return_values_from_R(res, dd);
  } catch(std::exception &ex) {
    std::string ex_str = ex.what();
    Rcpp::warning("rdevice_textUTF8: " + cdata->rfunction + "() " + ex_str);
  }

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Create and initialise the device description
//
// - Create a DevDesc (Device Description structure)
// - Populate it with functions to call for each of the primitive elements
//   that need to be created
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pDevDesc rdevice_open(std::string rfunction, SEXP rdata) {


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Device description struct is passed to all callbacks
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  pDevDesc dd = (DevDesc*) calloc(1, sizeof(DevDesc));
  if (dd == NULL)
    return dd;


  double pointsize = 12;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // If the user doesn't specify, then use this width/height (in inches)
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  int width  = 10;
  int height =  8;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Interpret the rdata as a list and see if width, height and pointsize
  // have been specified
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Rcpp::Environment rcl = Rcpp::clone(rdata);
  R_PreserveObject(rcl);

  if (rcl.exists("width"    )) width      = Rcpp::as<int>(rcl["width"]);
  if (rcl.exists("height"   )) height     = Rcpp::as<int>(rcl["height"]);
  if (rcl.exists("pointsize")) pointsize  = Rcpp::as<double>(rcl["pointsize"]);


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Callbacks
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->activate        = rdevice_activate;
  dd->cap             = rdevice_cap;
  dd->circle          = rdevice_circle;
  dd->clip            = rdevice_clip;
  dd->close           = rdevice_close;
  dd->deactivate      = rdevice_deactivate;
  dd->eventHelper     = rdevice_eventHelper;
  dd->holdflush       = rdevice_holdflush;
  dd->line            = rdevice_line;
  dd->locator         = rdevice_locator;
  dd->metricInfo      = rdevice_metricInfo;
  dd->mode            = rdevice_mode;
  dd->newFrameConfirm = rdevice_newFrameConfirm;
  dd->newPage         = rdevice_newPage;
  dd->onExit          = rdevice_onExit;
  dd->path            = rdevice_path;
  dd->polygon         = rdevice_polygon;
  dd->polyline        = rdevice_polyline;
  dd->raster          = rdevice_raster;
  dd->rect            = rdevice_rect;
  dd->size            = rdevice_size;
  dd->strWidth        = rdevice_strWidth;
  dd->strWidthUTF8    = rdevice_strWidthUTF8;
  dd->text            = rdevice_text;
  dd->textUTF8        = rdevice_textUTF8;


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Screen Dimensions in pts (doubles)
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->left   = 0;
  dd->top    = 0;
  dd->right  = width  * 72;
  dd->bottom = height * 72;


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // R only has the notion of a rectangular clipping region
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->clipLeft   = dd->left;
  dd->clipTop    = dd->top;
  dd->clipRight  = dd->right;
  dd->clipBottom = dd->bottom;


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // character alignment offsets
  // R source code says (no author attributed)
  //    "I hate these next three -- they seem like a real fudge
  //     BUT I'm not sure what to replace them with so they stay for now"
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->xCharOffset = 0.4900; // [dbl] x character addressing offset - unused
  dd->yCharOffset = 0.3333; // [dbl] y character addressing offset
  dd->yLineBias   = 0.2;    // [dbl] 1/2 interline space as frac of line height


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // inches per raster (raster = pt)
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->ipr[0] = 1.0 / 72.0;
  dd->ipr[1] = 1.0 / 72.0;


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Magic constants copied from other graphics devices
  // nominal character sizes in pts
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->cra[0] = 0.9 * pointsize; // [dbl] Character size in rasters; [0]=x, [1]=y
  dd->cra[1] = 1.2 * pointsize;

  dd->gamma = 1;                // [dbl] Gamma correction factor

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Device capabilities
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->canClip           = TRUE;  // [lgl] Device-level clipping
  dd->canHAdj           = 0;     // [int] Can do at least some horiz adjust of text. 0 = none, 1 = {0,0.5,1}, 2 = [0,1]
  dd->canChangeGamma    = FALSE; // [lgl] can the gamma factor be modified?
  dd->displayListOn     = FALSE; // [lgl] toggle for initial display list status
  dd->haveTransparency  = 2;     // [int] 0 = NA, 1 = no, 2 = yes
  dd->haveTransparentBg = 2;     // [int] 0 = NA, 1 = no, 2 = fully, 3 = semi
  dd->haveRaster        = 2;     // [int] 0 = NA, 1 = no, 2 = yes, 3 = except for missing values
  dd->haveCapture       = 2;     // [int] 0 = NA, 1 = no, 2 = yes
  dd->haveLocator       = 2;     // [int] 0 = NA, 1 = no, 2 = yes

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Initial settings
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->startfill  = R_RGB(255, 255, 255); // [int] sets par("bg") and gpar("fill")
  dd->startcol   = R_RGB(0, 0, 0);       // [int] sets par("fg"), par("col"), gpar("col")
  dd->startps    = pointsize;            // [dbl]
  dd->startlty   = 0;                    // [int]
  dd->startfont  = 1;                    // [int]
  dd->startgamma = 1;                    // [dbl]


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // UTF-8 support
  // If hasTextUTF8 is TRUE, all text *not in the symbol font* is sent
  // in UTF8 to the textUTF8/strWidthUTF8 entry points
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->wantSymbolUTF8 = TRUE; // [lgl]
  dd->hasTextUTF8    = TRUE; // [lgl]

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Is rotated text good enough to be preferable to Hershey in contour labels?
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->useRotatedTextInContour = TRUE; // [lgl]

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Event handling entries
  // Used in do_setGraphicsEventEnv
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  dd->canGenMouseDown = FALSE; // [lgl] can the device generate mousedown events
  dd->canGenMouseMove = FALSE; // [lgl] can the device generate mousemove events
  dd->canGenMouseUp   = FALSE; // [lgl] can the device generate mouseup events
  dd->canGenKeybd     = FALSE; // [lgl] can the device generate keyboard events
  dd->canGenIdle      = FALSE; // [lgl] can the device generate idle events
  dd->gettingEvent    = FALSE; // [lgl] This is set while getGraphicsEvent is actively looking for events


  //--------------------------------------------------------------------------
  // Create device-specific data structure to store state info
  //--------------------------------------------------------------------------
  cdata_struct *cdata = new cdata_struct;
  cdata->rcallback = "rcallback";
  cdata->rfunction = rfunction;
  cdata->rdata     = rcl;


  dd->deviceSpecific = cdata;

  //--------------------------------------------------------------------------
  // Give the user the opportunity to edit 'dd' before anything starts
  //--------------------------------------------------------------------------
  Rcpp::Environment package_env("package:devout");
  Rcpp::Function rcallback_func = package_env[cdata->rcallback];
  Rcpp::List res = rcallback_func(
    Rcpp::Named("rfunction") = cdata->rfunction,
    Rcpp::Named("device_call") = "open",

    Rcpp::Named("state") = Rcpp::List::create(
      Rcpp::Named("rdata")     = cdata->rdata,
      Rcpp::Named("dd")        = dd_to_list(dd)
    ),

    Rcpp::Named("args") = Rcpp::List()
  );

  handle_return_values_from_R(res, dd);

  return dd;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initialise a device descriptor and tell R about it
//
// This function is mostly cloned from svglite/src/devSVG.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void make_rdevice_device(std::string rfunction, SEXP rdata) {

  R_GE_checkVersionOrDie(R_GE_version);
  R_CheckDeviceAvailable();
  BEGIN_SUSPEND_INTERRUPTS {
    pDevDesc dev = rdevice_open(rfunction, rdata);
    if (dev == NULL)
      Rcpp::stop("Failed to start 'rdevice' device");

    pGEDevDesc dd = GEcreateDevDesc(dev);
    GEaddDevice2(dd, "rdevice");
    GEinitDisplayList(dd);

  } END_SUSPEND_INTERRUPTS;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//' Create a rdevice graphics device
//'
//' @param rfunction name of callback function
//' @param rdata a list of information used on the R side
//'
// [[Rcpp::export]]
bool rdevice_(std::string rfunction, SEXP rdata) {
  make_rdevice_device(rfunction, rdata);
  return true;
}




