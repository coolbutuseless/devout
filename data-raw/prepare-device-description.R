

device_description <- readr::read_csv(
'name                  , type   , length, description                                                                       , default
left                   , numeric,      1, left raster coordinate                                                            , 0
top                    , numeric,      1, top raster coordinate                                                             , 0
right                  , numeric,      1, right raster coordinate                                                           , width * 72
bottom                 , numeric,      1, bottom raster coordinate                                                          , height * 72
clipLeft               , numeric,      1, rectangular clipping extents                                                      , 0
clipTop                , numeric,      1, rectangular clipping extents                                                      , 0
clipRight              , numeric,      1, rectangular clipping extents                                                      , width * 72
clipBottom             , numeric,      1, rectangular clipping extents                                                      , height * 72
xcharOffset            , numeric,      1, x character addressing offset - unused                                            , 0.49
yCharOffset            , numeric,      1, y character addressing offset                                                     , 0.3333
yLineBias              , numeric,      1, 1/2 interline space as frac of line height                                        , 0.2
ipr                    , numeric,      2, "Inches per raster c(x, y)",                                                        "c(1/72, 1/72)"
cra                    , numeric,      2, "Character size in rasters c(x, y)",                                                "c(0.9 * startps, 1.2 * startps)"
gamma                  , numeric,      1, Device Gamma Correction                                                           , 1
canClip                , logical,      1, Device-level clipping                                                             , TRUE
canHAdj                , integer,      1, "Can do at least some horiz adjust of text: 0 = none, 1 = {0, 0.5, 1}, 2 = [0, 1]", 0L
canChangeGamma         , logical,      1, can the gamma factor be modified?                                                 , FALSE
displayListOn          , logical,      1, toggle for initial display list status                                            , FALSE
haveTransparency       , integer,      1, "1 = no, 2 = yes",                                                                  2L
haveTransparentBg      , integer,      1, "1 = no, 2 = fully, 3 = semi",                                                      2L
haveRaster             , integer,      1, "1 = no, 2 = yes, 3 = except for missing values",                                   2L
haveCapture            , integer,      1, "1 = no, 2 = yes",                                                                  2L
haveLocator            , integer,      1, "1 = no, 2 = yes",                                                                  2L
startfill              , integer,      4, sets par(bg) and gpar(fill)                                                       , "c(255L, 255L, 255L, 255L)"
startcol               , integer,      4, "sets par(fg), par(col) and gpar(col)",                                             "c(0L, 0L, 0L, 255L)"
startps                , numeric,      1, initial pointsize                                                                 , 12
startlty               , integer,      1, initial linetype                                                                  , 0L
startfont              , integer,      1, initial font                                                                      , 1L
startgamma             , numeric,      1, initial gamma correction                                                          , 1
wantSymbolUTF8         , logical,      1, NFI                                                                               , TRUE
hasTextUTF8            , logical,      1, Device supports UTF8 text                                                         , TRUE
useRotatedTextInContour, logical,      1,  Is rotated text good enough to be preferable to Hershey in contour label         , TRUE
canGenMouseDown        , logical,      1, can the device generate mousedown events                                          , FALSE
canGenMouseMove        , logical,      1, can the device generate mousemove events                                          , FALSE
canGenMouseUp          , logical,      1, can the device generate mouseup events                                            , FALSE
canGenKeybd            , logical,      1, can the device generate keyboard events                                           , FALSE
canGenIdle             , logical,      1, can the device generate idle events                                               , FALSE
gettingEvent           , logical,      1, This is set while getGraphicsEvent is actively looking for events                 , FALSE
') %>% as.data.frame()


usethis::use_data(device_description, overwrite = TRUE, internal = FALSE)

