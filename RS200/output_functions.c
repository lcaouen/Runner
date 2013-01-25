/* ------------------------------------------------------------------------------------------------------
   output_functions.c

   This file contains functions that output the session data in various formats.
------------------------------------------------------------------------------------------------------- */

#include "stdafx.h"
#include "output_functions.h"
#include "display_functions.h"

#define RS200_SECTION_LENGTH 255


/* ------------------------------------------------------------------------------------------------------
  Prints out session data as a human-readable summary
--------------------------------------------------------------------------------------------------------- */

void session_summary(session_data sess_data)
{
	static char *months[15] = { "00", "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec","13?","14?"};
	int format = OUTPUT_NORMAL;
	char time[16];
	int s;

	print_debug("Printing summary data.");

	plotheader(format, 	"SESSION DATA");
	plotheader(format, 	"User information");
	plotdata(format, 	"Weight (kg)", 		"%d", sess_data.user_weight);
	plotdata(format, 	"Height", 		"%d", sess_data.user_height);
	plotdata(format, 	"Birth date", 		"%s", sess_data.user_birth_date); 
	plotdata(format, 	"Max pulse", 		"%d", sess_data.user_max_hr);
	plotdata(format, 	"VO max", 		"%d", sess_data.user_vo_max);
	plotdata(format, 	"Sitting pulse", 	"%d", sess_data.user_sitting_hr);
	plotdata(format, 	"Gender",		"%s", sess_data.user_sex);
	plotdata(format, 	"Activity level", 	"%s", sess_data.user_activity_level);

	plotheader(format, 	"Session data");
	plotdata(format, 	"Session name", 	"%s", sess_data.session_name);

	plotdata(format, 	"Number of laps", 	"%d", sess_data.number_of_laps);
	
	sprintf(time, "%s %02d %d", months[sess_data.session_month], sess_data.session_day, sess_data.session_year);
	plotdata(format, 	"Session date", 	"%s", time);
	
	sprintf(time, "%02d:%02d:%02d", sess_data.session_start_hour, sess_data.session_start_minute, sess_data.session_start_second);
	plotdata(format, 	"Session start time", 	"%s", time);

	time_to_string(time, sess_data.session_length);
	plotdata(format, 	"Session length", 	"%s", time);

	plotdata(format, 	"Average heart rate", 	"%d", sess_data.session_avg_hr);
	plotdata(format, 	"Max heart rate", 	"%d", sess_data.session_max_hr);
	plotdata(format, 	"Max set heart rate", 	"%d", sess_data.session_max_set_hr);
	plotdata(format, 	"Calories", 		"%d", sess_data.session_calories);

	plotheader(format, 	"Limiter Zones");
	plotdata(format,	"Limiter type",		"%d", (int)sess_data.limit_type);
	
	for (s = 0; s < 4; s++) {
		if (sess_data.limit_type == PACE_LIMITS) {
			plotpacedata(format, "Pace zone low (in min per km)", sess_data.limiterzones[s].limit_low, PACE_FORMAT_MIN_PER_KM);
			plotpacedata(format, "Pace zone high (in min per km)", sess_data.limiterzones[s].limit_high, PACE_FORMAT_MIN_PER_KM);
		} else if (sess_data.limit_type == HR_LIMITS) {
			plotzonedata(format, "Limiter zone", s+1, "BPM limit low", sess_data.limiterzones[s].limit_low);
			plotzonedata(format, "Limiter zone", s+1, "BPM limit high", sess_data.limiterzones[s].limit_high);
		} else {
			plotzonedata(format, "Limiter zone", s+1, "SportZone", sess_data.limiterzones[s].sport_zone);
		}
	}

	// time spent on each zone
	for (s = 0; s < 4; s++) {
		// below
		time_to_string(time, sess_data.limiterzones[s].below_zone_seconds);
		plotzonestringdata(format, "HR zone", s+1, "below", time);
		// on zone
		time_to_string(time, sess_data.limiterzones[s].on_zone_seconds);
		plotzonestringdata(format, "HR zone", s+1, "inside", time);
		// above
		time_to_string(time, sess_data.limiterzones[s].above_zone_seconds);
		plotzonestringdata(format, "HR zone", s+1, "above", time);
	}

	for (s = 0; s < 5; s++) {
		plotzonedata(format, "Sport zone", s+1, "low%", sess_data.sportzones[s].low_percent);
		plotzonedata(format, "Sport zone", s+1, "high%", sess_data.sportzones[s].high_percent);
	}
	for (s = 0; s < 5; s++) {
		time_to_string(time, sess_data.sportzones[s].on_zone_seconds);
		plotzonestringdata(format, "Sport zone", s+1, "on this zone", time);
	}

	plotheader(format, 		"Lap summary");
	plotdata(format,		"Total distance in meters", 	"%d", sess_data.session_total_distance);
	plotpacedata(format, 		"Max pace (in min per km)", 	sess_data.session_max_pace, PACE_FORMAT_MIN_PER_KM);
	plotpacedata(format, 		"Avg pace (in min per km)", 	sess_data.session_avg_pace, PACE_FORMAT_MIN_PER_KM);

	plotdata(format, 		"Best lap", 			"%d", sess_data.session_best_lap);

	time_to_string(time, sess_data.session_best_laptime);
	plotdata(format, 		"Best lap time",	 	"%s", time);

	plotheader(format, 		"Laps");
	for (s = 0;s < sess_data.number_of_laps;s++) {

		plotheader(format, "");
		plotdata(format, 	"Lap no",			"%d", sess_data.laps[s]->lap_no);

		time_to_string(time, sess_data.laps[s]->lap_end_time);
		plotdata(format, 	"Lap end time",			"%s", time);
		time_to_string(time, sess_data.laps[s]->laptime);
		plotdata(format, 	"Laptime",			"%s", time);
		plotdata(format, 	"End pulse",			"%d", sess_data.laps[s]->end_hr);
		plotdata(format, 	"Avg pulse",			"%d", sess_data.laps[s]->avg_hr);
		plotdata(format, 	"Max pulse",			"%d", sess_data.laps[s]->max_hr);

		if (sess_data.has_pace_data) {
			plotpacedata(format, "End pace (in min / km)", sess_data.laps[s]->end_pace, PACE_FORMAT_MIN_PER_KM);
			plotpacedata(format, "Avg pace (in min / km)", sess_data.laps[s]->avg_pace, PACE_FORMAT_MIN_PER_KM);
			plotdata(format, "Lap length (in m)", "%d", sess_data.laps[s]->lap_length*10);
		}
	}
	plotheader(format, "END OF DATA");
}


