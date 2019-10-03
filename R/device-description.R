

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Sanitize the Device Description values
#'
#' @param dd device description list of values
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
sanitize_device_description <- function(dd) {
  if (is.null(dd) || length(dd) == 0L) {
    return(dd)
  }

  for (i in seq_len(nrow(devout::devinfo$dd))) {
    item <- devout::devinfo$dd[i, ]

    if (item$name %in% names(dd)) {
      value <- dd[[item$name]]
      if (is.null(value) || anyNA(value)) {
        warning("device description: '", item$name, "' can't be NULL or NA. Ingoring.")
        dd[[item$name]] <- NULL
        next
      }
      good  <- switch(item$type,
                      'numeric' = is.numeric(value),
                      'integer' = is.integer(value),
                      'logical' = is.logical(value),
                      stop("No such item$type: ", item$type)
      )

      if (!good || length(value) != item$length) {
        warning("device description: '", item$name, "' must be ", item$length, " ", item$type, " values, but found ", length(value), " ", typeof(value), " values. Ignoring.")
        dd[[item$name]] <- NULL
      }
    }
  }

  dd
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Get the default device decription used in the rdevice
#'
#' @param width,height dimensions of device
#' @param startps starting pointsize
#'
#' @return list of device description information
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
get_default_device_description <- function(width = 100, height = 100, startps = 12) {

  dd <- list()

  for (i in seq_len(nrow(devout::devinfo$dd))) {
    item <- devout::devinfo$dd[i, ]
    dd[[item$name]] <- eval(parse(text = item$default))
  }

  dd
}
