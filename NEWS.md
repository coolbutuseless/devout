
# devout 0.2.8 2020-04-20

* The name of the device can now be set by passing `device_name` 
  argument to `rdevice()`

# devout 0.2.7 2020-04-19

* Check validity of `rfunction` on initial function call to `rdevice()` and store
  as part of `rdata` data structure.

# devout 0.2.6 2020-04-19

* Removed more caching/copying that is no longer needed now that `rdata` is 
  an environment
* `rdevice()` now accepts either a function object or character string as
  the callback function. The character string argument is soft-deprecated, 
  and passing in an actual function object is the preferred method.
* The initial call from Rcpp back into R (into `devout::rcallback()`) is now 
  done differently. This fixes some namespace issues when using devout 
  from within another package.

# devout 0.2.5 2020-04-18

* Removed some caching/copying that is no longer needed now that `rdata` is 
  an environment
* Removed the option 'DEVOUT_KEEP_RDATA'. If you need to access the cached
  `rdata` for a current device, just view it in `devout:::device_rdata`
  any time after opening the device and before calling `dev.off()`

# devout 0.2.4 2020-04-18

* Internal `rdata` data structure is now an environment rather than a list.
* Change default sizing from 400x300 (pixels) to 10x8 (inches)
* Refactored the advanced debug mechanism for `rdata`.  This was used often during 
  initial package development, but is less used now.  To enable, set
  `options(DEVOUT_KEEP_RDATA = TRUE)`.

# devout 0.2.3

* Using `new/delete` rather than `calloc/free` to address a segmentation fault on windows.

# devout 0.2.2

* More verbose error message for bad return value

# devout 0.2.1

* Update `devinfo$device_call` to include `rect` which was erroneously omitted in 
  prior release.

# devout 0.2.0

* Major internal refactor
* Added the `rdevice` which allows the user to write devices using just R
* Re-wrote `ascii` device to use the underlying `rdevice`

# devout 0.1.3

* Inclusion of `rdevice()` such that graphics devices can be written in R


# devout 0.1.2

* Added support for multiple page output
    * If output to console, pages will be output one after another
    * If output to file and filename is a format string (e.g. "output%03i.txt") then
      unique files will be generated for each page
    * If output to file and filename is a NOT a format string (e.g. "output.txt") then
      only the last page will be saved to file
      

# devout 0.1.1

* Added support for path objects, so `tmap` output now works.


# devout 0.1.0

* Initial release includes alpha versions of `ascii()` and `descriptive()` devices.
