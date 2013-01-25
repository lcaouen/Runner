/* ------------------------------------------------------------------------------------------------------
   decoders.c

   This file contains functions for interpreting RS200 data.
--------------------------------------------------------------------------------------------------------- */

#include "stdafx.h"
#include <sys/stat.h>

#include "decoders.h"
#include "signal_functions.h"
#include "data_structures.h"
#include "output_functions.h"

// Polar heart rate monitor model

#define None 0
#define S410 1
#define S510 2
#define RS200 3

// in RS200 each transmitted section is always 255 bytes long (the data is padded)
#define RS200_SECTION_LENGTH 255

static int monitor_type = None;
static char out_f[255];

/* ------------------------------------------------------------------------------------------------------
   CRC-16 calculation used in checking the preamble (actual data does not seem to have a CRC in RS200)
--------------------------------------------------------------------------------------------------------- */
unsigned short calc_crc( unsigned short crc, unsigned char byte )
{
	unsigned short uch  = (unsigned short) byte;
	unsigned short new_crc = crc;
	int i;
	new_crc ^= ( uch << 8 );
	for ( i = 0; i < 8; i++ ) {
		if ( new_crc & 0x8000 )
		new_crc = ( new_crc << 1 ) ^ 0x8005;
	else
		new_crc <<= 1;
	}
	return new_crc;
}

/* ------------------------------------------------------------------------------------------------------
   generates a file name from the data given by RS200
   this is used when the user defines just a directory where to put the output but no file name
--------------------------------------------------------------------------------------------------------- */
void set_automatic_out_file(char *dir_name, char *file_name, char *suffix, session_data sess_data)
{
	static char *months[15] = { "00", "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec","13?","14?"};

	sprintf(file_name, "%s/session_data_%s_%s-%d-%d_%d-%d-%d%s", dir_name, sess_data.session_name, months[sess_data.session_month], sess_data.session_day, sess_data.session_year, sess_data.session_start_hour, sess_data.session_start_minute, sess_data.session_start_second, suffix);

}


/* ------------------------------------------------------------------------------------------------------
   opens an output for writing, either stdout or a file
--------------------------------------------------------------------------------------------------------- */
void open_output(char *name)
{
	if (name != NULL) {
		print_debug("Opening output file %s.", name);
		set_output_stream(fopen(name,"w"));
		if (get_output_stream == NULL) {
			fail("Could not open file for output.");
    	}
	} else {
		print_debug("Setting output to standard out.");
		set_output_stream(stdout);
	}
}


/* ------------------------------------------------------------------------------------------------------
   closes output file
--------------------------------------------------------------------------------------------------------- */
void close_output()
{
	print_debug("Close output called, will do nothing if standard out used.");
	if ((get_output_stream() != NULL) && (get_output_stream() != stdout)) {
		print_debug("Closing output file.");
		fclose(get_output_stream());
		set_output_stream(NULL);
	}
}


/* ------------------------------------------------------------------------------------------------------
   Reads bytes from file as unsigned chars
--------------------------------------------------------------------------------------------------------- */
static int byte_read(int restart)
{
	static FILE 	*fp = NULL;
	char		c1;
	unsigned char	c2;

	if (restart && fp != NULL) {
		print_debug("Closing old file and opening next.");
		fclose(fp);
		fp = NULL;
	}

	if (fp == NULL) {
		fp = fopen(get_input_file_name(),"rb");
		if(fp == NULL) {
			print_debug("Tried to open %s.",get_input_file_name());
			fail("Failed to open input file");
		}
	}

	c1 = getc(fp);
	c2 = c1;
	if(feof(fp)) {
		print_debug("Hit EOF");
		return 40000;
	}
	return c2;	
}


