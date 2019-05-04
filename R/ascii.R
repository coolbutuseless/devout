
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create an ascii graphics device
#'
#' Graphics primitives will be rendered using ascii characters.
#'
#' @param filename If given, write ascii to this file, otherwise write to console.
#'        If file is a format string e.g. "output%03i.txt", then this will be used
#'        to create a unique filename if multiple pages are output within the same
#'        \code{ascii()} call.
#' @param width,height dimensions of text output (in characters). If NULL then
#'        auto-detect console width and set approximate aspect ratio at 1/2
#' @param font_aspect Character spacing horizontally and vertically are almost
#'        never the same. On many terminals character spacing horizontally is
#'        half the spacing vertically. This argument is only used if width
#'        is autodetected and/or height is not specified. Default: 0.5
#' @param verbosity level. default: 0.  valid: 0, 1, 2
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii <- function(filename    = NULL,
                  width       = NULL,
                  height      = NULL,
                  font_aspect = 0.5,
                  verbosity   = 0) {


    if (is.null(filename) || filename == '' || is.na(filename) ||
        !is.character(filename) || filename == '-') {
      filename <- 'console'
    }

    if (is.null(width)) {
      width  <- getOption('width', default = 80)
    }

    if (is.null(height)) {
      height <- as.integer(width * 1/2 * font_aspect) # extra 1/2 multiplier for font sizing
    }

    # Need to keep a reference to the matrix in R otherwise I think it
    # might get garbage collected, and then the Rcpp code is drawing over
    # memory it might not own anymore?  None of this may be true, but doing
    # this stopped some weird corruption errors in plots, so I'm going to keep it
    # mikefc 20190501
    mat <- .Call(`_devout_ascii_`, filename, width, height, verbosity)

    keep_alive_env$mat <- mat

    invisible(mat)
}

