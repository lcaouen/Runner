/* ------------------------------------------------------------------------------------------------------
   Functions to the Windows audio API
   Uses the winmm.lib library
--------------------------------------------------------------------------------------------------------- */


#include "stdafx.h"
#include "audio_api_windows.h"

/* Common data structures managed by the recorder and the watcher thread */
HWAVEIN				WaveInHandle = NULL;

BOOL				InRecord = FALSE;
BOOL				bufferReady = FALSE;

WAVEHDR				WaveHeader[2];
int					bufferSize = 0;
char				*currentBuffer = NULL;


/* ------------------------------------------------------------------------------------------------------
   Recording watcher thread
   Manager recording buffers and makes sure there is always data available for analyzing
--------------------------------------------------------------------------------------------------------- */

DWORD WINAPI waveInProc(LPVOID arg)
{
	MSG		msg;

	while (GetMessage(&msg, 0, 0, 0) == 1) {
		switch (msg.message) {
			/* A buffer has been filled by the driver */
			case MM_WIM_DATA:
			{
				// msg.lParam contains a pointer to the WAVEHDR structure for the filled buffer.
				if (((WAVEHDR *)msg.lParam)->dwBytesRecorded) {
					bufferReady = TRUE;
					currentBuffer = ((WAVEHDR *)msg.lParam)->lpData;
				}

				/* Are we still recording? If yes, rotate the current buffer to the end and reuse it*/
				if (InRecord) {
					waveInAddBuffer(WaveInHandle, (WAVEHDR *)msg.lParam, sizeof(WAVEHDR));
				}

				/* Keep waiting for more messages */
                continue;
			}

			// Still opening the WAVE device
			case MM_WIM_OPEN:
			{
                continue;
			}

			// Closing the WAVE device
			case MM_WIM_CLOSE:
			{
				// Terminate this thread
				print_deep_debug("Closing recording watch thread.\n");
				break;
			}
		}
	}
	return(0);
}


/* ------------------------------------------------------------------------------------------------------
   Open and close functions for audio
--------------------------------------------------------------------------------------------------------- */
int close_audio()
{
	MMRESULT	err = 0;

	InRecord = FALSE;
	if (WaveInHandle != NULL) {
		if (waveInReset(WaveInHandle))
			fail("ERROR: cannot reset wave handle.");
		waveInPrepareHeader(WaveInHandle, &WaveHeader[1], sizeof(WAVEHDR));
		waveInPrepareHeader(WaveInHandle, &WaveHeader[0], sizeof(WAVEHDR));
		waveInClose(WaveInHandle);

		// Free allocated memory
		if (WaveHeader[0].lpData) VirtualFree(WaveHeader[0].lpData, 0, MEM_RELEASE);

		print_debug("Microphone closed.");
	}

	WaveInHandle = NULL;
	InRecord = FALSE;
	bufferReady = FALSE;
	ZeroMemory(&WaveHeader[0], sizeof(WAVEHDR) * 2);
	bufferSize = 0;
	currentBuffer = NULL;

	return 0;
}


