// (c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc
// MIT License
// Project Home: https://github.com/voidqk/sndfilter

// main entry point, used as a simple demo of the features of the library

#include "wav.h"
#include "biquad.h"
#include "compressor.h"
#include <stdio.h>
#include <string.h>

static int printabout(){
	printf(
		"sndfilter - simple demonstrations of common sound filters\n"
		"(c) Copyright 2016, Sean Connelly (@voidqk), http://syntheti.cc\n"
		"MIT License\n"
		"Project Home: https://github.com/voidqk/sndfilter\n");
	return 0;
}

static int printhelp(){
	printabout();
	printf("\n"
		"Usage:\n"
		"  sndfilter input.wav output.wav <filter> <...>\n"
		"\n"
		"Where:\n"
		"  input.wav    Input WAV file to process\n"
		"  output.wav   Output WAV file of filtered results\n"
		"  <filter>     One of the available filters (see below)\n"
		"  <...>        Additional parameters for the particular filter\n"
		"\n"
		"  Filters:\n"
		"    lowpass     Passes low frequencies through and dampens high frequencies\n"
		"    highpass    Passes high frequencies through and dampens low frequencies\n"
		"    bandpass    Passes a band of frequencies through and dampens outside them\n"
		"    notch       Dampens the overtones surrounding a frequency\n"
		"    allpass     Phase shifts based on frequency (usually not audible)\n"
		"    peaking     Adds gain to the overtones surrounding a frequency\n"
		"    lowshelf    Adds gain to lower frequencies\n"
		"    highshelf   Adds gain to higher frequencies\n"
		"    compressor  Dyanmic range compression, usually to make sounds louder\n"
		"\n"
		"  Filter Details:\n"
		"    lowpass <cutoff> <resonance>\n"
		"      cutoff     Frequency of the filter cutoff (Hz)\n"
		"      resonance  Boost applied to overtones near cutoff\n"
		"\n"
		"    highpass <cutoff> <resonance>\n"
		"      cutoff     Frequency of the filter cutoff (Hz)\n"
		"      resonance  Boost applied to overtones near cutoff\n"
		"\n"
		"    bandpass <freq> <Q>\n"
		"      freq       Center freqency of the overtones passed through (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"\n"
		"    notch <freq> <Q>\n"
		"      freq       Center frequency of the attenuated overtones (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"\n"
		"    peaking <freq> <Q> <gain>\n"
		"      freq       Center frequency where the gain occurs (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"      gain       Amount of gain (dB)\n"
		"\n"
		"    allpass <freq> <Q>\n"
		"      freq       Frequency where the phase shift crosses 90 degrees (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"\n"
		"    lowshelf <freq> <Q> <gain>\n"
		"      freq       Frequency where the low shelf stops producing gain (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"      gain       Amount of gain of the shelf (dB)\n"
		"\n"
		"    highshelf <freq> <Q> <gain>\n"
		"      freq       Frequency where the high shelf stops producing gain (Hz)\n"
		"      Q          Inverse of the fractional bandwidth\n"
		"      gain       Amount of gain of the shelf (dB)\n"
		"\n"
		"    compressor <pregain> <threshold> <knee> <ratio> <attack> <release>\n"
		"      pregain    Decibel amount to perform gain before compression (0 - 40)\n"
		"      threshold  Decibel level that triggers the compression (-100 to 0)\n"
		"      knee       Decibel width of the knee (0 - 40)\n"
		"      ratio      Ratio of compression after the threshold (1 - 20)\n"
		"      attack     Seconds for the compression to kick in (0 - 1)\n"
		"      release    Seconds for the compression to release (0 - 1)\n");
	return 0;
}

static inline bool getargs(int argc, char **argv, float *v1, float *v2, float *v3, float *v4,
	float *v5, float *v6){
	if (v1){
		if (argc <= 4)
			return false;
		*v1 = atof(argv[4]);
	}
	if (v2){
		if (argc <= 5)
			return false;
		*v2 = atof(argv[5]);
	}
	if (v3){
		if (argc <= 6)
			return false;
		*v3 = atof(argv[6]);
	}
	if (v4){
		if (argc <= 7)
			return false;
		*v4 = atof(argv[7]);
	}
	if (v5){
		if (argc <= 8)
			return false;
		*v5 = atof(argv[8]);
	}
	if (v6){
		if (argc <= 9)
			return false;
		*v6 = atof(argv[9]);
	}
	return true;
}

static inline int badargs(const char *filter){
	fprintf(stderr, "Error: Bad arguments for %s\n", filter);
	return 1;
}

