/* ------------------------------------------------------------------------------------------------------
   display_functions.c

   This file contains helper functions for printing out information during
   program execution.
--------------------------------------------------------------------------------------------------------- */

#include "stdafx.h"
#include <stdarg.h>
#include "display_functions.h"


static boolean debug = FALSE;
static boolean deep_debug = FALSE;
static FILE *output_stream = NULL;

volatile char  *v_status   = "";
volatile double v_level    = 0.0;
volatile double v_progress = 0.0;

/* ------------------------------------------------------------------------------------------------------
   Prints out a data header in standard format
--------------------------------------------------------------------------------------------------------- */
void plotheader(int format, char *header_text)
{
	if (output_stream != NULL) {
		switch (format) {
			case OUTPUT_NORMAL:
				fprintf(output_stream,"\n--------------- %s -------------\n",header_text);
				break;
			case OUTPUT_CSV_PRETTY:
				fprintf(output_stream,"#%s\n",header_text);
				break;
		}
	}
}


/* ------------------------------------------------------------------------------------------------------
   Prints out a data field
--------------------------------------------------------------------------------------------------------- */
void plotdata(int format, char *title, char *value_formats, ...)
{
	va_list argp;
	
	if (output_stream != NULL) {
		va_start(argp, value_formats);
		switch (format) {
			case OUTPUT_NORMAL:
				fprintf(output_stream,"%-27s: ",title);
				vfprintf(output_stream, value_formats, argp);
				fprintf(output_stream,"\n");
				break;
			case OUTPUT_CSV:
				vfprintf(output_stream, value_formats, argp);
				fprintf(output_stream, ",%s\n", title);
				break;
			case OUTPUT_CSV_PRETTY:
				fprintf(output_stream,"%-27s,",title);
				vfprintf(output_stream, value_formats, argp);
				fprintf(output_stream,"\n");
				break;
		}
		va_end(argp);
	}
}


/* ------------------------------------------------------------------------------------------------------
   Prints out pace data given as string (such as pace in mm:ss / km)
--------------------------------------------------------------------------------------------------------- */
void plotpacedata(int display_format, char *title, int pace, int number_format)
{
	char output_string[40] = "";
	switch (number_format) {
		case PACE_FORMAT_MIN_PER_KM:
			sprintf(output_string,"%.2d:%.2d", minutes_in_sec_per_km(pace/10), seconds_in_sec_per_km(pace/10));
			break;
		case PACE_FORMAT_SEC_PER_KM:
			sprintf(output_string,"%d", pace/10);
			break;
		case PACE_FORMAT_KM_PER_HOUR:
			sprintf(output_string,"%.3f", 3600/((double)pace/10));
			break;
	}
	if (output_stream != NULL) {
		switch (display_format) {
			case OUTPUT_NORMAL:
				fprintf(output_stream,"%-27s: %s\n", title, output_string);
				break;
			case OUTPUT_CSV:
				fprintf(output_stream,"%s,%s\n", output_string, title);
				break;
			case OUTPUT_CSV_PRETTY:
				fprintf(output_stream,"%-27s,%s\n", title, output_string);
				break;
		}
	}
}


/* ------------------------------------------------------------------------------------------------------
   Prints out HR zone data given as number (such as zone limit)
--------------------------------------------------------------------------------------------------------- */
void plotzonedata(int format, char *zone_name, int zone_number, char *title, int value)
{
	char str[50];
	if (output_stream != NULL) {
		sprintf(str, "%s %d %s", zone_name, zone_number, title);
		switch (format) {
			case OUTPUT_NORMAL:
				fprintf(output_stream,"%-27s: %d\n", str, value);
				break;
			case OUTPUT_CSV:
				fprintf(output_stream,"%d,%s %s\n", value, zone_name, title);
				break;
			case OUTPUT_CSV_PRETTY:
				fprintf(output_stream,"%-27s,%d\n", str, value);
				break;
		}
	}
}