int open_audio()
{
	MMRESULT	err = 0;
	WAVEFORMATEX	waveFormat;
	HANDLE		waveInThread;
	DWORD		threadId;

	print_debug("Opening microphone for recording.");

	// open record buffer monitoring and switching thread
	waveInThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)waveInProc, 0, 0, &threadId);
	if (!waveInThread) {
		fail("Can't create WAVE recording thread!\n");
		return(-1);
	}
	CloseHandle(waveInThread);

	ZeroMemory(&WaveHeader[0], sizeof(WAVEHDR) * 2);

	// Initialize the WAVEFORMATEX for 16-bit, 44KHz, mono
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 1;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample/8);
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// open recording handle
	if (err = waveInOpen(&WaveInHandle, WAVE_MAPPER, &waveFormat, threadId, 0, CALLBACK_THREAD))
		fail("ERROR opening recording handle.");

	// allocate memory for buffers (a continuous memory block for two buffers)
	// half a second per buffer should be enough, any longer and it makes recording "jerky"
	WaveHeader[1].dwBufferLength = WaveHeader[0].dwBufferLength = waveFormat.nAvgBytesPerSec / 2;
	bufferSize = WaveHeader[1].dwBufferLength;
	if (!(WaveHeader[0].lpData = (char *)VirtualAlloc(0, WaveHeader[0].dwBufferLength * 2, MEM_COMMIT, PAGE_READWRITE)))
		fail("ERROR: Can't allocate memory for WAVE buffer!\n");

	// set the second buffer to point to half way of allocated memory
	WaveHeader[1].lpData = WaveHeader[0].lpData + WaveHeader[0].dwBufferLength;

	// Prepare two WAVEHDR's and add them as buffers
	waveInPrepareHeader(WaveInHandle, &WaveHeader[0], sizeof(WAVEHDR));
	waveInPrepareHeader(WaveInHandle, &WaveHeader[1], sizeof(WAVEHDR));
	waveInAddBuffer(WaveInHandle, &WaveHeader[0], sizeof(WAVEHDR));
	waveInAddBuffer(WaveInHandle, &WaveHeader[1], sizeof(WAVEHDR));
	
	// start recording
	if (err = waveInStart(WaveInHandle)) {
		fail("ERROR: Cannot start recording.\n");
	}
	InRecord = TRUE;
	return 1;
}


/* ------------------------------------------------------------------------------------------------------
   Reads the next 16 bit sample from the input, opens input if required
--------------------------------------------------------------------------------------------------------- */
int next_sample(int new_file)
{
	static FILE *fp = NULL;
	static int audioBufferCounter = 0;
	static int useAudio = 0;

	int c1=0, c2=0;
	errno_t err = 0;


	if (new_file) {
		print_debug("Closing old file and opening next.");
		if (fp) fclose(fp);
		fp = NULL;
		audioBufferCounter = 0;
		useAudio = 0;
	}

	if(fp == NULL && useAudio == 0) {

		if (!get_read_from_dsp() && !(get_input_file_name() != NULL && strlen(get_input_file_name()) > 0) && !(get_dsp_device() != NULL && strlen(get_dsp_device()) > 0)) {
			print_debug("Using stdin as input");
			fp = stdin;
		}
		else if (get_read_from_dsp()) {
			print_debug("Opening audio device for input");
			open_audio();
			useAudio = 1;
		}
		else if (get_input_file_name() != NULL && strlen(get_input_file_name()) > 0) {
			print_debug("Opening %s for input (len %d)", get_input_file_name(), strlen(get_input_file_name()));
			fp = fopen(get_input_file_name(),"rb");
		} else {
			fail("No input channel specified.");
		}
		if (fp == NULL && !useAudio) fail("Unable to open sound input. Check file name/path or device name and permissions.");
		
		if (useAudio) {
			audioBufferCounter += 44;
		} else {
			for(c1 = 0;c1 < 44;c1++) // skip the first 44 bytes (size of the header of a wav file)
				if(getc(fp) == EOF) {
					print_debug("Hit EOF");
					return 40000;
				}
		}
	}

	// read bytes from file (fp) or memory wave buffer (currentBuffer)
	if (fp != NULL) {
		c1 = getc(fp);
		c2 = getc(fp);
	} else if (currentBuffer != NULL) {
		// buffer not ready yet, so wait
bufferLoop:
		if (bufferReady) {
			c1 = currentBuffer[audioBufferCounter];
			c2 = currentBuffer[audioBufferCounter + 1];
			audioBufferCounter += 2;
			if (audioBufferCounter > bufferSize) {
				bufferReady = FALSE;
				audioBufferCounter = 0;
			}
		} else {
			Sleep(2);
			goto bufferLoop;
		}
	}

	if (fp != NULL && feof(fp)) {
		print_debug("Hit EOF");
		return 40000;
	}
	c1 += c2 << 8;
	if(c1 > 32767) c1 -= 65536;
	// print_deep_debug("Byte %d.\n", c1);
	return c1;
}

