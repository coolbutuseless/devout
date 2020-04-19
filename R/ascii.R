


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Graphics device for ASCII output
#'
#' Graphics primitives will be rendered using ASCII characters.
#'
#' Uses \code{devout::rdevice()}.
#'
#' @param filename If given, write ascii to this file, otherwise write to console.
#' @param width,height dimensions of text output (in characters). Default: NULL
#'                     (auto-detect)
#' @param font_aspect Character spacing horizontally and vertically are almost
#'        never the same. On many terminals character resolution vertically is
#'        half the resolution horizontally.  Adjust this value if circles don't
#'        look right. Default: 0.45
#' @param ... other parameters passed to the rdevice
#'
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii <- function(filename = NULL, width = NULL, height = NULL, font_aspect = 0.45, ...) {
  rdevice(ascii_callback, filename = filename, width = width, height = height, font_aspect = font_aspect, ...)
}
