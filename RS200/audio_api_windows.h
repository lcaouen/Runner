/* ------------------------------------------------------------------------------------------------------
   audio_api_windows.h
--------------------------------------------------------------------------------------------------------- */

#ifndef AUDIO_API_WINDOWS_H
#define AUDIO_API_WINDOWS_H

#include "stdafx.h"
#include <Windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include "signal_functions.h"

extern int open_audio();
extern int close_audio();
extern int next_sample(int);

#endif