/* ------------------------------------------------------------------------------------------------------
  Prints out session data as a comma separated values
--------------------------------------------------------------------------------------------------------- */

void session_as_csv(session_data sess_data)
{
	int format = OUTPUT_CSV;
	int s;

	print_debug("Printing csv data.");

	// in csv files, first start off with session_id
	plotdata(format, 	"session_id", 		"%s", sess_data.session_id);

	plotheader(format, 	"SESSION DATA");
	plotheader(format, 	"User information");
	plotdata(format, 	"Weight (kg)", 		"%d", sess_data.user_weight);
	plotdata(format, 	"Height", 		"%d", sess_data.user_height);
	plotdata(format, 	"Birth date", 		"%s", sess_data.user_birth_date); 
	plotdata(format, 	"Max pulse", 		"%d", sess_data.user_max_hr);
	plotdata(format, 	"VO max", 		"%d", sess_data.user_vo_max);
	plotdata(format, 	"Sitting pulse", 	"%d", sess_data.user_sitting_hr);
	plotdata(format, 	"Gender",		"%s", sess_data.user_sex);
	plotdata(format, 	"Activity level", 	"%s", sess_data.user_activity_level);

	plotheader(format, 	"Session data");
	plotdata(format, 	"Session name", 	"%s", sess_data.session_name);
	
	if (sess_data.has_pace_data) {
		plotdata(format, "Has pace data", "%d", 1);
	} else {
		plotdata(format, "Has pace data", "%d", 0);
	}

	plotdata(format, 	"Number of laps", 	"%d", sess_data.number_of_laps);
	
	plotdata(format, 	"Session day", 		"%d", sess_data.session_day);
	plotdata(format, 	"Session month", 	"%d", sess_data.session_month);
	plotdata(format, 	"Session year", 	"%d", sess_data.session_year);
	plotdata(format, 	"Session start hour", 	"%d", sess_data.session_start_hour);
	plotdata(format, 	"Session start minute", "%d", sess_data.session_start_minute);
	plotdata(format, 	"Session start second", "%d", sess_data.session_start_second);

	plotdata(format, 	"Session length in seconds", 	"%.1f", sess_data.session_length);

	plotdata(format, 	"Average heart rate", 	"%d", sess_data.session_avg_hr);
	plotdata(format, 	"Max heart rate", 	"%d", sess_data.session_max_hr);
	plotdata(format, 	"Max set heart rate", 	"%d", sess_data.session_max_set_hr);
	plotdata(format, 	"Calories", 		"%d", sess_data.session_calories);

	plotheader(format, 	"Limiter Zones");
	plotdata(format,	"Limiter type",		"%d", (int)sess_data.limit_type);
	
	for (s = 0; s < 4; s++) {
		if (sess_data.limit_type == PACE_LIMITS) {
			plotpacedata(format, "Pace zone low (in sec per km)", sess_data.limiterzones[s].limit_low, PACE_FORMAT_SEC_PER_KM);
			plotpacedata(format, "Pace zone low (in sec per km)", sess_data.limiterzones[s].limit_high, PACE_FORMAT_SEC_PER_KM);
		} else if (sess_data.limit_type == HR_LIMITS) {
			plotzonedata(format, "Limiter zone", s+1, "BPM limit low", sess_data.limiterzones[s].limit_low);
			plotzonedata(format, "Limiter zone", s+1, "BPM limit high", sess_data.limiterzones[s].limit_high);
		} else {
			plotzonedata(format, "Limiter zone", s+1, "SportZone", sess_data.limiterzones[s].sport_zone);
		}
	}

	// time spent on each zone
	for (s = 0; s < 4; s++) {
		plotdata(format, 	"Time below zone seconds", 	"%.1f", sess_data.limiterzones[s].below_zone_seconds);
		plotdata(format, 	"Time on zone seconds", 	"%.1f", sess_data.limiterzones[s].on_zone_seconds);
		plotdata(format, 	"Time above zone seconds", 	"%.1f", sess_data.limiterzones[s].above_zone_seconds);
	}

	for (s = 0; s < 5; s++) {
		plotzonedata(format, 	"Sport zone", s+1, 		"low%", sess_data.sportzones[s].low_percent);
		plotzonedata(format, 	"Sport zone", s+1, 		"high%", sess_data.sportzones[s].high_percent);
	}
	for (s = 0; s < 5; s++) {
		plotdata(format, 	"On SportZone seconds", 	"%.1f", sess_data.sportzones[s].on_zone_seconds);
	}

	plotheader(format, 		"Lap summary");
	plotdata(format,		"Total distance in meters", 	"%d", sess_data.session_total_distance);
	plotpacedata(format, 		"Max pace (in sec per km)", 	sess_data.session_max_pace, PACE_FORMAT_SEC_PER_KM);
	plotpacedata(format, 		"Avg pace (in sec per km)", 	sess_data.session_avg_pace, PACE_FORMAT_SEC_PER_KM);
	plotdata(format, 		"Best lap", 			"%d", sess_data.session_best_lap);
	plotdata(format, 		"Best lap time",	 	"%.1f", sess_data.session_best_laptime);

	plotheader(format, 		"Laps");
	for (s = 0;s < sess_data.number_of_laps;s++) {

		plotheader(format, "");
		plotdata(format, 	"Lap no",			"%d", sess_data.laps[s]->lap_no);
		plotdata(format, 	"Lap end time",			"%.1f", sess_data.laps[s]->lap_end_time);
		plotdata(format, 	"Laptime seconds",		"%.1f", sess_data.laps[s]->laptime);
		plotdata(format, 	"Autolap", 			"%d", sess_data.laps[s]->autolap);
		plotdata(format, 	"End pulse",			"%d", sess_data.laps[s]->end_hr);
		plotdata(format, 	"Avg pulse",			"%d", sess_data.laps[s]->avg_hr);
		plotdata(format, 	"Max pulse",			"%d", sess_data.laps[s]->max_hr);

		plotpacedata(format, 	"End pace (in sec / km)", 	sess_data.laps[s]->end_pace, PACE_FORMAT_SEC_PER_KM);
		plotpacedata(format, 	"Avg pace (in sec / km)", 	sess_data.laps[s]->avg_pace, PACE_FORMAT_SEC_PER_KM);
		plotdata(format, 	"Lap length (in m)", "%d", 	sess_data.laps[s]->lap_length*10);
	}
	plotheader(format, "END OF DATA");
}



