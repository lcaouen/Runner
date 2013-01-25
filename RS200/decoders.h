/* ------------------------------------------------------------------------------------------------------
   decoders.h
--------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "convert_functions.h"
#include "data_structures.h"
#include "display_functions.h"

#ifndef DECODERS_H
#define DECODERS_H

extern session_data extract_session_data(int *, int);
extern followup_data extract_followup_data(int *, int);
extern void free_session_data(session_data);
extern void free_followup_data(followup_data);
extern char* decode(char* in_strPath);

#endif

