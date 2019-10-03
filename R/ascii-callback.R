

`%||%` <- function (x, y) {
  if (is.null(x)) y else x
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#Naively converting a colour to grey level = 0.3R + 0.59G + 0.11B
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
col2grey <- function(col) {
  (0.3 * col[1] + 0.59 * col[2] + 0.11 * col[3])/255
}


grey2char <- function(grey) {
  if (grey < 0) grey <- 0
  if (grey > 1) grey <- 1
  char_lookup <- c('#', '*', 'O', 'X', 'x', 'o', 'a', '-', '+', '~', ':', '.', ' ')
  char_lookup[as.integer(grey * (length(char_lookup) - 1) + 1)]
}


col2char <- function(col) {
  grey2char(col2grey(col))
}




#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Compact/abbreviated dumping of a list to screen
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dump_list <- function(ll) {

  ll <- lapply(ll, FUN = function(l) {
    if (!is.null(dim(l))) {
      paste(nrow(l), "x", ncol(l), class(l[1]), class(l))
    } else if (length(l) > 1 && length(l) <= 4) {
      deparse(l)
    } else if (length(l) > 4) {
      paste(deparse(length(l)), class(l[1]), class(l))
    } else {
      as.character(l)
    }
  })

  print(unlist(ll))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set_pixel <- function(state, x, y, char) {
  # TODO: check if x, y are within clipping

  if (is.null(x) || is.null(y) || is.na(x) || is.na(y) || is.null(char) || is.na(char)) {
    message("set_pixel: NULL or NA value: ", x, " - ", char)
    return(state)
  }


  x <- as.integer(round(x))
  y <- as.integer(round(y))


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Out of bounds
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (x < state$dd$clipLeft/72 - 0.5 || x > state$dd$clipRight/72 + 0.5 ||
      y < state$dd$clipBottom/72 - 0.5|| y > state$dd$clipTop/72 + 0.5) {

    # cat("clip reject - (", state$dd$clipLeft/72, state$dd$clipRight/72, x, "), (", state$dd$clipBottom/72, state$dd$clipTop/72, y, ")\n")

    return(state)
  }


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Be extra paranoid about plotting extents
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  x <- max(x, 1)
  x <- min(x, state$rdata$width)

  y <- max(y, 1)
  y <- min(y, state$rdata$height)


  # message("set_pixel: ", x, ", ", y)
  state$rdata$canvas[y, x] <- char

  state
}





#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
set_multiple_pixels <- function(state, x, y, char) {

  if (is.null(x) || is.null(y) || anyNA(x) || anyNA(y) || is.null(char) || is.na(char)) {
    message("set_muliple_pixels: NULL or NA value. With char = ", char)
    return(state)
  }


  x <- as.integer(round(x))
  y <- as.integer(round(y))




  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Out of bounds
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  out_of_bounds <-
    x < state$dd$clipLeft  /72 - 0.5 |
    x > state$dd$clipRight /72 + 0.5 |
    y < state$dd$clipBottom/72 - 0.5 |
    y > state$dd$clipTop   /72 + 0.5


  x <- x[!out_of_bounds]
  y <- y[!out_of_bounds]


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Be extra paranoid about plotting extents
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  x <- pmax(x, 1)
  x <- pmin(x, state$rdata$width)

  y <- pmax(y, 1)
  y <- pmin(y, state$rdata$height)


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Set all values
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  idx <- cbind(y, x)
  state$rdata$canvas[idx] <- char

  state
}




#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Super dodgy naive line-drawing algorithm. Vectorised.
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
draw_line <- function(state, x1, y1,  x2,  y2, char, scale = TRUE) {

  if (is.null(x1) || is.null(x2) || is.null(y1) || is.null(y2) || is.null(char)) {
    message("GOT A NULL IN DRAW_LINE)")
    return(state)
  }

  if (any(is.na(c(x1, y2, x2, y2, char)))) {
    message("GOT NA IN DRAW_LINE)")
    return(state)
  }

  if (scale) {
    x1 <- x1/72
    y1 <- y1/72
    x2 <- x2/72
    y2 <- y2/72
  }



  x1 <- round(x1)
  x2 <- round(x2)
  y1 <- round(y1)
  y2 <- round(y2)

  xdelta <- abs(x2 - x1)
  ydelta <- abs(y2 - y1)

  if (xdelta > ydelta) {
    x <- x1:x2
    y <- seq(y1, y2, length.out = length(x))
  } else {
    y <- y1:y2
    x <- seq(x1, x2, length.out = length(y))
  }


  state <- set_multiple_pixels(state, x, y, char)


  state
}




#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# When the device is opened
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_open <- function(args, state) {

  width <- state$rdata$width
  if (is.null(width)) {
    width  <- getOption('width', default = 80)
  }

  height <- state$rdata$height
  if (is.null(height)) {
    height <- as.integer(width * 0.5 * 0.5)
  }

  state$rdata$width  <- width
  state$rdata$height <- height
  state$dd$right       <- width   * 72
  state$dd$bottom      <- height  * 72
  state$dd$clipRight   <- width   * 72
  state$dd$clipBottom  <- height  * 72


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Initialise the canvas on which to draw using the 'dd->startfill' colour
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  startfill <- state$dd$startfill
  bg <- col2char(startfill)
  state$rdata$canvas <- matrix(bg, nrow = height, ncol = width)



  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # ASCII has different "inches per pixel" in the x and y direction.
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  font_aspect <- state$rdata$font_aspect %||% 0.45
  state$dd$ipr <- c(1/72, 1/72/font_aspect)
  state$dd$hasTextUTF8    <- TRUE  # The macOS terminal will let us use utf8
  state$dd$wantSymbolUTF8 <- TRUE



  state
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# When the device is closed
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_close <- function(args, state) {

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Get the canvas we've been drawing on
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  canvas <- state$rdata$canvas

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Output a collapsed character matrix to screen
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (!is.null(state$rdata$filename)) {
    sink(state$rdata$filename)
  }
  cat(paste(apply(t(canvas), 2, paste0, collapse = ""), collapse = "\n"), "\n")

  if (!is.null(state$rdata$filename)) {
    sink()
  }


  state
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_line <- function(args, state) {

  state <- draw_line(state, args$x1, args$y1,  args$x2,  args$y2, col2char(state$gc$col))

  state
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_rect <- function(args, state) {

  col  <- col2char(state$gc$col)
  fill <- col2char(state$gc$fill)


  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Fill. Currently ignoring clip box.
  # TODO: clipping
  # TODO: transpararent fill
  # TODO: alpha
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (state$gc$fill[4] > 0) {
    xmin <- round(min(c(args$x0, args$x1))/72)
    xmax <- round(max(c(args$x0, args$x1))/72)
    ymin <- round(min(c(args$y0, args$y1))/72)
    ymax <- round(max(c(args$y0, args$y1))/72)

    xmin <- max(1, xmin)
    ymin <- max(1, ymin)
    xmax <- min(state$rdata$width, xmax)
    ymax <- min(state$rdata$height, ymax)

    state$rdata$canvas[ymin:ymax, xmin:xmax] <- fill
  }

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Border - currently drawing rectangular outline using same characters
  # regardless of colour (unless it's blank or totally transparent)
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  if (state$gc$col[4] > 0 && col != ' ') {
    state <- draw_line(state, args$x0, args$y0,  args$x1,  args$y0, '-')
    state <- draw_line(state, args$x1, args$y0,  args$x1,  args$y1, '|')
    state <- draw_line(state, args$x1, args$y1,  args$x0,  args$y1, '-')
    state <- draw_line(state, args$x0, args$y1,  args$x0,  args$y0, '|')

    state <- set_pixel(state, args$x0/72, args$y0/72, '+')
    state <- set_pixel(state, args$x0/72, args$y1/72, '+')
    state <- set_pixel(state, args$x1/72, args$y1/72, '+')
    state <- set_pixel(state, args$x1/72, args$y0/72, '+')
  }



  state
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_polyline <- function(args, state) {

  char <- col2char(state$gc$col)

  for (i in seq(args$n - 1)) {
    state <- draw_line(state, args$x[i], args$y[i],  args$x[i+1],  args$y[i+1], char)
  }

  state
}




#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# No filled polygons (yet!)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_polygon <- function(args, state) {

  char <- col2char(state$gc$col)

  n <- args$n

  for (i in seq(n - 1)) {
    state <- draw_line(state, args$x[i], args$y[i],  args$x[i+1],  args$y[i+1], char)
  }


  state <- draw_line(state, args$x[n], args$y[n],  args$x[1],  args$y[1], char)

  state
}




#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Draw multiple paths
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_path <- function(args, state) {

  char <- col2char(state$gc$col)

  x <- args$x
  y <- args$y

  # print(length(x))

  # ind <- 1L
  # for (i in seq_len(args$npoly)) {
  #   # Move to the first point of the sub-path
  #   state <- set_pixel(state, x[ind]/72, y[ind]/72, char)
  #   ind <- ind + 1L
  #   # Draw the sub-path
  #   for (j in seq_len(args$nper[i] - 1)) {
  #     state <- set_pixel(state, x[ind]/72, y[ind]/72, char);
  #     ind <- ind + 1L;
  #   }
  # }

  # cat("ascii_path: ", length(x), "\n")
  state <- set_multiple_pixels(state, x/72, y/72, char)

  state
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Calculate and return the string width in the current state
#
# device_StrWidth should return the width of the given
# string in DEVICE units.
#
# graphics parameters that should be honoured (if possible):
#   font, cex, ps
#
# @param str string
#
# @return Optionally return 'width' the display width of the string in device units (numeric).
#         If not returned then a default value is used i.e. (strlen(str) + 2) * 72
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_strWidth <- function(args, state) {

  state$width <- (nchar(args$str) + 1) * 72

  state
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_metricInfo <- function(args, state) {

  state$ascent  <- 0.5  * 72
  state$descent <- 0.7 * 72

  state
}





#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# ref: http://members.chello.at/~easyfilter/bresenham.html
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_circle <- function(args, state) {

  # message("circle")
  # dump_list(args)
  col_char <- col2char(state$gc$col)
  is_col_transparent <- state$gc$col[4] == 0

  fill_char <- col2char(state$gc$fill)
  is_fill_transparent <- state$gc$fill[4] == 0

  # cat("col : ", state$gc$col , "\n")
  # cat("fill: ", state$gc$fill, "\n")

  r <- ceiling(args$r/2)
  xc <- args$x / 72
  yc <- args$y / 72

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Aspect ratio of terminal font for display
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  font_aspect <- state$rdata$font_aspect %||% 0.45

  if (r == 1) {
    state <- set_pixel(state, xc, yc, col_char)
    return(state)
  }

  x <- -r
  y <- 0
  err <- 2 - 2*r
  while (x < 0) {

    # Fill the circle centre
    if (!is_fill_transparent) {
      state <- draw_line(state, xc + x, yc + y * font_aspect,  xc - x,  yc + y * font_aspect, fill_char, scale = FALSE)
      state <- draw_line(state, xc + x, yc - y * font_aspect,  xc - x,  yc - y * font_aspect, fill_char, scale = FALSE)
    }

    # draw the border
    if (!is_col_transparent) {
      state <- set_pixel(state, xc - x, yc + y * font_aspect, col_char);   #    I. Quadrant
      state <- set_pixel(state, xc - y, yc - x * font_aspect, col_char);   #   II. Quadrant
      state <- set_pixel(state, xc + x, yc - y * font_aspect, col_char);   #  III. Quadrant
      state <- set_pixel(state, xc + y, yc + x * font_aspect, col_char);   #   IV. Quadrant
    }


    r <- err
    if (r <= y) {
      y   <- y + 1
      err <- err + y * 2 + 1
    }
    if (r > x || err > y) {
      x <- x + 1
      err <- err + x * 2 + 1
    }
  }



  state
}





#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Text
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_text <- function(args, state) {
  x <- round(args$x/72)
  y <- round(args$y/72)

  n   <- nchar(args$str)
  n2  <- floor(n/2)
  str <- strsplit(args$str, '')[[1]]

  if (args$rot == 0) {
    for (i in seq_along(str)) {
      char <- str[i]
      state <- set_pixel(state, x + i - 1, y, char)
    }
  } else {for (i in seq_along(str)) {
    char <- str[i]
    state <- set_pixel(state, x, y + i - 1 - n2 , char)
  }
  }

  state
}






#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' ASCII callback for the rdevice
#'
#' @param device_call name of device function call
#' @param args arguments to device function call
#' @param state list of rdata, dd and gc. Some or all of which may be NULL
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ascii_callback <- function(device_call, args, state) {

  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # Anything we're not handling, just return() straight away
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  # print(state$name)
  state <- switch(
    device_call,
    "open"         = ascii_open      (args, state),
    "close"        = ascii_close     (args, state),
    "line"         = ascii_line      (args, state),
    "polyline"     = ascii_polyline  (args, state),
    "circle"       = ascii_circle    (args, state),
    "rect"         = ascii_rect      (args, state),
    "text"         = ascii_text      (args, state),
    'strWidth'     = ascii_strWidth  (args, state),
    "textUTF8"     = ascii_text      (args, state),
    'strWidthUTF8' = ascii_strWidth  (args, state),
    'polygon'      = ascii_polygon   (args, state),
    'metricInfo'   = ascii_metricInfo(args, state),
    'path'         = ascii_path      (args, state),
    'circle'       = ascii_circle    (args, state),
    {
      # if (!device_call %in% c('strWidth', 'size', 'clip', 'mode', 'metricInfo')) {
      #   print(device_call);
      # }
      state
    }
  )

  state
}


if (FALSE) {
  # rdevice('ascii_callback', list(x=1)); plot(1); dev.off()

  library(devout)
  library(ggplot2)

  rdevice('ascii_callback', font_aspect = 0.45);
  p <- ggplot(mtcars) +
    geom_point(aes(mpg, wt, size = cyl)) +
    theme_bw() +
    labs(title = "hello mike", subtitle = "subtitle")
  print(p)

  dev.off()

}