/* ------------------------------------------------------------------------------------------------------
  Prints out session data as xml
--------------------------------------------------------------------------------------------------------- */

void session_as_xml(session_data sess_data)
{
	int s;

	print_debug("Printing xml data.");

	xmlstarttag("rs200_session", 0, "id", "%s", sess_data.session_id);
	xmlstarttag("user", 1, NULL, NULL);
	xmlvaluetag("weight", 2, "%d", sess_data.user_weight);
	xmlvaluetag("height", 2, "%d", sess_data.user_height);
	xmlvaluetag("birth_date", 2, "%s", sess_data.user_birth_date); 
	xmlvaluetag("max_hr", 2, "%d", sess_data.user_max_hr);
	xmlvaluetag("vo_max", 2, "%d", sess_data.user_vo_max);
	xmlvaluetag("sitting_hr", 2, "%d", sess_data.user_sitting_hr);
	xmlvaluetag("gender", 2, "%s", sess_data.user_sex);
	xmlvaluetag("activity_level", 2, "%s", sess_data.user_activity_level);

	xmlendtag("user", 1);

	xmlstarttag("session_data", 1, NULL, NULL);

	xmlvaluetag("name", 2, "%s", sess_data.session_name);
	xmlvaluetag("day", 2, "%d", sess_data.session_day);
	xmlvaluetag("month", 2, "%d", sess_data.session_month);
	xmlvaluetag("year", 2, "%d", sess_data.session_year);
	xmlvaluetag("start_hour", 2, "%d", sess_data.session_start_hour);
	xmlvaluetag("start_minute", 2, "%d", sess_data.session_start_minute);
	xmlvaluetag("start_second", 2, "%d", sess_data.session_start_second);
	
	if (sess_data.has_pace_data) {
		xmlvaluetag("has_pace_data", 2, "%s", "TRUE");
	} else {
		xmlvaluetag("has_pace_data", 2, "%s", "FALSE");
	}

	xmlstarttag("summary", 2, NULL, NULL);
	xmlvaluetag("number_of_laps", 3, "%d", sess_data.number_of_laps);
	xmlvaluetag("total_distance", 3, "%d", sess_data.session_total_distance);
	xmlvaluetag("max_pace", 3, "%.1f", (double)sess_data.session_max_pace/10);
	xmlvaluetag("avg_pace", 3, "%.1f", (double)sess_data.session_avg_pace/10);
	xmlvaluetag("best_lap", 3, "%d", sess_data.session_best_lap);
	xmlvaluetag("best_laptime", 3, "%.1f", sess_data.session_best_laptime);
	xmlvaluetag("length", 3, "%.1f", sess_data.session_length);
	xmlvaluetag("avg_hr", 3, "%d", sess_data.session_avg_hr);
	xmlvaluetag("max_hr", 3, "%d", sess_data.session_max_hr);
	xmlvaluetag("max_set_hr", 3, "%d", sess_data.session_max_set_hr);
	xmlvaluetag("calories", 3, "%d", sess_data.session_calories);
	xmlendtag("summary", 2);

	xmlstarttag("limiterzones", 2, NULL, NULL);
	xmlvaluetag("limiter_type", 3, "%d", (int)sess_data.limit_type);
	
	for (s = 0; s < 4; s++) {
		xmlstarttag("limiterzone", 3, "zoneno", "%d", s+1);
		if (sess_data.limit_type == PACE_LIMITS) {
			xmlvaluetag("limit_low", 4, "%d", sess_data.limiterzones[s].limit_low/10);
			xmlvaluetag("limit_high", 4, "%d", sess_data.limiterzones[s].limit_high/10);
		} else if (sess_data.limit_type == HR_LIMITS) {
			xmlvaluetag("limit_low", 4, "%d", sess_data.limiterzones[s].limit_low);
			xmlvaluetag("limit_high", 4, "%d", sess_data.limiterzones[s].limit_high);
		} else {
			xmlvaluetag("limit_zone", 4, "%d", sess_data.limiterzones[s].sport_zone);
		}
		xmlvaluetag("time_below", 4, "%.1f", sess_data.limiterzones[s].below_zone_seconds);
		xmlvaluetag("time_on", 4, "%.1f", sess_data.limiterzones[s].on_zone_seconds);
		xmlvaluetag("time_above", 4, "%.1f", sess_data.limiterzones[s].above_zone_seconds);
		xmlendtag("limiterzone", 3);
	}

	xmlendtag("limiterzones", 2);
	xmlstarttag("sportzones", 2, NULL, NULL);
	for (s = 0; s < 5; s++) {
		xmlstarttag("sportzone", 3, "zoneno", "%d", s+1);
		xmlvaluetag("low_percent", 4, "%d", sess_data.sportzones[s].low_percent);
		xmlvaluetag("high_percent", 4, "%d", sess_data.sportzones[s].high_percent);
		xmlvaluetag("time_on", 4, "%.1f", sess_data.sportzones[s].on_zone_seconds);
		xmlendtag("sportzone", 3);
	}

	xmlendtag("sportzones", 2);

	xmlstarttag("laps", 2, NULL, NULL);
	for (s = 0;s < sess_data.number_of_laps;s++) {

		xmlstarttag("lap", 3, "lap_no", "%d", sess_data.laps[s]->lap_no);
		xmlvaluetag("lap_end_time", 4, "%.1f", sess_data.laps[s]->lap_end_time);
		xmlvaluetag("laptime", 4, "%.1f", sess_data.laps[s]->laptime);
		xmlvaluetag("autolap", 4, "%d", sess_data.laps[s]->autolap);
		xmlvaluetag("end_hr", 4, "%d", sess_data.laps[s]->end_hr);
		xmlvaluetag("avg_hr", 4, "%d", sess_data.laps[s]->avg_hr);
		xmlvaluetag("max_hr", 4, "%d", sess_data.laps[s]->max_hr);

		xmlvaluetag("end_pace", 4, "%.1f", (double)sess_data.laps[s]->end_pace/10);
		xmlvaluetag("avg_pace", 4, "%.1f", (double)sess_data.laps[s]->avg_pace/10);
		xmlvaluetag("lap_length", 4, "%d", sess_data.laps[s]->lap_length*10);
		xmlendtag("lap", 3);
	}
	xmlendtag("laps", 2);
	xmlendtag("session_data", 1);
	xmlendtag("rs200_session", 0);
}


