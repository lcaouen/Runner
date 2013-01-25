/* ------------------------------------------------------------------------------------------------------
   convert_functions.h
--------------------------------------------------------------------------------------------------------- */

#ifndef CONVERT_FUNCTIONS_H
#define CONVERT_FUNCTIONS_H

#include <stdio.h>

#define PACE_FORMAT_MIN_PER_KM 0
#define PACE_FORMAT_SEC_PER_KM 1
#define PACE_FORMAT_KM_PER_HOUR 2

#define TIME_FORMAT_BCD 0
#define TIME_FORMAT_BITWISE 1

#define TRUE 1
#define FALSE 0

extern int msb_lsb_to_decimal(int, int);
extern int bcd_to_decimal(int);
extern int hours_in_sec_per_km(int);
extern int minutes_in_sec_per_km(int);
extern int seconds_in_sec_per_km(int);
extern int hours_in_seconds(double seconds);
extern int minutes_in_seconds(double);
extern double seconds_in_seconds(double);
extern int time_to_hours(int);
extern int time_to_minutes(int);
extern int time_to_seconds(int);
extern int time_to_tenths(int, int);
extern void time_to_string(char*, double);
extern double time_to_total_seconds(int, int, int, int);
extern void laptime_to_string(char*, int, int, int, int, int, int, int);
extern double laptime_to_seconds(int, int, int, int, int, int);
extern void byte_to_binary_str(char*, char);
extern char is_autolap(int);
extern void substr(char*, char*, int, int);

#endif
