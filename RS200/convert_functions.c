/* ------------------------------------------------------------------------------------------------------
   convert_functions.c

   This file contains functions for converting values given in various formats into
   typically human readable and comprehensible values.
--------------------------------------------------------------------------------------------------------- */


#include "stdafx.h"
#include "convert_functions.h"

/* ------------------------------------------------------------------------------------------------------
   Converts a number given as two bytes (high and low) into a single integer
--------------------------------------------------------------------------------------------------------- */
int msb_lsb_to_decimal(int msb, int lsb)
{
	return (lsb + (msb << 8));
}


/* ------------------------------------------------------------------------------------------------------
   Converts a binary coded decimal number into an integer
--------------------------------------------------------------------------------------------------------- */
int bcd_to_decimal(int bcd)
{
	return ((bcd >> 4)*10 + (bcd & 0x0F));
}

/* ------------------------------------------------------------------------------------------------------
   Returns number of hours in a value given in seconds
--------------------------------------------------------------------------------------------------------- */
int hours_in_sec_per_km(int sec_per_km)
{
	return ((int)sec_per_km/3600);
}


/* ------------------------------------------------------------------------------------------------------
   Returns number of minutes in a value given in seconds
--------------------------------------------------------------------------------------------------------- */
int minutes_in_sec_per_km(int sec_per_km)
{
	return ((int)sec_per_km/60);
}


/* ------------------------------------------------------------------------------------------------------
   Returns number of seconds in a value given in seconds 
   (remainder part only)
--------------------------------------------------------------------------------------------------------- */
int seconds_in_sec_per_km(int sec_per_km)
{
	return (sec_per_km % 60);
}

/* ------------------------------------------------------------------------------------------------------
   Returns number of hours in a value given in seconds
--------------------------------------------------------------------------------------------------------- */
int hours_in_seconds(double seconds)
{
	return ((int)seconds/3600);
}


/* ------------------------------------------------------------------------------------------------------
   Returns number of minutes in a value given in seconds
--------------------------------------------------------------------------------------------------------- */
int minutes_in_seconds(double seconds)
{
	return (((int)seconds - hours_in_seconds(seconds)*3600)/60);
}


/* ------------------------------------------------------------------------------------------------------
   Returns number of seconds in a value given in seconds 
   (with fraction if necessary)
--------------------------------------------------------------------------------------------------------- */
double seconds_in_seconds(double seconds)
{
	return (seconds - (double)hours_in_seconds(seconds)*3600 - (double)minutes_in_seconds(seconds)*60);
}


/* ------------------------------------------------------------------------------------------------------
   The following functions return time values as integers from time data encoded into
   three bytes as sent by RS200
--------------------------------------------------------------------------------------------------------- */
int time_to_hours(int hours)
{
	// rs200 marks the first bit as one if time data comes from autolap
	// as maximum exercise time is 100 hours, it i safe to assume that this first bit never
	// stores any other information. Here, information about autolap is simply
	// discarded
	hours = hours & 0x7F;
	return hours;
}

int time_to_minutes(int minutes)
{
	return (minutes & 0x3F);
}

int time_to_seconds(int seconds)
{
	return (seconds & 0x3F);
}

int time_to_tenths(int minutes, int seconds)
{
	return(4 * (minutes >> 6) + (seconds >> 6));
}


/* ------------------------------------------------------------------------------------------------------
   Returns time given in seconds as one string in the format hh:mm:ss.t
--------------------------------------------------------------------------------------------------------- */
void time_to_string(char* time, double seconds)
{
	double full_seconds = seconds_in_seconds(seconds);
	sprintf(time, "%d:%02d:%02d.%d", hours_in_seconds(seconds), minutes_in_seconds(seconds), (int)full_seconds, (int)((full_seconds-(double)((int)full_seconds))*10));
}


/* ------------------------------------------------------------------------------------------------------
   Returns time encoded into three bytes (bitwise encoding or BCD) as seconds
--------------------------------------------------------------------------------------------------------- */
double time_to_total_seconds(int hours, int minutes, int seconds, int format)
{
	double time_in_seconds = 0.0;

	switch (format) {
		case TIME_FORMAT_BITWISE:
			time_in_seconds += (double)time_to_hours(hours)*3600;
			time_in_seconds += (double)time_to_minutes(minutes)*60;
			time_in_seconds += (double)time_to_seconds(seconds);
			time_in_seconds += (double)time_to_tenths(minutes, seconds)/10;
			break;
		case TIME_FORMAT_BCD:
			time_in_seconds = (double)bcd_to_decimal(hours)*3600 + (double)bcd_to_decimal(minutes)*60 + (double)bcd_to_decimal(seconds);
			break;
	}
	return time_in_seconds;
}