/* ------------------------------------------------------------------------------------------------------
  Prints out follow-up data as a human-readable summary
--------------------------------------------------------------------------------------------------------- */

void followup_summary(followup_data followup)
{
	int format = OUTPUT_NORMAL;
	char time[16];
	char time2[16];
	int s, k;

	print_debug("Printing follow-up summary data.");

	plotheader(format, 	"FOLLOW-UP DATA");
	plotdata(format, 	"Decoding date", "%s", followup.decoding_date);

	// OwnIndex values
	plotheader(format, 	"OwnIndex history");
	for (s = 0; s < followup.number_of_indices; s++) {
		sprintf(time, "%02d.%02d.%d", followup.indices[s]->recorded_day, followup.indices[s]->recorded_month, followup.indices[s]->recorded_year); 
		plotdata(format, 	"OwnIndex", "%d (date %s)", followup.indices[s]->value, time);
	}

	// All-time totals
	plotheader(format, 	"Totals");

	sprintf(time, "%02d.%02d.%d", (int)followup.exercises_recording_start_day, (int)followup.exercises_recording_start_month, (int)followup.exercises_recording_start_year);
	plotdata(format, 	"Total sessions", "%d (recorded since %s)", followup.total_exercises, time);

	sprintf(time, "%02d.%02d.%d", followup.distance_recording_start_day, followup.distance_recording_start_month, followup.distance_recording_start_year);
	plotdata(format, 	"Total distance", "%.1f km (recorded since %s)", followup.total_distance, time);

	sprintf(time, "%02d.%02d.%d", followup.calories_recording_start_day, followup.calories_recording_start_month, followup.calories_recording_start_year);
	plotdata(format, 	"Total calories", "%.1f kcal (recorded since %s)", followup.total_calories, time);

	time_to_string(time, followup.total_duration);
	sprintf(time2, "%02d.%02d.%d", followup.duration_recording_start_day, followup.duration_recording_start_month, followup.duration_recording_start_year);
	plotdata(format, 	"Total duration", "%s (recorded since %s)", time, time2);

	plotdata(format, 	"All time SonicLink use", "%d", followup.all_time_soniclink_sends);
	plotdata(format, 	"All time session recordings", "%d", followup.all_time_number_of_sessions_stored);
	plotdata(format, 	"All time sessions with pod", "%d", followup.all_time_number_of_sessions_with_pod);
	plotdata(format, 	"All time tests performed", "%d", followup.all_time_number_of_tests_performed);


	// Weekly totals
	plotheader(format, 	"Weekly summaries");

	for (k = 0; k < followup.number_of_summaries; k++) {
		plotdata(format, 	"Week number", "%d", k + 1);
		time_to_string(time, followup.summaries[k]->sum_session_time);
		plotdata(format, 	"Total duration", "%s", time);
		plotdata(format, 	"Total distance", "%.2f km", followup.summaries[k]->sum_session_distance);
		plotdata(format, 	"Total calories", "%.1f kcal", followup.summaries[k]->sum_session_calories);
		plotdata(format, 	"Total sessions", "%d", followup.summaries[k]->number_of_sessions);

		for (s=0; s<5; s++) {
			time_to_string(time, followup.summaries[k]->sportzones[s].on_zone_seconds);
			plotzonestringdata(format, "Sport zone", s+1, "on this zone", time);
		}
		plotheader(format, 	"");

	}

	plotheader(format, "END OF DATA");
}