/* ------------------------------------------------------------------------------------------------------
   extracts data from SonicLink waveform or a byte file in raw format without interpretation
--------------------------------------------------------------------------------------------------------- */
int extract_bytes(int *data, int max_size, boolean unlimited_read, boolean is_wave)
{
	int byte,restart,new_file;
	int size,total_size;
	int data_size;
	int file_size=0;
	int section_size;
	int number_of_sections;
	int section_start,section_number,section_header_count;
	int section_headers[3];
	int padded;
	unsigned short crc;
	boolean padding = FALSE;

	print_debug("Decoding signal into bytes.");
	new_file = 1;

	goto error;
	for(;;) {
		if (is_wave)
			byte = byte_decode(restart, new_file);
		else
			byte = byte_read(restart);

		if (byte == -3) {
			print_debug("No more bytes.\n");
			break;
		}
		if (byte>0)
			print_deep_debug("Got byte %d.\n",byte);
		restart = 0;
		new_file = 0;
		if(byte == -2) {
error:
			print_debug("Errors in signal, restart reading.\n");
			restart = 1;
			size = 0;
			total_size = 0;
			padded = 0;
			section_start = 8;
			section_header_count = 0;
			section_number = 1;
			crc = 0;
			continue;
		}
		if(byte >= 0) {
			if(size >= max_size) fail("Too much data, read aborted.");

			// skip padded data between sections
			if (!padding) {
				data[total_size++] = byte;
			} else {
				padded++;
			}
			size++;

			// capture section headers
			if (section_header_count < 3) {
				section_headers[section_header_count++] = byte;
			}

			crc = calc_crc(crc, byte);

			if(size == 1 && data[0] != 85) {
				print_debug("Bad first section header (byte 1).");
				goto error;
			}
			if(size == 2) {
				switch(data[1]) {
					case 1:
						monitor_type = RS200;
						print_debug("Polar RS200 SonicLink data detected.");
						break;
					case 10:
						monitor_type = S410;
						print_debug("Polar S410 SonicLink data detected.");
						break;
					case 81:
						monitor_type = S510;
						print_debug("Polar S510 SonicLink data detected.");
						break;
					default:
						print_debug("Polar S410 or S510 or RS200 SonicLink data NOT detected.");
						break;
				}
			}
			if(size == 3 && data[2] != 2) {
				print_debug("Bad preamble header (byte %d), header value %d.", 3, data[2]);
				goto error;
			}
			if(size == 4) {
				// byte 3 stores the number of sections
				number_of_sections = data[3];
				print_debug("Number of sections in file is %d.",number_of_sections);
			}
			if(size == 6) {
				// bytes 4 and 5 give the size of how much actual data there is in sections (excluding padding)
				// add size of main header + section headers + 1 for trailer to get total
				data_size = (data[5] << 8) + data[4] + 8 + 3 * number_of_sections + 1;

				// file size is header + 256 bytes per section + trailer
				file_size = 8 + number_of_sections * RS200_SECTION_LENGTH + 1;
				print_debug("Given datasize %d.",data_size);
				print_debug("File size %d.",file_size);
			}
			if(size == 8 && crc) {
				print_debug("Preamble crc error.");
				if (!unlimited_read) fail("Error in data checksum (preamble crc)");
			}
			if(size < 8)
				continue;
			if(size == section_start) { // next section starts, start collecting headers
				section_header_count = 0;
			}
			if(size == section_start + 3) {  // new section (header complete), check that header is valid and stop padding
				print_debug("Section header found.");
				if(section_headers[0] != 85 || section_headers[1] != section_number || section_headers[2] <= 0) {
					print_debug("Bad section header. Header ID %d, section number %d, section size %d.", section_headers[0], section_headers[1], section_headers[2]);
					if (!unlimited_read) fail("Error in data (section header)");
				}
				print_debug("New data section, section number %d, section size %d\n", section_headers[1], section_headers[2]);
				section_size = section_headers[2];
				padding = FALSE;
				crc = 0;
			}
			if((size >= section_start + 3) && (size == section_start + section_size +3)) { // end of a section
				print_debug("Section finished.");
				if (section_number < number_of_sections)
					print_debug("Waiting for a new section.");
				//if (is_wave) {
					section_start = section_number * RS200_SECTION_LENGTH + 8;
				//} else {
				//	section_start = size;
				//	section_header_count = 0;
				//}
				section_number++;
				// don't record the remaining data, next relevant data will start after padding
				padding = TRUE;
			}
			if(size == file_size) {
				if (byte == 7) {
					print_debug("Found trailer byte %d marking end of file.", 7);
				} else {
					if (!unlimited_read) fail("Error, no trailer byte in data.");
				}
			}
			update_progress((double)size / (double)file_size);
			// is this the end of data, if yes and unlimited read is not defined, out
			if(size == file_size && !unlimited_read) {
				print_debug("End of data, processed size: %d", size);
				break;
			}
		}
	}
	print_debug("Finished decoding, extracted file size %d bytes, found %d bytes and %d data bytes.", file_size, size, total_size);
	update_status("Terminé");

	return total_size;
}

