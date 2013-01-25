/* ------------------------------------------------------------------------------------------------------
   signal_functions.c

   This file contains functions needed for analysing the SonicLink waveform and
   converting it to bytes.
------------------------------------------------------------------------------------------------------- */

#include "stdafx.h"
#include "signal_functions.h"

static TCHAR *input_file_name = NULL;
static char *dsp_device = NULL;
static char read_from_dsp = 0;

/* ------------------------------------------------------------------------------------------------------
   pass input signal through a (linear phase FIR) high-pass filter
   y = filter(remez(40,[0 2000 4000 22050]/22050,[0 0 1 1],[2 1]),1,x);
--------------------------------------------------------------------------------------------------------- */
double filter(double x)
{
	static double h[41] =
	{
	0.00379802504960,-0.01015567605831,-0.00799539667861,-0.00753846964193,
	-0.00613119820747,-0.00283788804837, 0.00237999784896, 0.00894445674876,
	0.01562664364293, 0.02085836025169, 0.02288210087128, 0.02006200096880,
	0.01129559749322,-0.00374264061795,-0.02440163523552,-0.04905850773028,
	-0.07526471722031,-0.10006201874319,-0.12045502068751,-0.13386410688608,
	0.86146086608477,-0.13386410688608,-0.12045502068751,-0.10006201874319,
	-0.07526471722031,-0.04905850773028,-0.02440163523552,-0.00374264061795,
	0.01129559749322, 0.02006200096880, 0.02288210087128, 0.02085836025169,
	0.01562664364293, 0.00894445674876, 0.00237999784896,-0.00283788804837,
	-0.00613119820747,-0.00753846964193,-0.00799539667861,-0.01015567605831,
	0.00379802504960
	};
	static double input_signal[64];
	static int pos = -1;
	double y;
	int i;

	if(pos == -1) {
		pos = 0;
		for(i = 0;i < 64;i++) input_signal[i] = 0.0;
	}
	input_signal[pos] = x;
	y = 0.0;
	for(i = 0;i <= 40;i++) y += h[i] * input_signal[(pos - i) & 63];
	pos = (pos + 1) & 63;
	return y;
}


/* ------------------------------------------------------------------------------------------------------
   dilate the rectified signal
   y = delay(3,ordfilt2(abs(x),7,ones(1,7)));
--------------------------------------------------------------------------------------------------------- */
double dilate(double x)
{
	static double input_signal[8];
	static int pos = -1;
	double y;
	int i;

	if(pos == -1) {
		pos = 0;
		for(i = 0;i < 8;i++)
			input_signal[i] = 0.0;
	}
	input_signal[pos] = (x >= 0.0) ? x : -x;
	y = input_signal[pos];
	for(i = 1;i <= 6;i++)
		if(input_signal[(pos - i) & 7] > y) y = input_signal[(pos - i) & 7];
	pos = (pos + 1) & 7;
	return y;
}


/* ------------------------------------------------------------------------------------------------------
   apply a median filter to the signal
   y = delay(2,ordfilt2(x,3,ones(1,5)));
--------------------------------------------------------------------------------------------------------- */
double median(double x)
{
	static double input_signal[8];
	static int pos = -1;
	double t,y[5];
	int i,j;

	if(pos == -1) {
		pos = 0;
		for(i = 0;i < 8;i++)
			input_signal[i] = 0.0;
	}
	input_signal[pos] = x;
	for(i = 0;i <= 4;i++) { // sort
		t = input_signal[(pos - i) & 7];
		for(j = 0;j < i;j++)
		if(t < y[j]) {
			y[i] = y[j];
			y[j] = t;
			t = y[i];
		}
		y[i] = t;
	}
	pos = (pos + 1) & 7;
	return y[2];
}


/* ------------------------------------------------------------------------------------------------------
   decide if each signal sample corresponds to part of a one or not

   y = x;
   for k=2:length(x)
     y(k) = max(x(k),0.9999*y(k-1));
   end
   amplitude = delay(20,ordfilt2(y,41,ones(1,41)));
   decisions = [ delay(20,x) >= level*amplitude  amplitude >= 15*delay(30,amplitude) ]);
--------------------------------------------------------------------------------------------------------- */
double make_decision(double x,int *decisions)
{ // decisions[0..4] = above/below threshold for five different threshold levels
  // decisions[5] = best above/below decision based on a majority rule
  // decisions[6] = abrupt change/no abrupt change in the amplitude (start flag)
	static double threshold_levels[5] = { 0.25,0.30,0.35,0.40,0.45 };
	static double input_signal[64],amplitude[64];
	static int pos = -1,count_down;
	double t;
	int i;

	if(pos == -1) {
		pos = 0;
		count_down = 64;
		for(i = 0;i < 64;i++) {
			input_signal[i] = 0.0;
			amplitude[i] = 0.0;
		}
	}
	// compute the amplitude
	input_signal[pos] = x;
	amplitude[pos] = 0.9999 * amplitude[(pos - 1) & 63];
	if(x > amplitude[pos]) amplitude[pos] = x;
	// dilate the amplitude
	t = amplitude[pos];
	for(i = 1;i <= 40;i++)
		if(amplitude[(pos - i) & 63] > t) t = amplitude[(pos - i) & 63];	
	// compare the (delayed) signal with the (dilated) amplitude scaled by a threshold level
	decisions[5] = 0;
	for(i = 0;i < 5;i++) {
		decisions[i] = (input_signal[(pos - 20) & 63] >= threshold_levels[i] * t) ? 1 : 0;
		decisions[5] += decisions[i];
	}
	decisions[5] = (decisions[5] >= 3) ? 1 : 0; // majority rule (median)
	decisions[6] = (amplitude[pos] >= 15.0 * amplitude[(pos - 30) & 63]) ? 1 : 0;
	if(count_down > 0) {
 		--count_down;
		decisions[6] = 0;
	}
	i = pos;
	pos = (pos + 1) & 63;
	return amplitude[i];
}


