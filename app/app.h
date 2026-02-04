#ifndef APP_H
#define APP_H

#include "sheet.h"
#include "eval.h"

const char *error_to_string (ErrorCode e);
ErrorCode map_calc_error (erors e);
ErrorCode evaluate_expression_to_value(sheet *sh , const char *expr, double *out_value);
ErrorCode set_cell_from_input(sheet *sh, const char *target_addr, const char *input_line);
void recalc_all (sheet *sh);

#endif