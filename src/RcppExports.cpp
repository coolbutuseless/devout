// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// rdevice_
bool rdevice_(SEXP rdata);
RcppExport SEXP _devout_rdevice_(SEXP rdataSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< SEXP >::type rdata(rdataSEXP);
    rcpp_result_gen = Rcpp::wrap(rdevice_(rdata));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_devout_rdevice_", (DL_FUNC) &_devout_rdevice_, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_devout(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
