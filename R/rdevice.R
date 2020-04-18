

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# For debugging purposes, it is possible to keep the rdata used for any
# device - it is stored in an environment within the package called
# 'device_rdata'.
#
# Every time a device call is made, the 'rdata' for this device is saved to this
# environment.  Usually when the device is closed, this cached version of `rdata`
# is deleted.
#
# To keep this `rdata` for a device even after it has been closed set the
# following option:
#    options(DEVOUT_KEEP_RDATA = TRUE)
#
# Note: If you do enable this option, note that the environment is never tidied,
# so the cache of `rdata` will increase in size for every run until R restarted.
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
device_rdata <- new.env()


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create an rdevice graphics device
#'
#' Inspired by: http://www.omegahat.net/RGraphicsDevice/overview.html
#'
#' @param rfunction character string containing name of callback function in R
#' @param ... all other named, non-NULL objects are passed into the device
#'            as `rdata`
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rdevice <- function(rfunction, ...) {

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Add arguments from ... to the rdata
  #  - unnamed args are dropped
  #  - NULLs are dropped
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  varargs  <- list(...)
  varnames <- names(varargs)
  if (!is.null(varnames)) {
    valid_values <- vapply(varargs, Negate(is.null), logical(1))
    valid_names <- which(!is.na(varnames) & (varnames != ""))
    rdata <- varargs[valid_names & valid_values]
    rdata <- as.environment(rdata)
  } else {
    rdata <- new.env()
  }

  rdata$pointsize <- rdata$pointsize %||% 12

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Generate a time code to use as the unique key for the data for
  # this instance of the device.  If `DEVOUT_KEEP_RDATA` option is set to
  # TRUE then `rdata` will be cached in `devout:::device_rdata` to aid
  # with debugging
  # Set options(DEVOUT_KEEP_RDATA = TRUE) to use.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  rdata$.key <- strftime(Sys.time(), format = "%FT%H:%M:%OS6")

  invisible(
    .Call(`_devout_rdevice_`, rfunction, rdata)
  )
}





