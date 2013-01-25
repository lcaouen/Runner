// RS200.cpp : définit le point d'entrée pour l'application DLL.
//

#include "stdafx.h"
#include <windows.h>

#include "signal_functions.h"
#include "decoders.h"

extern char*	v_status;
extern double	v_level;
extern double	v_progress;

////////////////////////
// Méthodes exportées //
////////////////////////
__declspec (dllexport) char* DecodeRS200(char* in_strPath)
{
	return decode(in_strPath);
}

__declspec (dllexport) double GetRS200ProgressValue()
{
	return v_progress;
}

__declspec (dllexport) char* GetRS200Satus()
{
	return  v_status;
}

__declspec (dllexport) double GetRS200LevelValue()
{
	return v_level;
}

/////////////
// DllMain //
/////////////
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}
