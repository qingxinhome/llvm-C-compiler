#ifndef DIAG
/* Define DIAG(ID, KIND, MSG) before including to generate diagnostics */
#define DIAG(ID, KIND, MSG)
#endif

/// lexer
DIAG(err_unknown_char, Error, "unknown char '{0}'")

/// parser
DIAG(err_expected, Error, "expected '{0}', but found '{1}'")
DIAG(err_break_stmt, Error, "'break' statement not in loop or switch statement")
DIAG(err_continue_stmt, Error, "'continue' statement not in loop or switch statement")

/// semantic
DIAG(err_redefined, Error, "redefined symbol '{0}'")
DIAG(err_undefined, Error, "undefined symbol '{0}'")
DIAG(err_lvalue, Error, "required lvalue on the assignment operation left side")
DIAG(err_type, Error, "typename expected")

#undef DIAG