/* ------------------------------------------------------------------------------------------------------
   Prints out HR zone data given as string (such as time in format hh:mm:ss)
--------------------------------------------------------------------------------------------------------- */
void plotzonestringdata(int format, char *zone_name, int zone_number, char *title, char *value)
{
	char str[50];
	if (output_stream != NULL) {
		sprintf(str, "%s %d %s", zone_name, zone_number, title);
		switch (format) {
			case OUTPUT_NORMAL:
				fprintf(output_stream,"%-27s: %s\n", str, value);
				break;
			case OUTPUT_CSV:
				fprintf(output_stream,"%s,%s %s value\n", value, zone_name, title);
				break;
			case OUTPUT_CSV_PRETTY:
				fprintf(output_stream,"%-27s,%s\n", str, value);
				break;
		}
	}
}

/*
------------------------------------------------------------------------------------------------------
   XML output functions
---------------------------------------------------------------------------------------------------------
*/
void xmlstarttag(char *tagname, int indent, char *param, char *paramformatstr, ...)
{
	int t;
	va_list argp;
	
	if (output_stream != NULL) {
		for (t=0; t < indent; t++){
			fprintf(output_stream,"    ");
		}
		va_start(argp, paramformatstr);

		if (param != NULL && paramformatstr != NULL) {
			fprintf(output_stream,"<%s %s=\"",tagname,param);
			vfprintf(output_stream,paramformatstr,argp);
			fprintf(output_stream,"\">\n");
		} else {
			fprintf(output_stream,"<%s>\n",tagname);
		}
		va_end(argp);
	}
}


void xmlendtag(char *tagname, int indent)
{
	int t;
	if (output_stream != NULL) {
		for (t=0; t < indent; t++){
			fprintf(output_stream,"    ");
		}
		fprintf(output_stream,"</%s>\n",tagname);
	}
}


void xmlvaluetag(char *tagname, int indent, char *valueformatstr, ...)
{
	int t;
	va_list argp;
	
	if (output_stream != NULL) {
		for (t=0; t < indent; t++){
			fprintf(output_stream,"    ");
		}
		va_start(argp, valueformatstr);
		fprintf(output_stream,"<%s>",tagname);
		vfprintf(output_stream,valueformatstr,argp);
		fprintf(output_stream,"</%s>\n",tagname);
	}
}


/* ------------------------------------------------------------------------------------------------------
   Prints debug messages during program execution
--------------------------------------------------------------------------------------------------------- */
void print_debug(char *s, ...)
{
	va_list argp;
	if (debug) {
		fprintf(stderr, "\nDebug: ");
		va_start(argp, s);
		vfprintf(stderr, s, argp);
		va_end(argp);
		fprintf(stderr, "\n");
	}
}

void print_deep_debug(char *s, ...)
{
	va_list argp;
	if (deep_debug) {
		va_start(argp, s);
		vfprintf(stdout, s, argp);
		va_end(argp);
	}
}

/* ------------------------------------------------------------------------------------------------------
   Updates the level field
--------------------------------------------------------------------------------------------------------- */
void update_level(double l)
{
	static double i_level = 0.0;
	static int c = 0;

	if(l > i_level)
		i_level = l;
	if(++c >= 4410) { // update level every 0.1 seconds (sampling frequency = 44100)
		v_level = i_level;
		i_level = 0.0;
		c = 0;
  	}
}


/* ------------------------------------------------------------------------------------------------------
   Updates the progress field
--------------------------------------------------------------------------------------------------------- */
void update_progress(double p)
{
	v_progress = p;
}


/* ------------------------------------------------------------------------------------------------------
   Updates the status field
--------------------------------------------------------------------------------------------------------- */
void update_status(char *s)
{
	v_status = s;
}


/* ------------------------------------------------------------------------------------------------------
   fatal error handling
--------------------------------------------------------------------------------------------------------- */
void fail(char *message)
{
	fprintf(stderr,"\n%s\n",message);
}


/* ------------------------------------------------------------------------------------------------------
   Getter and setter functions for variables related to output modes
--------------------------------------------------------------------------------------------------------- */
void set_debug(boolean value)
{
	debug = value;
}

void set_deep_debug(boolean value)
{
	deep_debug = value;
}

boolean get_debug()
{
	return debug;
}

boolean get_deep_debug()
{
	return deep_debug;
}

void set_output_stream(FILE *value)
{
	output_stream = value;
}

FILE * get_output_stream()
{
	return output_stream;
}
