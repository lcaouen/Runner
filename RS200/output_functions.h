/* ------------------------------------------------------------------------------------------------------
   output_functions.h
--------------------------------------------------------------------------------------------------------- */


#include <string.h>
#include <stdlib.h>
#include "data_structures.h"

#ifndef OUTPUT_FUNCTIONS_H
#define OUTPUT_FUNCTIONS_H

#define OUTPUT_NORMAL 0
#define OUTPUT_CSV 1
#define OUTPUT_CSV_PRETTY 2

extern void session_summary(session_data);
extern void followup_summary(followup_data);
extern void session_as_csv(session_data);
extern void session_as_xml(session_data);
extern void raw_data(int *,int);
extern void raw_binary_data(int *,int);

#endif

