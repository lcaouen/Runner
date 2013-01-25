/*
	Definitions of internal data structures and helpful variables
*/

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <windows.h>

// Summary data output formats
#define OUTPUT_NORMAL 0
#define OUTPUT_CSV 1
#define OUTPUT_CSV_PRETTY 2

#define INPUT_WAV 0
#define INPUT_BINARY 1
#define INPUT_ASCII 2

#define TRUE 1
#define FALSE 0

#define NO_LIMITS 0
#define HR_LIMITS 1
#define PACE_LIMITS 2

typedef char boolean;

typedef struct LAP {
	int			lap_no;
	double		lap_end_time;
	double 		laptime;
	boolean 	autolap;
	int			end_hr;
	int			avg_hr;
	int			max_hr;
	int			end_pace;
	int			avg_pace;
	int			lap_length;
} lap;

typedef struct SPORTZONE {
	int			zone_number;
	int			low_percent;
	int			high_percent;
	double		on_zone_seconds;
} sportzone;

typedef struct LIMITERZONE {
	int			limit_low;
	int			limit_high;
	int			sport_zone;
	double		below_zone_seconds;
	double		on_zone_seconds;
	double		above_zone_seconds;
} limiterzone;

typedef struct SESSION_DATA {
	char		session_id[31];
	char		session_name[9];
	int			user_weight;
	int			user_height;
	char		user_birth_date[11];
	int			user_max_hr;
	int			user_vo_max;
	int			user_sitting_hr;
	char		user_sex[7];
	char		user_activity_level[9];
	char		limit_type;
	boolean		has_pace_data;
	int			number_of_laps;
	int			session_day;
	int			session_month;
	int			session_year;
	int			session_start_hour;
	int			session_start_minute;
	int			session_start_second;
	double		session_length;
	int			session_avg_hr;
	int			session_max_hr;
	int			session_max_set_hr;
	int			session_calories;
	int			session_total_distance;
	int			session_max_pace;
	int			session_avg_pace;	
	int			session_best_lap;
	double		session_best_laptime;
	sportzone 	sportzones[5];
	limiterzone limiterzones[5];
	lap 		**laps;
} session_data;


typedef struct OWNINDEX {
	short		value;
	short		recorded_day;
	short		recorded_month;
	int			recorded_year;
} ownIndex;

typedef struct WEEKLY_SUMMARY {
	sportzone 	sportzones[5];
	double		sum_session_time;
	double		sum_session_distance;
	double		sum_session_calories;
	short		number_of_sessions;
} weekly_summary;

typedef struct FOLLOWUP_DATA {
	char		decoding_date[11];
	int			all_time_soniclink_sends;
	int			all_time_number_of_sessions_stored;
	int			all_time_number_of_sessions_with_pod;
	int			all_time_number_of_tests_performed;
	double		total_duration;
	short		duration_recording_start_day;
	short		duration_recording_start_month;
	short		duration_recording_start_year;
	double		total_calories;
	short		calories_recording_start_day;
	short		calories_recording_start_month;
	short		calories_recording_start_year;
	int			total_exercises;
	short		exercises_recording_start_day;
	short		exercises_recording_start_month;
	short		exercises_recording_start_year;
	double		total_distance;
	short		distance_recording_start_day;
	short		distance_recording_start_month;
	short		distance_recording_start_year;
	short		number_of_indices;
	short		number_of_summaries;
	weekly_summary 	**summaries;
	ownIndex	**indices;
} followup_data;

#endif