/* ------------------------------------------------------------------------------------------------------
   apply a median filter to the decisions
   d = delay(2,ordfilt2(d,3,ones(1,5)));
--------------------------------------------------------------------------------------------------------- */
void b_median(int *d)
{
	static int inputs[8][8];
	static int pos = -1;
	int i,j;

	if(pos == -1) {
		pos = 0;
		for(i = 0;i < 8;i++)
			for(j = 0;j < 8;j++)
				inputs[i][j] = 0;
	}
	for(i = 0;i < 7;i++) {
		inputs[i][pos] = d[i];
		for(j = 1;j <= 4;j++)
			d[i] += inputs[i][(pos - j) & 7];
		d[i] = (d[i] >= 3) ? 1 : 0;  // median (majority rule)
	}
	pos = (pos + 1) & 7;
}



/* ------------------------------------------------------------------------------------------------------
   Creates a byte from the waveform

   0..255  new byte
   -1      continue
   -2      error
   -3      no more bytes
--------------------------------------------------------------------------------------------------------- */
int byte_decode(int restart, int new_file)
{
	static int t,t_active,t_byte;
	static int byte,n_bytes,c[6];
	int i,j,k,d[7],sampleval;
	double x;

	// t = number of samples read
	// t_byte = sample where byte starts
	// t_active is > 0 when a signal has been detected

	if(restart) {
		t = t_active = 0;
		update_progress(0.0);
		update_status("Attente du signal...");
	}

	// process next sample
	t++;
	sampleval = next_sample(new_file);
	
	if (sampleval == 40000) {
		return -3;
	}
	x = (double)sampleval / 32768.0;
	x = filter(x);
	x = dilate(x);
	x = median(x);
	x = make_decision(x,d);
	b_median(d);
	update_level(x);

	// print_deep_debug("t: %d, d6 %d, d5 %d, x %0.5f\n", t, d[6], d[5], x);
	// activation test
	if(d[6] && t_active == 0) {
		print_deep_debug(" - act - ");
		t_active = t;
		t_byte = 0;
		n_bytes = 0;
		update_status("Analyse du signal...");
	}
	if(t_active == 0) return -1;

	// deactivation test after a long period of inactivity
	if(t > t_active + 10000) {
		update_status("Désactivation pour cause d'inactivité");
		return -2;
	}

	// new byte?
	if(t_byte == 0 && d[5]) { // new byte
		print_deep_debug(" -- new byte -- ");
		t_active = t_byte = t;
		n_bytes++;
		byte = 0;
		for(i = 0;i < 6;i++)
			c[i] = 0;
	}

	// processing a byte?
	if(t_byte > 0) {
		j = (t - t_byte) / 88;  // bit number; the duration of a bit is very close to 2ms=88.2 samples
		if(j >= 10) { // end of the byte
			print_deep_debug(" [b = %d ] ", byte);
			if((byte & 1) == 0) { // first bit (lsb) must be a one
				print_debug("Bad byte start");
				return -2;
			}
			if(byte >= 512) { // last bit (msb) must be a zero
				print_debug("Bad byte finish");
				return -2;
			}
			byte >>= 1;
			t_byte = 0;
			if(n_bytes > 5) {
				return byte;
			}
			if(byte != 0xAA) { // bad synchronization byte
				print_debug("Bad synchronization byte.");
				return -2;
			}
			print_debug("Found good synchronization byte.");
			return -1; // discard synchronization byte
		}
		k = t - t_byte - 88 * j;  // sample number inside the bit
		if (k < 64) // the bit burst has a duration of close to 60 samples (here we use 64 but latter we use 60...)
		for(i = 0;i < 6;i++)
			c[i] += d[i];
		else if(k == 64) {
			k = 0;
			for(i = 0;i < 6;i++) {
				if(c[i] >= 30) // threshold = 60/2
					k += (i < 5) ? 1 : 2;
				c[i] = 0;
			}
			if(k >= 4) // majority rule
			byte += 1 << j;
		}
	}
	return -1;
}


/* ------------------------------------------------------------------------------------------------------
   Getter and setter functions for variables related to input reading
--------------------------------------------------------------------------------------------------------- */
// void set_input_file_name(char *value)
void set_input_file_name(TCHAR *value)
{
	input_file_name = value;
}

// char * get_input_file_name()
TCHAR * get_input_file_name()
{
	return input_file_name;
}

void set_dsp_device(char *value)
{
	dsp_device = value;
}

char * get_dsp_device()
{
	return dsp_device;
}

void set_read_from_dsp(char value)
{
	read_from_dsp = value;
}

char get_read_from_dsp()
{
	return read_from_dsp;
}