/* ------------------------------------------------------------------------------------------------------
  reads raw data from an ascii file
--------------------------------------------------------------------------------------------------------- */
#define LINE_LENGTH 80

int extract_ascii_file_bytes(int *data, int max_size, boolean unlimited_read)
{
	FILE 	*fp = NULL;
	char 	c2[LINE_LENGTH];
	char 	line[LINE_LENGTH];
	int	total_size = 0;
	int 	i;
	int	commastart, commaend;

	print_debug("Reading bytes from ascii file.");

	fp = fopen(get_input_file_name(),"r");

	if(fp == NULL) {
		print_debug("Tried to open %s.",get_input_file_name());
		fail("Failed to open input device");
	}

	while(fgets(line, LINE_LENGTH, fp) != NULL) {
		commastart = 0;
		commaend = 0;
		if (line[0] != '#') {
			for (i=0; i< LINE_LENGTH; i++) {
				if (line[i] == ',') {
					if (commastart == 0)
						commastart = i;
					else
						commaend = i;
				}
				if (commastart > 0 && commaend > 0)
					break;
				
			}
			if (commastart > 0 && commaend > 0) {
				strncpy(c2, line+commastart+1, commaend-commastart-1);
				data[total_size++] = atoi(c2);
			}
		}
	}
	fclose(fp);

	return total_size;
}

int extract_sound_bytes(int *data, int max_size, boolean unlimited_read)
{
	return extract_bytes(data, max_size, unlimited_read, TRUE);
}

int extract_file_bytes(int *data, int max_size, boolean unlimited_read)
{
	return extract_bytes(data, max_size, unlimited_read, FALSE);
}

/* ------------------------------------------------------------------------------------------------------
  Decoding data
--------------------------------------------------------------------------------------------------------- */
char* decode(char* in_strPath)
{
	int size,data[8192];
	char *output_file_name = NULL;
	char *output_dir_name = NULL;
	char suffix[5] = "";
	boolean unlimited_read = FALSE;
	session_data sess_data;
	followup_data followup;
	struct stat St;

	out_f[0] = 0;

	set_debug(FALSE);
	set_deep_debug(FALSE);
	set_output_stream(NULL);

	// microphone
	set_read_from_dsp(TRUE);
	set_dsp_device("");

	// directory
	output_dir_name = in_strPath;

	if (!(output_dir_name != NULL && strlen(output_dir_name)))
		fail("Cannot write data. You must specify a directory where to put the session data.");
	if (stat(output_dir_name, &St) != 0 )
		fail("Error, the directory you specified does not exist.");

	get_read_from_dsp();

	// run the program for each input file and produce requested output files
	// then process the files
	
	
	size = extract_sound_bytes(data, sizeof(data), unlimited_read);

	// output the data in all requested formats
	if (size > 0) {
		// is this session data or follow-up data?
		if (data[11] == 1) {
			followup = extract_followup_data(data, size);
			open_output(output_file_name);
			followup_summary(followup);
		} else {
			sess_data = extract_session_data(data, size);
			// output data in xml format
			sprintf(suffix,".xml");
			set_automatic_out_file(output_dir_name, out_f, suffix, sess_data);
			open_output(out_f);
			session_as_xml(sess_data);
			close_output();
		}

		// free any memory allocated for session data (laps) before next round
		// also empty all other variables
		free_session_data(sess_data);
		free_followup_data(followup);
		sess_data.number_of_laps = 0;
		followup.number_of_indices = 0;
		followup.number_of_summaries = 0;
	}

	close_audio();

	return out_f;
}

