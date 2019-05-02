
<!-- README.md is generated from README.Rmd. Please edit that file -->

# devout - non-standard devices for graphical output from R

<!-- badges: start -->

![](https://img.shields.io/badge/cool-useless-green.svg)
![](https://img.shields.io/badge/Status-alpha-orange.svg)
![](https://img.shields.io/badge/Version-0.1.1-blue.svg)
<!-- badges: end -->

`devout` is a package for some of some non-standard devices for R.

A graphics device (e.g. `png()`, `pdf()`) is an interface to which R
passes plotting instructions. The graphics device then stores these in
the appropriate format for the given filetype.

However, there is no requirement that the output is saved to a file, nor
is there any requirement that the output is saved as a graphic.

## What’s in the box

  - `ascii()` - a non-standard device which outputs an ascii
    representation of the plot to a file, or to the console or terminal
  - `descriptive()` - a non-standard output device that describes what
    the output looks like

## News:

  - v0.1.0 - initial release
  - v0.1.1
      - Added support for path objects, so more map plots now work.
      - More example plots - `sf`, `pie`, `igraph`

## Installation

You can install from [GitHub](https://github.com/coolbutuseless/devout)
with:

``` r
# install.packages("remotes")
remotes::install_github("coolbutuseless/devout")
```

## `ascii()` device

The `ascii()` device draws an approximation of the graphics using ASCII
characters on the console (by default) or saved to a text file (if
specified).

### `ggplot2`: Basic scatterplot

``` r
p <- ggplot(mtcars) + 
  geom_point(aes(mpg, wt)) +
  labs(
    y     = "Car Weight",
    title    = "Basic scatter plot",
    subtitle = "Rendered with devout::ascii()"
  )  + 
  theme_bw()

ascii(width = 100)
p
jnk <- dev.off()
```

``` 
    Basic scatter plot                                                                              
    Rendered with devout::ascii()                                                                   
    +----------------------------------------------------------------------------------------------+
    |  .#       .       #.        .         .        .        .        .         .        .        |
    |  .        .        .        .         .        .        .        .         .        .        |
 C5 |..............................................................................................|
 a  |  .        .        .        .         .        .        .        .         .        .        |
 r  |..............................................................................................|
    |  .        .        .        .         .        .        .        .         .        .        |
 W  |.........................#....................................................................|
 e4 |  .        .  #     .        .      #  .        .        .        .         .        .        |
 i  |  .        .        .#       #         .        .        .        .         .        .        |
 g  |.................#..###.......#.#.#.#.........................................................|
 h  |  .        .        .  #     .         .    #   .      # .        .         .        .        |
 t  |.................................................#............................................|
  3 |  .        .        .        .       # .  # #   .        .        .         .        .        |
    |  .        .        .        .         .  #     .        .        .         .        .        |
    |............................................#.................................................|
    |  .        .        .        .         .        .#       .   #    .         .        #        |
  2 |..................................................................#...........................|
    |  .        .        .        .         .        .        .        .         .        .    #   |
    |  .        .        .        .         .        .        .        .         .#       .        |
    +----------------------------------------------------------------------------------------------+
     10                 15                20                 25                30                 3 
                                                  mpg                                               
```

### `ggplot2` Facetted Histogram with a `fill` aesthetic

``` r
p <- ggplot(mtcars) + 
  theme_bw()  +
  geom_histogram(aes(wt, fill = cyl), bins = 5, colour = 'black') +
  labs(title = "Facetted Histogram with 'fill' Aesthetic", 
    subtitle = "Rendered with devout::ascii()") + 
  facet_wrap(~cyl, labeller = label_both) +
  theme(panel.grid = element_blank())

ascii(width = 100)
p
jnk <- dev.off()
```

``` 
       Facetted Histogram with 'fill' Aesthetic                                                     
       Rendered with devout::ascii()                                                                
       +--------- cyl: 4 -----------+--------- cyl: 6 ------------+--------- cyl: 8 -----------+    
       +----------------------------+-----------------------------+----------------------------+    
  10.0 |                            |                             |           +----+           |    
       |                            |                             |           |----|           |    
       |                            |                             |           |----|           |    
       |                            |                             |           |----|           |    
   7.5 |                            |                             |           |----|           |    
 c     |+----+                      |                             |           |----|           |    
 o     ||----|                      |                             |           |----|                
 u     ||----|                      |                             |           |----|           cyl  
 n     ||----|                      |                             |           |----|           4    
 t 5.0 ||----|                      |                             |           |----|           6    
       ||----+-----+                |      +----+                 |           |----|           8    
       ||----|-----|                |      |oooo|                 |           |----|           +    
       ||----|-----|                |      |oooo+-----+           |           |----|           |    
   2.5 ||----|-----|                |      |oooo|ooooo|           |           |----|           |    
       ||----|-----|                |      |oooo|ooooo|           |           |----+----+      |    
       ||----|-----|                |      |oooo|ooooo|           |           |----|----|      |    
       ||----|-----|                |      |oooo|ooooo|           |     +-----|----|----+-----+|    
       ||----|-----|                |      |oooo|ooooo|           |     |-----|----|----|-----||    
   0.0 +----------------------------+-----------------------------+----------------------------+    
          2    3     4    5     6      2     3    4     5    6       2    3     4    5     6        
                                                 wt                                                 
```

### `geom_sf` map of the Gulf of Mexico

  - Example taken from the [r-spatial
    website](https://www.r-spatial.org/r/2018/10/25/ggplot2-sf.html)
  - This would probably look better with filled polygons, but they are
    not supported yet.

<!-- end list -->

``` r
library(ggplot2)
library(sf)
library(rnaturalearth)
world <- ne_countries(scale = "medium", returnclass = "sf")

world_points <- st_centroid(world)
world_points <- cbind(world, st_coordinates(st_centroid(world$geometry)))

ascii(width = 200)
ggplot(data = world) +
  geom_sf() +
  geom_text(data= world_points,aes(x=X, y=Y, label=name),
            color = "darkblue", fontface = "bold", check_overlap = FALSE) +
  annotate(geom = "text", x = -90, y = 26, label = "Gulf of Mexico", fontface = "italic", color = "grey22", size = 6) +
  coord_sf(xlim = c(-102.15, -74.12), ylim = c(7.65, 33.97), expand = FALSE) + 
  theme_bw()
jnk <- dev.off()
```

<img src = "man/figures/gulfofmexico.png">

### `tmap` world map

  - Thanks to [Barry Rowlingson](https://twitter.com/geospacedman) for
    pointing out that basic mapping didn’t work in v0.1.0 and posting an
    example

<!-- end list -->

``` r
library(tmap)
data(World)

ascii(verbosity = 0, width = 200)
tm_shape(World, projection = 'longlat') + 
  tm_polygons() +
  tm_layout("Long lat coordinates (WGS84)", inner.margins=c(0,0,.1,0))
dev.off()
```

<img src = "man/figures/tmap.png">

### `treemap`

``` r
library(treemap)

ascii(width = 100)
mtcars %>% 
  tibble::rownames_to_column() %>% 
  head(10) %>% 
  treemap(index = 'rowname', vSize = 'disp', palette = c('#ffffff'))
jnk <- dev.off()
```

    +---------------------------------+----------------------+---------------------+-------------------+
    |                                 |                      |                     |                   |
    |                                 |                      |                     |                   |
    |                                 |                      |                     |                   |
    |                                 |                      |                     |                   |
    |                                 |                      |     Merc 280        |  Mazda RX4 Wag    |
    |          Duster 360             |                      |                     |                   |
    |                                 |   Hornet 4 Drive     |                     |                   |
    |                                 |                      |                     |                   |
    |                                 |                      +---------------------++------------------+
    |                                 |                      |                      |                  |
    |                                 |                      |                      |                  |
    +---------------------------------+                      |                      |                  |
    |                                 +----------------------+      Mazda RX4       |                  |
    |                                 |                      |                      |    Merc 230      |
    |                                 |                      |                      |                  |
    |                                 |                      |                      |                  |
    |                                 |                      |----------------------+                  |
    |                                 |                      |                      |------------------+
    |       Hornet Sportabout         |       Valiant        |                      |                  |
    |                                 |                      |                      |                  |
    |                                 |                      |      Merc 240D       |   Datsun 710     |
    |                                 |                      |                      |                  |
    |                                 |                      |                      |                  |
    +---------------------------------+----------------------+----------------------+------------------+

### `igraph` network plot

``` r
library(igraph)

ascii(width = 100)
igraph::random.graph.game(6, 0.4) %>% 
  plot(vertex.shape = 'none')
jnk <- dev.off()
```

``` 
                                                                                                    
                           1                                                                        
                             ++                                                                     
                               ++                                                                   
                                 ++                                                                 
                                   ++                                 3                             
                                     ++                              +                              
                                       +                           ++                               
                                        5  +++                   ++                                 
                                          +   ++++            +++                                   
                                           +      +++++     ++                                      
                                           +            6 ++                                        
                                            +            +                                          
                                             +         + +                                          
                                             +       ++  +                                          
                                              +     +    +                                          
                                               +  ++     +                                          
                                               2 +       +                                          
                                                 +        +                                         
                                                  ++      +                                         
                                                    ++    +                                         
                                                      ++  +                                         
                                                        +                                           
                                                         4                                          
                                                                                                    
```

### `pie` plot in base R

``` r
ascii(width = 100) 
pie(1:3)
jnk <- dev.off()
```

``` 
                                                                                                    
                                                                                                    
                                                                                                    
                                     2     ##############                                           
                                       ####              ###                                        
                                     ##                   # ###                                     
                                   ##                    #     #                                    
                                  #                     #       ## 1                                
                                 #                     #           #                                
                                #                    ##            #                                
                               #                    #               #                               
                               #                   #                #                               
                               ######################################                               
                               #                                    #                               
                                #                                  #                                
                                #                                  #                                
                                 #                                                                  
                                  #                             ##                                  
                                   ##                          #                                    
                                     ##                     ###                                     
                                       ####              ###                                        
                                           ##############                                           
                                                3                                                   
                                                                                                    
                                                                                                    
```

## `descriptive()` device

The descriptive device gives a blow-by-blow account of what the graphics
device is being asked to draw.

This device is useful to use as a starting point for creating other
graphics devices - I used it as the basis for the `ascii()` device and
added featues as I needed them.

``` r
p <- ggplot(mtcars) + 
  geom_point(aes(mpg, wt)) +
  labs(title = "Rendered with devout::ascii() more text goes here") +
  theme_void()

descriptive()
p
```

    - new page
    - circle      ( 182.2,  216.8)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 182.2,  200.5)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 210.1,  236.0)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 188.4,  178.8)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 146.6,  164.5)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 137.3,  163.2)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      (  78.5,  156.2)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 234.8,  180.4)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 210.1,  183.0)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 154.4,  164.5)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 132.7,  164.5)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 111.0,  124.2)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 125.0,  145.9)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      (  92.5,  142.7)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      (  18.2,   48.9)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      (  18.2,   37.8)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      (  84.7,   42.8)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 358.6,  243.6)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 327.7,  281.0)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 381.8,  267.0)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 189.9,  226.7)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      (  97.1,  159.3)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      (  92.5,  164.8)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      (  63.1,  138.9)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 154.4,  138.6)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 279.7,  260.6)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 259.6,  247.5)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 327.7,  287.5)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 101.7,  181.7)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 162.1,  207.2)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      (  89.4,  156.2)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - circle      ( 188.4,  206.6)  radius: 1.95   col:#000000FF (#) fill:#000000FF (#) lwd:0.94 lty:0 cex:1
    - text        'Rendered with devout::ascii() more text goes here'   (   0.0,   13.3)  rot: 0 Hadj: 0   col:#000000FF (#) fill:#000000FF (#) lwd:1.00 lty:0 cex:1 fontface:1 fontsize:13.2 fontfamily: lineheight:0.9

