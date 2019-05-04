
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