/* ------------------------------------------------------------------------------------------------------
  interprets the extracted data as Polar RS200 session data and stores into internal structure
--------------------------------------------------------------------------------------------------------- */
session_data extract_session_data(int *data, int size)
{
	static char *charset = "0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz?";

	int i,s;
	int start_of_this_part = 10;
	int lap_offset = 0;

	session_data sess_data;
	sess_data.limit_type = NO_LIMITS;

	print_debug("Now extracting fields from data.");

	// the file must be at least header + 128 bytes long to be valid
	if (size < start_of_this_part + 128)
		fail("Data file too small");

	// first extract basic data to be written to file header. This includes session name, session date and session time
	// exercise name is bytes 17 to 24
	// the session name follows the charset of the device in normal letters and numbers
	// however in special characters ( -%/()*+.: ), there seems to be no clear logic 
	// (the values seem to come from a conversion table)
	s = 0;
	for (i = 17; i < 25; i++) {
		if (data[start_of_this_part + i] < 64)
        		sess_data.session_name[s] = charset[data[start_of_this_part + i]];
      		else {
			// second bit marks a special character, only lowest 4 bits count
			if (data[start_of_this_part + i] & 0x40) {
				switch(data[start_of_this_part + i] & 0xF) {
					case 0:
						sess_data.session_name[s] = ':';
						break;
					case 1:
						sess_data.session_name[s] = '.';
						break;
					case 4:
						sess_data.session_name[s] = '/';
						break;
					case 5:
						sess_data.session_name[s] = '*';
						break;
					case 8:
						sess_data.session_name[s] = '-';
						break;
					case 9:
						sess_data.session_name[s] = '+';
						break;
					case 10:
						sess_data.session_name[s] = '%';
						break;
					case 11:
						sess_data.session_name[s] = '(';
						break;
					case 12:
						sess_data.session_name[s] = ')';
						break;
				}
				
			}
			// first bit marks the end of the name string
			if (data[start_of_this_part + i] & 0x80) {
				sess_data.session_name[s] = charset[data[start_of_this_part + i] - 128];
				break;
			}
		}
		s++;
	}
	sess_data.session_name[s+1]='\0';

	// combine high bits and low bits from bytes 27 and 28
	// year is then year 2000 + 3 high bits of result
	// month number is bits 6 to 9
	// day is the five lowest bits
	s = (data[start_of_this_part + 27 +1] << 8) + data[start_of_this_part + 27];
	sess_data.session_day = s & 0x1F;
	sess_data.session_month = (s & 0x1E0) >> 5;
	sess_data.session_year = 2000 + (s >> 9);

	// session start, seconds, minutes and hours are in bytes 29 to 31
	sess_data.session_start_hour = bcd_to_decimal(data[start_of_this_part + 29 + 2]);
	sess_data.session_start_minute = bcd_to_decimal(data[start_of_this_part + 29 + 1]);
	sess_data.session_start_second = bcd_to_decimal(data[start_of_this_part + 29]);

	sprintf(sess_data.session_id, "%s%d%d%d%d%d%d", sess_data.session_name, sess_data.session_day, sess_data.session_month, sess_data.session_year,  sess_data.session_start_hour, sess_data.session_start_minute, sess_data.session_start_second);

	
	// user data can be found in the first 16 bytes of actual data
	for (i = 1; i < 17; i++) {
		switch(i) {
			// weight
			case 7:
				// weight is stored in pounds multiplied by ten
				sess_data.user_weight = (int)((double)msb_lsb_to_decimal(data[start_of_this_part + i], data[start_of_this_part +i - 1])/10*0.45359+0.5);
				break;
			// height
			case 8:
				sess_data.user_height = data[start_of_this_part + i];
				break;
			// Birthdate
			case 9:
				sprintf(sess_data.user_birth_date, "%02d.%02d.%d", data[start_of_this_part + i], data[start_of_this_part + i + 1], 1900+data[start_of_this_part + i + 2]);
				break;
			// Max pulse
			case 12: 
				sess_data.user_max_hr = data[start_of_this_part + i];
				break;
			// VO max
			case 13: 
				sess_data.user_vo_max = data[start_of_this_part + i];
				break;
			// Rest pulse
			case 14: 
				sess_data.user_sitting_hr = data[start_of_this_part + i];
				break;
			// Sex
			case 15:
				// bit 1 of this byte seems to indicate gender
				// the meaning of rest of the bits is unknown
				if (data[start_of_this_part + i] & 0x1)
					strcpy(sess_data.user_sex, "Female");
				else
					strcpy(sess_data.user_sex, "Male");
				break;
			// Activity level
			case 16:
				switch(data[start_of_this_part + i]) {
					case 0:
						strcpy(sess_data.user_activity_level, "Top");
						break;
					case 1:
						strcpy(sess_data.user_activity_level, "High");
						break;
					case 2:
						strcpy(sess_data.user_activity_level, "Moderate");
						break;
					case 3:
						strcpy(sess_data.user_activity_level, "Low");
						break;
				}
				break;
		}

	}

	// session data is bytes 25 to 39
	for (i = 25; i < 40; i++) {
		switch(i) {
			case 26:
				// bytes 25 and 26 contain information about exercise type
				// bit 4 of byte 25 should be 1 if footpod has been turned on
				// bit 1 of byte 26 seems to be one when BPM HR limits are in use
				// on interval trainings with pace limit bit 1 of byte 25 seems to be additionally on
				if (data[start_of_this_part + i - 1] & 0x8) {
					sess_data.has_pace_data = TRUE;
				} else {
					sess_data.has_pace_data = FALSE;
				}
				if (data[start_of_this_part + i] & 0x1) {
					sess_data.limit_type = HR_LIMITS;
				}
				if (data[start_of_this_part + i] & 0x1 && data[start_of_this_part + i-1] & 0x1) {
					sess_data.limit_type = PACE_LIMITS;
				}
				sess_data.number_of_laps = data[start_of_this_part + 122];
				break;
			// total session time
			case 32:
				sess_data.session_length = time_to_total_seconds(data[start_of_this_part +i+2],data[start_of_this_part +i+1],data[start_of_this_part +i], TIME_FORMAT_BITWISE);
				break;
			// average heart rate in session
			case 35: 
				sess_data.session_avg_hr = data[start_of_this_part + i];
				break;
			// max heart rate in session
			case 36:
				sess_data.session_max_hr = data[start_of_this_part + i];
				break;
			// max heart rate (programmed)
			case 37:
				sess_data.session_max_set_hr = data[start_of_this_part + i];
				break;
			// calories
			case 38:
				sess_data.session_calories = msb_lsb_to_decimal(data[start_of_this_part + i+1], data[start_of_this_part + i]);
				break;
		}
	}

	// zone limits and times on each zone are in bytes 40 to 91
	// first zone limits, there are max 3 sets of limits per exercise
	for (s = 0; s < 4; s++) {
		// limits can be set either as a sportzone, as BPM or as pace
		if (sess_data.limit_type == PACE_LIMITS) {
			sess_data.limiterzones[s].limit_low = msb_lsb_to_decimal(data[start_of_this_part + 40 + s*4 + 1], data[start_of_this_part + 40 + s*4]);
			sess_data.limiterzones[s].limit_high = msb_lsb_to_decimal(data[start_of_this_part + 40 + s*4 + 3], data[start_of_this_part + 40 + s*4 + 2]);
		} else if (sess_data.limit_type == HR_LIMITS) {
			sess_data.limiterzones[s].limit_low = data[start_of_this_part + 40 + s*4];
			sess_data.limiterzones[s].limit_high = data[start_of_this_part + 40 + s*4+2];
		} else {
			sess_data.limiterzones[s].sport_zone = data[start_of_this_part + 40 + s*4];
		}
	}

	// then time spent on each zone
	for (s = 0; s < 4; s++) {
		// below
		sess_data.limiterzones[s].below_zone_seconds=time_to_total_seconds(data[start_of_this_part + 56 + s*9+2],data[start_of_this_part + 56 + s*9+1],data[start_of_this_part + 56 + s*9], TIME_FORMAT_BCD);
		// inside
		sess_data.limiterzones[s].on_zone_seconds=time_to_total_seconds(data[start_of_this_part + 56 + s*9+5],data[start_of_this_part + 56 + s*9+4],data[start_of_this_part + 56 + s*9+3], TIME_FORMAT_BCD);
		// above
		sess_data.limiterzones[s].above_zone_seconds=time_to_total_seconds(data[start_of_this_part + 56 + s*9+8],data[start_of_this_part + 56 + s*9+7],data[start_of_this_part + 56 + s*9+6], TIME_FORMAT_BCD);
	}

	// five sport zones can be found from bytes 92 to 116
	for (s = 0; s < 5; s++) {
		sess_data.sportzones[s].zone_number = s+1;
		sess_data.sportzones[s].low_percent = data[start_of_this_part + 92 + s * 2];
		sess_data.sportzones[s].high_percent = data[start_of_this_part + 92 + s * 2 + 1];
	}
	for (s = 0; s < 5; s++) {
		sess_data.sportzones[s].on_zone_seconds = time_to_total_seconds(data[start_of_this_part + 102 + s*3 + 2], data[start_of_this_part + 102 + s*3+1],data[start_of_this_part + 102 + s*3], TIME_FORMAT_BCD);
	}

	// pace and lap summary, distance, max pace and average pace
	// are in bytes 117 to 120 and 127 to 128
	if (sess_data.has_pace_data) {
		sess_data.session_total_distance = msb_lsb_to_decimal(data[start_of_this_part + 118], data[start_of_this_part + 117])*10;
		sess_data.session_max_pace = msb_lsb_to_decimal(data[start_of_this_part + 120], data[start_of_this_part + 119]);
		sess_data.session_avg_pace = msb_lsb_to_decimal(data[start_of_this_part + 128], data[start_of_this_part + 127]);
	} else {
		sess_data.session_total_distance = 0;
		sess_data.session_max_pace = 0;
		sess_data.session_avg_pace = 0;
	}
	
	// best lap and best lap time in bytes 123 to 126
	sess_data.session_best_lap = data[start_of_this_part + 126];
	sess_data.session_best_laptime = time_to_total_seconds(data[start_of_this_part +125],data[start_of_this_part +124],data[start_of_this_part +123], TIME_FORMAT_BITWISE);

	if (sess_data.number_of_laps > 0)
		sess_data.laps = (lap **)malloc(sizeof(lap*)*sess_data.number_of_laps);
	
	// laps start from byte 129
	for (s = 0; s < sess_data.number_of_laps; s++) {

		sess_data.laps[s] = (lap *)malloc(sizeof(lap));
	
		// with pace information there are 12 bytes of data per lap
		// without it there are only six bytes
		sess_data.laps[s]->lap_no = sess_data.number_of_laps-s;

		if (sess_data.has_pace_data) 
			lap_offset = 12;
		else 
			lap_offset = 6;

		// lap end time
		sess_data.laps[s]->lap_end_time = time_to_total_seconds(data[start_of_this_part +129 + s * lap_offset + 2],data[start_of_this_part +129 + s * lap_offset + 1],data[start_of_this_part +129 + s*lap_offset], TIME_FORMAT_BITWISE);

		// calculate actual lap time, if this is not the first lap then lap start time can be found from the
		// end time of the previous lap (this comes later in the data)
		if (sess_data.number_of_laps-s > 1) {
			sess_data.laps[s]->laptime = laptime_to_seconds(data[start_of_this_part +129 + (s+1) * lap_offset + 2],data[start_of_this_part +129 + (s+1) * lap_offset + 1],data[start_of_this_part +129 + (s+1)*lap_offset], data[start_of_this_part +129 + s * lap_offset + 2],data[start_of_this_part +129 + s * lap_offset + 1],data[start_of_this_part +129 + s*lap_offset]);
		} else {
			sess_data.laps[s]->laptime = time_to_total_seconds(data[start_of_this_part +129 + s * lap_offset + 2],data[start_of_this_part +129 + s * lap_offset + 1],data[start_of_this_part +129 + s*lap_offset], TIME_FORMAT_BITWISE);
		}

		sess_data.laps[s]->autolap = (char)is_autolap(data[start_of_this_part +129 + s * lap_offset + 2]);

		// pulse values
		sess_data.laps[s]->end_hr = data[start_of_this_part + s * lap_offset + 129 + 3];
		sess_data.laps[s]->avg_hr = data[start_of_this_part + s * lap_offset + 129 + 4];
		sess_data.laps[s]->max_hr = data[start_of_this_part + s * lap_offset + 129 + 5];

		if (sess_data.has_pace_data) {
			sess_data.laps[s]->end_pace = msb_lsb_to_decimal(data[start_of_this_part + s * lap_offset + 129 + 7], data[start_of_this_part + s * lap_offset + 129 + 6]);
			sess_data.laps[s]->avg_pace = msb_lsb_to_decimal(data[start_of_this_part + s * lap_offset + 129 + 11], data[start_of_this_part + s * lap_offset + 129 + 10]);
			if (sess_data.number_of_laps-s > 1) {
				sess_data.laps[s]->lap_length = msb_lsb_to_decimal(data[start_of_this_part + s * lap_offset + 129 + 9], data[start_of_this_part + s * lap_offset + 129 + 8]) - msb_lsb_to_decimal(data[start_of_this_part + (s + 1) * lap_offset + 129 + 9], data[start_of_this_part + (s + 1) * lap_offset + 129 + 8]);
			} else {
				sess_data.laps[s]->lap_length = msb_lsb_to_decimal(data[start_of_this_part + s * lap_offset + 129 + 9], data[start_of_this_part + s * lap_offset + 129 + 8]);
			}
		} else {
			sess_data.laps[s]->end_pace = 0;
			sess_data.laps[s]->avg_pace = 0;
			sess_data.laps[s]->lap_length = 0;
		}
	}
	return sess_data;
}