/* ------------------------------------------------------------------------------------------------------
   Calculates a laptime from two time values encoded into three bytes (bitwise encoding)
   as one string in the format hh:mm:ss.t
--------------------------------------------------------------------------------------------------------- */
void laptime_to_string(char* time, int lap_start_hours, int lap_start_minutes, int lap_start_seconds, int lap_end_hours, int lap_end_minutes, int lap_end_seconds, int format)
{
	double start_seconds = 0.0;
	double end_seconds = 0.0;
	double laptime_seconds = 0.0;
	int laptime_minutes;
	int laptime_hours;

	switch (format) {
		case TIME_FORMAT_BITWISE:
			start_seconds += (double)time_to_hours(lap_start_hours)*3600;
			start_seconds += (double)time_to_minutes(lap_start_minutes)*60;
			start_seconds += (double)time_to_seconds(lap_start_seconds);
			start_seconds += (double)time_to_tenths(lap_start_minutes, lap_start_seconds)/10;
			end_seconds += (double)time_to_hours(lap_end_hours)*3600;
			end_seconds += (double)time_to_minutes(lap_end_minutes)*60;
			end_seconds += (double)time_to_seconds(lap_end_seconds);
			end_seconds += (double)time_to_tenths(lap_end_minutes, lap_end_seconds)/10;
			laptime_seconds = end_seconds - start_seconds;
			laptime_hours = (int)(laptime_seconds/3600);
			laptime_minutes = (int)((laptime_seconds-laptime_hours*3600)/60);
			laptime_seconds = laptime_seconds - 3600*laptime_hours - 60*laptime_minutes;
			sprintf(time, "%d:%02d:%02.1f", laptime_hours, laptime_minutes, laptime_seconds);
			break;
	}
}


/* ------------------------------------------------------------------------------------------------------
   Calculates a laptime from two time values encoded into three bytes (bitwise encoding)
   as one string in the format ss.t
--------------------------------------------------------------------------------------------------------- */
double laptime_to_seconds(int lap_start_hours, int lap_start_minutes, int lap_start_seconds, int lap_end_hours, int lap_end_minutes, int lap_end_seconds)
{
	double start_seconds = 0.0;
	double end_seconds = 0.0;
	double laptime_seconds = 0.0;

	start_seconds += (double)time_to_hours(lap_start_hours)*3600;
	start_seconds += (double)time_to_minutes(lap_start_minutes)*60;
	start_seconds += (double)time_to_seconds(lap_start_seconds);
	start_seconds += (double)time_to_tenths(lap_start_minutes, lap_start_seconds)/10;
	end_seconds += (double)time_to_hours(lap_end_hours)*3600;
	end_seconds += (double)time_to_minutes(lap_end_minutes)*60;
	end_seconds += (double)time_to_seconds(lap_end_seconds);
	end_seconds += (double)time_to_tenths(lap_end_minutes, lap_end_seconds)/10;
	laptime_seconds = end_seconds - start_seconds;
	return laptime_seconds;
}


/* ------------------------------------------------------------------------------------------------------
   Returns an 8 bit binary representation of a byte (lsb)
--------------------------------------------------------------------------------------------------------- */
void byte_to_binary_str(char* binary, char byte)
{	
	char val = byte;
	char mask = 128;
	int i;
	for (i = 0; i< 8; i++) {
		binary[i] = ( (val & mask) >> 7 ? '1' : '0' );
		val = val << 1;
	}
	binary[8] = '\0';
}


/* ------------------------------------------------------------------------------------------------------
   Returns true if this value represents an hour measurement from
   an autolap, false if not
--------------------------------------------------------------------------------------------------------- */
char is_autolap(int hour_value)
{
	// rs200 marks the first bit of hour value as one if time data comes from autolap
	if ((hour_value & 0x80) >> 7)
		return TRUE;
	else
		return FALSE;
}


/* ------------------------------------------------------------------------------------------------------
   Simple substring function
--------------------------------------------------------------------------------------------------------- */
void substr(char dest[], char src[], int offset, int len)
{
	int i;
	for(i = 0; i < len && src[offset + i] != '\0'; i++)
		dest[i] = src[i + offset];
	dest[i] = '\0';
}
