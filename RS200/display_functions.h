/* ------------------------------------------------------------------------------------------------------
   display_functions.h
--------------------------------------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "convert_functions.h"
#include "data_structures.h"

#ifndef DISPLAY_FUNCTIONS_H
#define DISPLAY_FUNCTIONS_H

extern void plotheader(int, char *);
extern void plotdata(int, char *, char*, ...);
extern void plotpacedata(int, char *, int , int );
extern void plotzonedata(int, char *, int , char *, int );
extern void plotzonestringdata(int, char *, int , char *, char *);
extern void xmlstarttag(char *, int, char *, char *, ...);
extern void xmlendtag(char *, int);
extern void xmlvaluetag(char *, int , char *, ...);
extern void update_status(char *);
extern void print_debug(char *, ...);
extern void print_deep_debug(char *, ...);
extern void update_level(double);
extern void update_progress(double);
extern void fail(char *);
extern void set_debug(char);
extern char get_debug();
extern void set_deep_debug(char);
extern char get_deep_debug();
extern void set_output_stream(FILE *);
extern FILE *get_output_stream();

#endif