/* ------------------------------------------------------------------------------------------------------
  interprets the extracted data as Polar RS200 follow-up data and stores into internal structure
--------------------------------------------------------------------------------------------------------- */
followup_data extract_followup_data(int *data, int size)
{
	int i,s,j;
	int start_of_this_part = 10;	
	time_t now;
	struct tm *now_tm;

	followup_data followup;

	// store current date into the structure for later use
	now = time (NULL);
        now_tm = localtime (&now);
	sprintf(followup.decoding_date, "%02d.%02d.%d", now_tm->tm_mday, now_tm->tm_mon + 1, now_tm->tm_year + 1900);

	print_debug("Now extracting fields from data.");

	// the file must be at least header + 128 bytes long to be valid
	if (size < start_of_this_part + 128)
		fail("Data file too small");

	// first read own indices, there are max 16 of these
	// these consist of ownIndex value and date recorded
	s = 0;
	followup.indices = (ownIndex **)malloc(sizeof(ownIndex*)*16);
	followup.number_of_indices = 0;
	for (i = 6; i < 70; i+=4) {
		// if there is a recorded date, allocate memory for this object and store values
		if (data[start_of_this_part + i] > 0) {
			followup.indices[followup.number_of_indices] = (ownIndex *)malloc(sizeof(ownIndex));
			followup.indices[followup.number_of_indices]->value = (short)data[start_of_this_part + i+3];
			followup.indices[followup.number_of_indices]->recorded_day = (short)data[start_of_this_part + i];
			followup.indices[followup.number_of_indices]->recorded_month = (short)data[start_of_this_part + i+1];
			followup.indices[followup.number_of_indices]->recorded_year = 2000 + data[start_of_this_part + i+2];
        		followup.number_of_indices++;
		}
	}

	// store available total values

	followup.all_time_soniclink_sends = msb_lsb_to_decimal(data[start_of_this_part + 87], data[start_of_this_part + 86]);
	followup.all_time_number_of_sessions_stored = msb_lsb_to_decimal(data[start_of_this_part + 77], data[start_of_this_part + 76]);
	followup.all_time_number_of_sessions_with_pod = msb_lsb_to_decimal(data[start_of_this_part + 83], data[start_of_this_part + 82]);

	// this one could use further testing
	followup.all_time_number_of_tests_performed = msb_lsb_to_decimal(data[start_of_this_part + 93], data[start_of_this_part + 92]);
	
	followup.total_duration = bcd_to_decimal(data[start_of_this_part + 96]) + bcd_to_decimal(data[start_of_this_part + 97]) * 60 + bcd_to_decimal(data[start_of_this_part + 98]) * 3600 + bcd_to_decimal(data[start_of_this_part + 99]) * 100 * 3600;
	followup.duration_recording_start_day = (short)(data[start_of_this_part + 100] & 0x1F);
	followup.duration_recording_start_month = (short)(((data[start_of_this_part + 101] & 0x1) << 3) | ((data[start_of_this_part + 100] & 0xE0) >> 5));
	followup.duration_recording_start_year = (short)(2000 + ((data[start_of_this_part + 101] & 0xFE) >> 1));
	
	followup.total_calories = (double)bcd_to_decimal(data[start_of_this_part + 102]) + (double)bcd_to_decimal(data[start_of_this_part + 103]) * 100 + (double)bcd_to_decimal(data[start_of_this_part + 104]) * 10000 + (double)bcd_to_decimal(data[start_of_this_part + 105]) * 1000000;
	followup.calories_recording_start_day = (short)(data[start_of_this_part + 106] & 0x1F);
	followup.calories_recording_start_month = (short)(((data[start_of_this_part + 107] & 0x1) << 3) | ((data[start_of_this_part + 106] & 0xE0) >> 5));
	followup.calories_recording_start_year = (short)(2000 + ((data[start_of_this_part + 107] & 0xFE) >> 1));
	
	followup.total_exercises = bcd_to_decimal(data[start_of_this_part + 108]) + bcd_to_decimal(data[start_of_this_part + 109]) * 100;
	followup.exercises_recording_start_day = (short)(data[start_of_this_part + 110] & 0x1F);
	followup.exercises_recording_start_month = (short)(((data[start_of_this_part + 111] & 0x1) << 3) | ((data[start_of_this_part + 110] & 0xE0) >> 5));
	followup.exercises_recording_start_year = (short)(2000 + ((data[start_of_this_part + 111] & 0xFE) >> 1));
	
	followup.total_distance = ((double)(data[start_of_this_part + 112] + (data[start_of_this_part + 113] << 8) + (data[start_of_this_part + 114] << 16) + (data[start_of_this_part + 115] << 24)))/100;
	followup.distance_recording_start_day = (short)(data[start_of_this_part + 116] & 0x1F);
	followup.distance_recording_start_month = (short)(((data[start_of_this_part + 117] & 0x1) << 3) | ((data[start_of_this_part + 116] & 0xE0) >> 5));
	followup.distance_recording_start_year = (short)(2000 + ((data[start_of_this_part + 117] & 0xFE) >> 1));

	// store weekly summaries, there are max 15 summaries in total
	// each has 23 values
	// first week starts from byte 417, second week follows
	// third week jumps back to byte 118
	followup.summaries = (weekly_summary **)malloc(sizeof(weekly_summary*)*15);
	followup.number_of_summaries = 0;
	j=0;
	for (i = 118; i < 463; i+=23) {
		if (i == 417) j = 0;
		else if (i == 440) j = 1;
		else j = (i - 118) / 23 + 2;
		followup.summaries[j] = (weekly_summary *)malloc(sizeof(weekly_summary));
		followup.summaries[j]->sum_session_time = bcd_to_decimal(data[start_of_this_part + i + 15]) + bcd_to_decimal(data[start_of_this_part + i + 16]) * 60 + bcd_to_decimal(data[start_of_this_part + i + 17]) * 3600;
		followup.summaries[j]->sum_session_distance = (double)msb_lsb_to_decimal(data[start_of_this_part + i + 19], data[start_of_this_part + i + 18]) / 100;
		followup.summaries[j]->sum_session_calories = (double)msb_lsb_to_decimal(data[start_of_this_part + i + 22], data[start_of_this_part + i + 21]);
		followup.summaries[j]->number_of_sessions = data[start_of_this_part + i + 20];
        		
		// sport zone data (5 zones each)
		for (s = 0; s < 5; s++) {
			followup.summaries[j]->sportzones[s].zone_number = s+1;
			followup.summaries[j]->sportzones[s].on_zone_seconds = bcd_to_decimal(data[start_of_this_part + i + s*3 + 0]) + bcd_to_decimal(data[start_of_this_part + i + s*3 + 1]) * 60 + bcd_to_decimal(data[start_of_this_part + i + s*3 + 2]) * 3600;
		}
		followup.number_of_summaries++;
	}

	return followup;
}


/* ------------------------------------------------------------------------------------------------------
  fucntions to free memory allocated for session_data and followup_data objects
--------------------------------------------------------------------------------------------------------- */

void free_session_data(session_data sess_data)
{
	int s;
	if (sess_data.number_of_laps > 0) {
		for (s = 0;s < sess_data.number_of_laps;s++) {
			free(sess_data.laps[s]);
		}
		free(sess_data.laps);
	}
}


void free_followup_data(followup_data followup)
{
	int s;

	if (followup.number_of_indices > 0) {
		for (s = 0; s < followup.number_of_indices; s++) {
			free(followup.indices[s]);
		}
		free(followup.indices);
	}

	if (followup.number_of_summaries > 0) {
		for (s = 0; s < followup.number_of_summaries; s++) {
			free(followup.summaries[s]);
		}
		free(followup.summaries);
	}
}