typedef sf_snd (*filter2_func)(sf_snd snd, float A, float B);
static int filter2(const char *input, const char *output,
	filter2_func func, float A, float B){
	sf_snd s1 = sf_wavload(input);
	if (s1 == NULL){
		fprintf(stderr, "Error: Failed to load WAV: %s\n", input);
		return 1;
	}
	sf_snd s2 = func(s1, A, B);
	sf_snd_free(s1);
	if (s2 == NULL){
		fprintf(stderr, "Error: Failed to apply filter\n");
		return 1;
	}
	bool res = sf_wavsave(s2, output);
	sf_snd_free(s2);
	if (!res){
		fprintf(stderr, "Error: Failed to save WAV: %s\n", output);
		return 1;
	}
	return 0;
}

typedef sf_snd (*filter3_func)(sf_snd snd, float A, float B, float C);
static int filter3(const char *input, const char *output,
	filter3_func func, float A, float B, float C){
	sf_snd s1 = sf_wavload(input);
	if (s1 == NULL){
		fprintf(stderr, "Error: Failed to load WAV: %s\n", input);
		return 1;
	}
	sf_snd s2 = func(s1, A, B, C);
	sf_snd_free(s1);
	if (s2 == NULL){
		fprintf(stderr, "Error: Failed to apply filter\n");
		return 1;
	}
	bool res = sf_wavsave(s2, output);
	sf_snd_free(s2);
	if (!res){
		fprintf(stderr, "Error: Failed to save WAV: %s\n", output);
		return 1;
	}
	return 0;
}

typedef sf_snd (*filter6_func)(sf_snd snd, float A, float B, float C, float D, float E, float F);
static int filter6(const char *input, const char *output,
	filter6_func func, float A, float B, float C, float D, float E, float F){
	sf_snd s1 = sf_wavload(input);
	if (s1 == NULL){
		fprintf(stderr, "Error: Failed to load WAV: %s\n", input);
		return 1;
	}
	sf_snd s2 = func(s1, A, B, C, D, E, F);
	sf_snd_free(s1);
	if (s2 == NULL){
		fprintf(stderr, "Error: Failed to apply filter\n");
		return 1;
	}
	bool res = sf_wavsave(s2, output);
	sf_snd_free(s2);
	if (!res){
		fprintf(stderr, "Error: Failed to save WAV: %s\n", output);
		return 1;
	}
	return 0;
}

static sf_snd compdefault(sf_snd snd, float pregain, float threshold, float knee, float ratio,
	float attack, float release){
	sf_compressor_params_st params = sf_compressor_defaults;
	params.pregain = pregain;
	params.threshold = threshold;
	params.knee = knee;
	params.ratio = ratio;
	params.attack = attack;
	params.release = release;
	return sf_compressor(snd, params);
}

int main(int argc, char **argv){
	if (argc < 4)
		return printhelp();

	const char *input  = argv[1];
	const char *output = argv[2];
	const char *filter = argv[3];

	float A, B, C, D, E, F;
	if (strcmp(filter, "lowpass") == 0){
		if (!getargs(argc, argv, &A, &B, NULL, NULL, NULL, NULL))
			return badargs(filter);
		return filter2(input, output, sf_lowpass, A, B);
	}
	else if (strcmp(filter, "highpass") == 0){
		if (!getargs(argc, argv, &A, &B, NULL, NULL, NULL, NULL))
			return badargs(filter);
		return filter2(input, output, sf_highpass, A, B);
	}
	else if (strcmp(filter, "bandpass") == 0){
		if (!getargs(argc, argv, &A, &B, NULL, NULL, NULL, NULL))
			return badargs(filter);
		return filter2(input, output, sf_bandpass, A, B);
	}
	else if (strcmp(filter, "notch") == 0){
		if (!getargs(argc, argv, &A, &B, NULL, NULL, NULL, NULL))
			return badargs(filter);
		return filter2(input, output, sf_notch, A, B);
	}
	else if (strcmp(filter, "peaking") == 0){
		if (!getargs(argc, argv, &A, &B, &C, NULL, NULL, NULL))
			return badargs(filter);
		return filter3(input, output, sf_peaking, A, B, C);
	}
	else if (strcmp(filter, "allpass") == 0){
		if (!getargs(argc, argv, &A, &B, NULL, NULL, NULL, NULL))
			return badargs(filter);
		return filter2(input, output, sf_allpass, A, B);
	}
	else if (strcmp(filter, "lowshelf") == 0){
		if (!getargs(argc, argv, &A, &B, &C, NULL, NULL, NULL))
			return badargs(filter);
		return filter3(input, output, sf_lowshelf, A, B, C);
	}
	else if (strcmp(filter, "highshelf") == 0){
		if (!getargs(argc, argv, &A, &B, &C, NULL, NULL, NULL))
			return badargs(filter);
		return filter3(input, output, sf_highshelf, A, B, C);
	}
	else if (strcmp(filter, "compressor") == 0){
		if (!getargs(argc, argv, &A, &B, &C, &D, &E, &F))
			return badargs(filter);
		return filter6(input, output, compdefault, A, B, C, D, E, F);
	}

	printhelp();
	fprintf(stderr, "Error: Bad filter \"%s\"\n", filter);
	return 1;
}