#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Sanitize the types of any explicit 'return' values expected by 'rdevice'
#'
#' @param device_call device call name
#' @param state return list
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sanitize_return_types <- function(device_call, state) {

  checks <- list(
    list(device_call = 'strWidth'       , return_name = 'width'          , type = 'non-negative numeric'),
    list(device_call = 'strWidthUTF8'   , return_name = 'width'          , type = 'non-negative numeric'),
    list(device_call = 'newFrameConfirm', return_name = 'device_specific', type = 'logical'),
    list(device_call = 'metricInfo'     , return_name = 'ascent'         , type = 'non-negative numeric'),
    list(device_call = 'metricInfo'     , return_name = 'descent'        , type = 'numeric'),
    list(device_call = 'metricInfo'     , return_name = 'width'          , type = 'non-negative numeric'),
    list(device_call = 'locator'        , return_name = 'x'              , type = 'non-negative numeric'),
    list(device_call = 'locator'        , return_name = 'y'              , type = 'non-negative numeric'),
    list(device_call = 'holdflush'      , return_name = 'level'          , type = 'non-negative integer'),
    list(device_call = 'size'           , return_name = 'left'           , type = 'non-negative numeric'),
    list(device_call = 'size'           , return_name = 'right'          , type = 'non-negative numeric'),
    list(device_call = 'size'           , return_name = 'bottom'         , type = 'non-negative numeric'),
    list(device_call = 'size'           , return_name = 'top'            , type = 'non-negative numeric')
  )


  for (check in checks) {

    if (identical(device_call, check$device_call) && (check$return_name %in% names(state))) {
      return_name <- check$return_name
      bad <- switch(check$type,
                    'numeric'              = { is.null(state[[return_name]]) || is.na(state[[return_name]]) || !is.numeric(state[[return_name]])                           },
                    'non-negative numeric' = { is.null(state[[return_name]]) || is.na(state[[return_name]]) || !is.numeric(state[[return_name]]) || state[[return_name]] < 0 },
                    'non-negative integer' = { is.null(state[[return_name]]) || is.na(state[[return_name]]) || !is.integer(state[[return_name]]) || state[[return_name]] < 0 },
                    'logical'              = { is.null(state[[return_name]]) || is.na(state[[return_name]]) || !is.logical(state[[return_name]])                           },
                    stop("sanitize_return_types: No such check type: ", check$type)
      )
      if (bad) {
        warning("Ignoring invalid '", check$return_name, "' returned from call to '", check$device_call,
                "'. Must be ", check$type, " value, but got: [", deparse(state[[return_name]]), "]")
        state[[return_name]] <- NULL
      }
    }

  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Check return values for: cap.
  # It should be an integer matrix
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (identical(device_call, 'cap')) {
    if (('contents' %in% names(state)) &&
        (is.null(state$contents)             ||
         is.na(state$contents)               ||
         class(state$contents) != 'matrix'   ||
         typeof(state$contents) != 'integer' ||
         anyNA(state$contents))) {
      warning("Ignoring invalid 'contents' returned from call to 'cap'. Must be integer matrix")
      state$contents <- NULL
    }
  }

  state
}




#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Callback for the rdevice
#'
#' This shim sits between the C++ world and the R world. I.e. this R function
#' gets called from the C++ code.  Calls are then made from
#' here to the R callback function.
#'
#' This shim should do the heavy lifting in terms of error checking to ensure that errors
#' aren't propogated back to the C++ execution (because if that happens we
#' get a segmentation fault!)
#'
#' It should also do sanity checking of return objects i.e. is 'rdata' still a list?
#'
#' TODO: Could then do signatures of the input state and the return values and
#' only include them in the return object if things have changed?
#'
#' @param rfunction name of rfunction
#' @param device_call name of device call
#' @param state list of rdata, dd and gc
#' @param args args to the device call
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rcallback <- function(rfunction, device_call, state, args) {

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Test that the function exists and is callable
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (is.null(rfunction) || rfunction == '' || !exists(rfunction)) {
    warning("rdevice: r callback not found: ", rfunction)
    return(list())
  }

  func <- get(rfunction)
  if (!is.function(func)) {
    warning("rdevice: r callback is not a function: ", rfunction)
    return(list())
  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # On the first call from C++ code, we should have rdata$.key and the
  # initial data transfer. Cache 'rdata' here
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  key <- state$rdata$.key

  if (device_call == 'open') {
    device_rdata[[key]] <- state$rdata
  } else {
    state$rdata <- device_rdata[[key]]
  }

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Call the function
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  state <- func(device_call = device_call, args = args, state = state)

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Sanity check we got a list back, and complain if we didn't.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!is.null(state) && !is.list(state)) {
    warning("rdevice: r callback should return a list from call to ", shQuote(device_call))
    state <- list()
  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Cache the value of the devices 'rdata'
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ('rdata' %in% names(state)) {
    device_rdata[[key]] <- modify_list(device_rdata[[key]], state$rdata)
  }

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # if we're closing the device, also delete the rdata associated with this device,
  # unless specificly requested by the user to keep it.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (device_call == 'close') {
    if (isTRUE(getOption('DEVOUT_KEEP_RDATA', FALSE))) {
      message("devout: kept `rdata` - ls(devout:::device_rdata[['", key, "']])")
    } else {
      rm(list = key, envir = device_rdata)
    }
  }

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # If the device call can take a return value, check that the return value
  # given by the user (if at all) is of the right type and length
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  state <- sanitize_return_types(device_call, state)


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # If the device description has changed from what C++ gave to R, then we
  # should validate it before passing it back from R to C++.
  # This avoids the situation where the user has corrupted the device description
  # and then having C++ code somehow deal with the error. Throw an error early in R!
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!is.null(state$dd) && !identical(state$dd, state$dd)) {
    state$dd <- sanitize_device_description(state$dd)
  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # 1000% **ABSOLUTELY** **MUST** pass a real **LIST** back to C++.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (is.list(state)) {
    state
  } else {
    list()
  }
}
