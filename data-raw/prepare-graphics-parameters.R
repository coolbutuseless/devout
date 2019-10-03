

suppressPackageStartupMessages({
  library(magrittr)
  library(dplyr)
  library(tidyr)
})

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Colours NOTE:  Alpha transparency included in col & fill
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
graphics_parameters <- readr::read_csv(
'group, name     , type   , type_info                          , description
colour, col      , integer, "length = 4, format = RGBA",         "pen colour (lines, text, borders, ...)"
colour, fill     , integer, "length = 4, format = RGBA",         "fill colour (for polygons, circles, rects, ...)"
colour, gamma    , double ,                                    ,  Gamma correction

line, lwd        , double ,                                    , "Line width (roughly number of pixels)"
line, lty        , integer,                                    , "Line type (solid, dashed, dotted, ...)"
line, lend       , integer,  "1 = Round, 2 = Butt, 3 = Square",   line end
line, ljoin      , integer,  "1 = Round, 2 = Mitre, 3 = Bevel",   line join
line, lmitre     , double ,                                    , line mitre

text, cex        , double ,                                    , Character expansion (font size = fontsize*cex)
text, ps         , double ,                                    , Font size in points
text, lineheight , double ,                                    , Line height (multiply by font size)
text, fontface   , intger ,                                    , "Font face (plain, italic, bold, ...)"
text, fontfamily , string ,                                    , Font family
') %>% as.data.frame()

graphics_parameters %<>% tidyr::replace_na(list(type_info = ""))
graphics_parameters

usethis::use_data(graphics_parameters, overwrite = TRUE, internal = FALSE)