``` r
jnk <- dev.off()
```

    - close

## Limitations

  - No filled polygons.
  - No anti-aliasing.
  - No UTF text support.
  - No plotmath support.
  - No angled text other than 0 or 90 degrees.
  - No raster support.
  - No sanity checking or safety checking of ‘filename’ string.
  - No way to have un-filled circles.
  - No stroke aesthetic for cicles.
  - No real support for legends.
      - Legends get drawn, but a colourscale (for example) doesn’t
        really map to an ascii representation so it looks rubbish.
      - You should probably always add `theme(legend.position = 'none')`
        .
  - No support for carriage returns in text.

## Other Non-Standard Device Ideas

  - ANSI graphics
  - Colour ASCII/ANSI output
  - audio + midi output
  - HPGL plotting output
  - [CNC](https://en.wikipedia.org/wiki/Numerical_control) machine
    tooling instructions
  - Directly drive motors to power an etch-a-sketch
  - Crochet and knitting patterns
  - Capture each call and turn it back into R code as a sequence of
    `grid` calls
  - Save drawing instructions as a CSV file

## References:

  - [svglite source code](https://github.com/r-lib/svglite)
  - [thomasp85’s](https://twitter.com/thomasp85) [devoid
    package](https://github.com/r-lib/devoid)
  - The defunct `RGraphicsDevice` project
      - <http://www.omegahat.net/RGraphicsDevice/>
      - <http://www.omegahat.net/RGraphicsDevice/overview.html>
