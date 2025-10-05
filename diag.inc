#ifndef DIAG
/* Define DIAG(ID, KIND, MSG) before including to generate diagnostics */
#define DIAG(ID, KIND, MSG)
#endif

DIAG(err_unknown_char, Error, "unknown char '{0}'")

#undef DIAG