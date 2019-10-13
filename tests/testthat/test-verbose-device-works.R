

test_that("'verbose' device works", {
  tf <- tempfile()
  sink(tf)
  devout::verbose()
  plot(1:10)
  invisible(dev.off())
  sink()
  expect_true(TRUE)
})
