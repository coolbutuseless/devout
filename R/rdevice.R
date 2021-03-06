

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# For debugging purposes, it is possible to access the rdata environment
# used for any device - it is stored in an environment within the package called
# 'device_rdata'.
#
# When the device is closed, its `rdata` environment is deleted.
#
# This means that any time prior to "dev.off()" you can access the
# environment within `devout::device_rdata`
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
device_rdata <- new.env()


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create an rdevice graphics device
#'
#' Inspired by: http://www.omegahat.net/RGraphicsDevice/overview.html
#'
#' @param rfunction a function (preferred) or
#'        a character string (soft-deprecated) containing name of callback function
#'        which will handle the device calls.
#' @param ... all other named, non-NULL arguments are passed into the device
#'            as `rdata`
#' @param device_name name to use for the device. default: "rdevice"
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rdevice <- function(rfunction, ..., device_name = 'rdevice') {

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
  # Generate a time code to use as the unique key for the environment for
  # this instance of the device.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  rdata$.key <- paste0('rdata-', strftime(Sys.time(), format = "%FT%H:%M:%OS6"))


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Determine if the rdevice is valid
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (is.function(rfunction)) {
    func <- rfunction
  } else if (is.character(rfunction)) {
    if (exists(rfunction)) {
      func <- get(rfunction)
      if (!is.function(func)) {
        stop("rdevice(): Supplied character string does not reference a function: ", rfunction, call.=FALSE)
      }
    } else {
      stop("rdevice(): Supplied character string does not reference a function: ", rfunction, call.=FALSE)
    }
  } else {
    stop("rdevice(): supplied device is not a function or name of a function.", call.=FALSE)
  }

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Add the 'rfunction' to the 'rdata'
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  rdata$rfunction <- func

  invisible(
    .Call(`_devout_rdevice_`, rdata, device_name)
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
#' @param device_call name of device call
#' @param state list of rdata, dd and gc
#' @param args args to the device call
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
rcallback <- function(device_call, state, args) {

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # `state$rdata` is an environment with all the state for the current device
  # that is needed from the R side.  Cache the rdata object within the
  # package `device_rdata` object to aid in debugging.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (device_call == 'open') {
    key <- state$rdata$.key
    device_rdata[[key]] <- state$rdata
  }

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Call the function
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  func <- state$rdata$rfunction
  new_state <- func(device_call = device_call, args = args, state = state)

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Sanity check we got a list back, and complain if we didn't.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!is.null(new_state) && !is.list(new_state)) {
    warning("rdevice: expecting a list to be returned by ", shQuote(device_call),
            "but got: ", deparse(class(new_state)))
    new_state <- list()
  }

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # if we're closing the device, also delete the reference to the rdata
  # environment
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (device_call == 'close') {
    key <- state$rdata$.key
    rm(list = key, envir = device_rdata)
  }

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # If the device call can take a return value, check that the return value
  # given by the user (if at all) is of the right type and length.
  # e.g. strWidth should return a numeric width
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  new_state <- sanitize_return_types(device_call, new_state)


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # If the device description has changed from what C++ gave to R, then we
  # should validate it before passing it back from R to C++.
  # This avoids the situation where the user has corrupted the device description
  # and then having C++ code somehow deal with the error. Throw an error early in R!
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!is.null(new_state$dd) && !identical(new_state$dd, state$dd)) {
    new_state$dd <- sanitize_device_description(new_state$dd)
  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # 1000% **ABSOLUTELY** **MUST** pass a real **LIST** back to C++.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (is.list(new_state)) {
    new_state
  } else {
    list()
  }
}
