
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create a descriptive graphics device
#'
#' @param verbosity level. default: 1
#' @param width,height nominal size of drawing area in pixels. default: width = 400, height = 300
#' @param ... all other arguments ignored
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
descriptive <- function(verbosity = 1L, width = 400L, height = 300L,  ...) {
  invisible(
    .Call(`_devout_descriptive_`, verbosity, width, height)
  )
}

