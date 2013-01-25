/* ------------------------------------------------------------------------------------------------------
   signal_functions.h
--------------------------------------------------------------------------------------------------------- */

#ifndef SIGNAL_FUNCTIONS_H
#define SIGNAL_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display_functions.h"
#include "data_structures.h"
#include "audio_api_windows.h"


extern double filter(double);
extern double dilate(double);
extern double median(double);
extern double make_decision(double,int *);
extern void b_median(int *);
extern int byte_decode(int, int);
extern void set_input_file_name(TCHAR *);
extern TCHAR *get_input_file_name();
// extern void set_input_file_name(char *);
// extern char *get_input_file_name();
extern void set_dsp_device(char *);
extern char *get_dsp_device();
extern void set_read_from_dsp(char);
extern char get_read_from_dsp();

#endif