/* ------------------------------------------------------------------------------------------------------
   Prints out raw data without interpretation as comma separated values (for analysing fields etc.)
--------------------------------------------------------------------------------------------------------- */
void raw_data(int *data,int size)
{
	int i;
	int header_size = 11;
	char binary[9];
	print_debug("Will print %d bytes of data and %d bytes of preamble.", size, header_size);

	fprintf(get_output_stream(), "# Raw data as csv:\n#\n");
	fprintf(get_output_stream(), "# byte,dec,hex,binary,comment\n");
	for(i = 0;i < size+header_size;i++) {
		if (i<header_size)
			fprintf(get_output_stream(), "%3d,", 0);
		else
			fprintf(get_output_stream(), "%3d,", i-(header_size-1));

		byte_to_binary_str(binary, data[i]);
		fprintf(get_output_stream(), "%3d,%02X,%s",data[i],data[i],binary);

		if(i == 0)
			fprintf(get_output_stream(), (i < size - 1) ? ",section start" : ",no more sections");
		if(i == 1)
			fprintf(get_output_stream(), ",data format type");
		if(i == 3)
			fprintf(get_output_stream(), ",number of sections");
		if(i == 4)
			fprintf(get_output_stream(), ",number of data bytes (lsb)");
		if(i == 5)
			fprintf(get_output_stream(), ",number of data bytes (msb)");
		fprintf(get_output_stream(), "\n");
	}
}



