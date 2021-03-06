


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Information about the \code{graphics parameters} (\code{gc}) \code{device description} (\code{dd}) and \code{device calls}
#'
#' The following documentation is copied from: src/include/R_ext/GraphicsEngine.h
#'
#' A structure containing graphical parameters
#'
#' This is how graphical parameters are passed from graphics systems
#' to the graphics engine AND from the graphics engine to graphics
#' devices.
#'
#' Devices are not *required* to honour graphical parameters
#' (e.g., alpha transparency is going to be tough for some)
#'
#'
#'
#'
#'
#'
#' Information about the \code{device description} list (\code{dd})
#'
#' The structure is passed from C++ to the user callback function.
#'
#' It can also be returned to the C++ code.
#'
#' The C++ header file which defines this is: src/include/R_ext/GraphicsDevice.h
#'
#'
#' @format The \code{devinfo} object is a named list of \code{gc}, \code{dd} and \code{device_call} information.
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"devinfo"
