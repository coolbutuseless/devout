

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Verbose debugging callback for the rdevice
#'
#' @param device_call name of device function call
#' @param args arguments to device function call
#' @param state list of rdata, dd and gc. Some or all of which may be NULL
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
verbose_callback <- function(device_call, args, state) {
  if (device_call %in% state$rdata$skip) return()
  cat("[", device_call, "]: ")
  cat(paste(names(args), args, sep = " = ", collapse = ",  "), "\n", sep = "")
}




#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Verbose device
#'
#' Simply prints all calls to the graphics driver
#'
#' Uses \code{devout::rdevice()}.
#'
#' @param skip names of device calls to ignore
#' @param ... other parameters passed to the rdevice
#'
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
verbose <- function(skip = c('mode', 'strWidthUTF8', 'metricInfo', 'clip'), ...) {
  rdevice(verbose_callback, skip = skip, ...)
}