/* ------------------------------------------------------------------------------------------------------
  Outputs a replica of the bytes (including headers and padding) from RS200 into a binary file
--------------------------------------------------------------------------------------------------------- */

void raw_binary_data(int *data,int size)
{
	int i;
	int preamble_size = 8;
	int section_max_size = 220;
	int section_size = 0;
	int written_size = 0;
	int counter = 0;
	int total_counter = 0;
	int section_counter = 0;

	print_debug("Outputting preamble.");

	for(i = 0;i < preamble_size;i++) {
		fputc(data[counter], get_output_stream());
		counter++;
	}

	while (counter < size) {
		section_counter++;
		print_debug("Outputting next section.");
		if (size-written_size > section_max_size)
			section_size = section_max_size;
		else
			section_size = size-written_size;
		written_size = written_size + section_size;

		// check if section header is missing from original data and if so, write it
		if (data[counter] != 85 || data[counter+1] != section_counter) {
			fputc(85, get_output_stream());
			fputc(section_counter, get_output_stream());
			fputc(section_size, get_output_stream());
			print_deep_debug("w: %d,header\n",85);
			print_deep_debug("w: %d,header\n",section_counter);
			print_deep_debug("w: %d,header\n",section_size);
		} else {
			fputc(data[counter++], get_output_stream());
			fputc(data[counter++], get_output_stream());
			fputc(data[counter++], get_output_stream());
			print_deep_debug("w: %d,header\n",data[counter-3]);
			print_deep_debug("w: %d,header\n",data[counter-2]);
			print_deep_debug("w: %d,header\n",data[counter-1]);
		}
		for(i = 0;i < section_size; i++) {
			fputc(data[counter], get_output_stream());
			print_deep_debug("w: %d,data\n",data[counter]);
			counter++;
		}
		// add padding
		total_counter = counter;
		for(i = section_size;i < RS200_SECTION_LENGTH-3; i++) {
			
			fputc(0, get_output_stream());
			print_deep_debug("w: %d,padding\n",0);
			total_counter++;
		}
	}
	// finish off with a trailer byte
	fputc(7, get_output_stream());
}
