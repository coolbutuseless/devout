
device_rdata <- new.env()

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Create a rdevice graphics device
#'
#' Inspired by: http://www.omegahat.net/RGraphicsDevice/overview.html
#'
#' @param rfunction character string containing name of callback function in R
#' @param ... all other named, non-NULL objects are passed into the device
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
  } else {
    rdata <- list()
  }

  rdata$pointsize <- rdata$pointsize %||% 12

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Generate a time code to use as the unique key for the data for
  # this instance of the device
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  op <- options(digits.secs = 6)
  rdata$.key <- gsub("\\s+", "-", as.character(Sys.time()))
  options(op)


  invisible(
    .Call(`_devout_rdevice_`, rfunction, rdata)
  )
}





#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Sanitize the types of any explicit 'return' values expected by 'rdevice'
#'
#' @param device_call device call name
#' @param res return list
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sanitize_return_types <- function(device_call, res) {

  checks <- list(
    list(device_call = 'strWidth'       , return_name = 'width'          , type = 'non-negative numeric'),
    list(device_call = 'strWidthUTF8'   , return_name = 'width'          , type = 'non-negative numeric'),
    list(device_call = 'newFrameConfirm', return_name = 'device_specific', type = 'logical'),
    list(device_call = 'metricInfo'     , return_name = 'ascent'         , type = 'non-negative numeric'),
    list(device_call = 'metricInfo'     , return_name = 'descent'        , type = 'non-negative numeric'),
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

    if (identical(device_call, check$device_call) && (check$return_name %in% names(res))) {
      return_name <- check$return_name
      bad <- switch(check$type,
                    'non-negative numeric' = { is.null(res[[return_name]]) || is.na(res[[return_name]]) || !is.numeric(res[[return_name]]) || res[[return_name]] < 0 },
                    'non-negative integer' = { is.null(res[[return_name]]) || is.na(res[[return_name]]) || !is.integer(res[[return_name]]) || res[[return_name]] < 0 },
                    'logical'              = { is.null(res[[return_name]]) || is.na(res[[return_name]]) || !is.logical(res[[return_name]])                           },
                    stop("sanitize_return_types: No such check type: ", check$type)
      )
      if (bad) {
        warning("Ignoring invalid '", check$return_name, "' returned from call to '", check$device_call,  "'. Must be ", check$type, " value")
        res[[return_name]] <- NULL
      }
    }

  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Check return values for: cap.
  # It should be an integer matrix
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (identical(device_call, 'cap')) {
    if (('contents' %in% names(res)) &&
        (is.null(res$contents)             ||
         is.na(res$contents)               ||
         class(res$contents) != 'matrix'   ||
         typeof(res$contents) != 'integer' ||
         anyNA(res$contents))) {
      warning("Ignoring invalid 'contents' returned from call to 'cap'. Must be integer matrix")
      res$contents <- NULL
    }
  }

  res
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
  # initial data transfer
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  key <- state$rdata$.key

  if (device_call == 'open') {
    # cat("key: ", key, "\n")
    device_rdata[[key]] <- state$rdata
  }

  state$rdata <- device_rdata[[key]]


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Call the function
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  res <- func(device_call = device_call, args = args, state = state)

  if (!is.null(res) && !is.list(res)) {
    warning("rdevice: r callback should return a list")
    res <- list()
  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Cache the value of the devices 'rdata'
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if ('rdata' %in% names(res)) {
    device_rdata[[key]] <- modifyList(device_rdata[[key]], res$rdata)
  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # if we're closing the device, also delete the data
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (device_call == 'close' && getOption("DEVOUT_DELETE_DATA_ON_CLOSE", TRUE)) {
    rm(list = key, envir = device_rdata)
  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # If the device call can take a return value, check that the return value
  # given by the user (if at all) is of the right type and length
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  res <- sanitize_return_types(device_call, res)


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # If the device description has changed from what C++ gave to R, then we
  # should validate it before passing it back from R to C++.
  # This avoids the situation where the user has corrupted the device description
  # and then having C++ code somehow deal with the error. Throw an error early in R!
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!is.null(res$dd) && !identical(res$dd, state$dd)) {
    res$dd <- sanitize_device_description(res$dd)
  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # We **MUST** pass a list back to C++.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (is.list(res)) {
    res
  } else {
    list()
  }
}
