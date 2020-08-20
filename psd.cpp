/* PSD - Pulse Shape Discrimination */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>

#include <iostream>
#include <string>
using namespace std;

#include "redpitaya/rp.h"
#include "psd_params.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//-----------------------------------------------------------------------------
struct InputParams {
	short Help;
	int Samples;
	float Trigger;
	int Delay;
	char FileName[1024];
	char HistFile[1024];
	char SumsFile[1024];
	int Iterations;
	short Print;
};
//-----------------------------------------------------------------------------
int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits, struct InputParams *in_params);
void set_params_defaults (struct InputParams *in_params);
void get_options (int argc, char **argv, struct InputParams *in_params);
void print_usage();
void print_params (struct InputParams *in_params);
void print_mote_buffer (float *buff, uint32_t buff_size, char *szFile);

void normalize_buff_float (float *buff, uint32_t buff_size);
void print_buffer_volts (float *buff, uint32_t buff_size, char *szFile);
void calc_histogram (float *adResults, uint32_t nSize, int nBins, int fUseZero, char *szFile);
void print_debug (const char *sz);
void set_files_extensions (struct InputParams *in_params);
void ExitWithError (const char * format, ...);
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	TPsdParams m_params;
	struct InputParams in_params;

	memset (&in_params, 0, sizeof (in_params));	
	get_options (argc, argv, &in_params);
	if (in_params.Help) {
		print_usage();
		exit(0);
	}

	print_params (&in_params);
	printf ("===========================================\n");
	m_params.LoadFromJson ("psd_params.json");
	m_params.print();
	printf ("===========================================\n");
	ExitWithError ("Quitting with error %d", 17);
        /* Print error, if rp_Init() function failed */
	if(rp_Init() != RP_OK){
		fprintf(stderr, "Rp api init failed!\n");
	}

/*************************************************************/
        /*LOOB BACK FROM OUTPUT 2 - ONLY FOR TESTING*/
	uint32_t buff_size = in_params.Samples;//6250;//12500;//16384;//8192;//16384;
	float *afBuff;

	afBuff = (float*) calloc(buff_size, sizeof(afBuff[0]));

	rp_AcqReset();
	if (rp_AcqSetDecimation(RP_DEC_1/*1*/) != RP_OK)
		printf("Error setting decimation\n");;
	if (rp_AcqSetSamplingRate(RP_SMP_125M) != RP_OK)
		printf ("Setting sampleing rate error\n");
	rp_AcqSetTriggerLevel(RP_CH_1, in_params.Trigger); //Trig level is set in Volts while in SCPI
	rp_AcqSetTriggerDelay(in_params.Delay);
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	if (rp_AcqSetDecimation(RP_DEC_1) != RP_OK)
		printf("Error setting decimation\n");

	int nWaits, nValids/*, fPrint, iBiggest*/;
	double d = ((double) in_params.Delay * -1.0) + 8188.0;
	float dSum=0;
	int j, k, nStart = (int) d;//(nDelay * -124.9) + 8188, fPrint; // from measurements
	float *adLong, *adShort, dHistMin=0, dHistMax=0, dSamplesMax;//, dBiggest;
/*
	float **mtx = (float**) calloc (100, sizeof (mtx[0]));
	for (j=0 ; j < 100 ; j++)
		mtx[j] = (float*) calloc(buff_size, sizeof (mtx[0][0]));
*/

	adLong= new float[in_params.Iterations];//calloc (in_params.Iterations, sizeof (adResults[0]));
	adShort = new float[in_params.Iterations];//calloc (in_params.Iterations, sizeof (adResults[0]));
	float *adMax = new float[in_params.Iterations];
	nStart = 0;
	printf("d=%g, nStart=%d, buf_size=%d\n", d, nStart, buff_size);
	nStart = 0;
	for (k=0, nValids=0 ; k < in_params.Iterations ; k++) {
		if (read_input_volts (afBuff, buff_size, &nWaits, &in_params) > 0) {
			dSamplesMax = dSum = afBuff[0];
			for (j=1 ; j < 100+300 ; j++) {
				dSamplesMax = max (afBuff[j], dSamplesMax);
				dSum += afBuff[j];
				if (j == 100+100)
					adShort[k] = dSum;
			}
			adLong[k] = dSum;
/*
			if (k < 100) {
				for (j=0 ; j < (int) buff_size ; j++)
					mtx[k][j] = afBuff[j];
			}
*/
			adMax[k] = dSamplesMax;
			nValids++;
			if ((nValids == 0) && (in_params.Print)) {
				print_buffer_volts (afBuff, buff_size, in_params.FileName);
			}
		}
		if ((k % 100) == 0)
			fprintf (stderr, "Completed %d iterations, Max: %g, Min: %g\r", k, dHistMax, dHistMin);
	}
	printf ("\n");
//	calc_histogram (adResults, in_params.Iterations, 1024, 0, "hist_sum.csv");
	//calc_histogram (adResults, in_params.Iterations, 1024, 0, in_params.HistFile);
//	calc_histogram (adMax, in_params.Iterations, 1024, 1, "hist_max.csv");
	fprintf (stderr, "histogram calculated\n");
	printf ("\n");

	FILE *f = fopen ("spsd.csv", "w+");
	fprintf (f, "n, short, long, max\n");
	for (int n=0 ; n < in_params.Iterations ; n++)
		fprintf (f, "%d,%g,%g, %g\n", n+1, adLong[n], adShort[n], adMax[n]);
	fclose (f);
/*
	f = fopen ("hundred.csv", "w+");
	std::string str;
	for (int n=0 ; n < (int) buff_size ; n++) {
		for (int m=0 ; m < 100 ; m++) {
			str += to_string (mtx[m][n]);
			if (m < 99)
				str += ",";
		}
		fprintf (f, "%s\n", str.c_str());
		str = "";
	}
	fclose(f);
	for (int m=0 ; m < 100 ; m++)
		free (mtx[m]);
	free (mtx);
*/
	delete[] adLong;
	delete[] adShort;
	rp_Release();
	return 0;
}
//-----------------------------------------------------------------------------
void print_debug (const char *sz)
{
	FILE *file = fopen ("oe_debug.txt", "a+");
	fprintf (file, "%s\n", sz);
	fclose(file);
}
int nDebug=1;
//-----------------------------------------------------------------------------
int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits, struct InputParams *in_params)
{
	time_t tStart, tNow;
	bool fTrigger, fTimeLimit;
	rp_acq_trig_state_t state = RP_TRIG_STATE_WAITING;

	rp_AcqStart();
	usleep(1);
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	time(&tStart);
	fTrigger = fTimeLimit = false;
	while((!fTrigger) && (!fTimeLimit)){
		rp_AcqGetTriggerState(&state);
		if(state == RP_TRIG_STATE_TRIGGERED){
			fTrigger = true;
		}
		time(&tNow);
		if (difftime (tNow, tStart) >= 5)
			fTimeLimit = true;
	}

	if (fTrigger) {
		//printf ("Triggered\n");
		uint32_t nTrigPos;
		rp_AcqGetWritePointerAtTrig (&nTrigPos);
		rp_AcqGetDataV(RP_CH_1, nTrigPos-100, &buff_size, buff); // 80 nSec before trigger
	}
	else
		printf ("Timeout\n");
	rp_AcqStop ();
	return (fTrigger);
}
//-----------------------------------------------------------------------------
void get_options (int argc, char **argv, struct InputParams *in_params)
{
	int c;

	set_params_defaults (in_params);

	while ((c = getopt (argc, argv, "hpt:n:f:d:i:s:H:")) != -1)
		switch (c) {
			default:
			case 'h':
				in_params->Help = 1;
				return;
			case 'p':
			case 'P':
				in_params->Print = 1;
				break;
			case 't':
			case 'T':
				in_params->Trigger = atof (optarg);
				break;
			case 'n':
			case 'N':
				in_params->Samples = atoi (optarg);
				break;
			case 'f':
			case 'F':
				strcpy (in_params->FileName, optarg);
				break;
			case 's':
			case 'S':
				strcpy (in_params->SumsFile, optarg);
				break;
			case 'H':
				strcpy (in_params->HistFile, optarg);
				break;
			case 'd':
			case 'D':
				in_params->Delay = atoi(optarg);
				break;
			case 'i':
			case 'I':
				in_params->Iterations = atoi(optarg);
				break;
		}
	set_files_extensions (in_params);
} 
//-----------------------------------------------------------------------------
char *add_file_extension (char *szFileName)
{
	if (strchr(szFileName, '.') == NULL)
		sprintf (szFileName, "%s.csv", szFileName);
	return (szFileName);
}
//-----------------------------------------------------------------------------
void set_files_extensions (struct InputParams *in_params)
{
	add_file_extension (in_params->FileName);
	add_file_extension (in_params->SumsFile);
	add_file_extension (in_params->HistFile);
}
//-----------------------------------------------------------------------------
void set_params_defaults (struct InputParams *in_params)
{
	in_params->Trigger = 10e-3;
	in_params->Samples = 12500;
	in_params->Delay = 1250;
	in_params->Help = 0;
	in_params->Iterations = 10;
	in_params->Print = 0;
	strcpy (in_params->FileName, "out");
	strcpy (in_params->SumsFile, "sums");
	strcpy (in_params->HistFile, "hist");
	set_files_extensions (in_params);
}
//-----------------------------------------------------------------------------
void print_usage()
{
	const char *szMessage = "Red Pitaya RF input\n"
					"Synopsis:\n"
					"./rp_read -t <trigger [volts]> -n <# of samples> -f <output file name> -d <delay items> -i <# of iterations>\n\t  -s <sums file name> -H <histogram file name> -p print results\n"
					"  \nDefaults:\n"
					"    Trigger: 10mV:\n"
					"    Samples: 10,000\n"
					"    Delay  : 1250 data points\n"
					"    File   : out.csv\n"
					"    Histogram   : hist.csv\n"
					"    Sums   : sums.csv\n"
					"    Iterations: 10\n"
					"    Print  : 0 (no)\n";
	printf ("%s\n", szMessage);
}
//-----------------------------------------------------------------------------
void print_params (struct InputParams *in_params)
{
	printf ("Red Pitaya RF input\n");
	printf ("    Trigger: %gmV:\n", in_params->Trigger);
	printf ("    Samples: %d points\n", in_params->Samples);
	printf ("    Delay  : %d data points\n", in_params->Delay);
	printf ("    File   : %s\n", in_params->FileName);
	printf ("    Histogram   : %s\n", in_params->HistFile);
	printf ("    Sums   : %s\n", in_params->SumsFile);
	printf ("    Iterations: %d\n", in_params->Iterations);
	printf ("    Print  : %s\n", (in_params->Print > 0) ? "yes" : "no");
}
//-----------------------------------------------------------------------------
void normalize_buff (uint32_t *buff, uint32_t buff_size)
{
	int n;
	uint32_t fMin = 0;

	for (n=0 ; n < (int) buff_size ; n++)
		fMin = min (fMin, buff[n]);
	for (int n=0 ; n < (int) buff_size ; n++)
		buff[n] = buff[n] - fMin;
}
//-----------------------------------------------------------------------------
void normalize_buff_float (float *buff, uint32_t buff_size)
{
	int n;
	float fMin = 1e300;

	for (n=0 ; n < (int) buff_size ; n++)
		fMin = min (fMin, buff[n]);
	for (int n=0 ; n < (int) buff_size ; n++)
		buff[n] = buff[n] - fMin;
}
//-----------------------------------------------------------------------------
void print_buffer_volts (float *buff, uint32_t buff_size, char *szFile)
{
	FILE *fout;
	int n;

	fout = fopen (szFile, "w+");
	for(n = 0 ; n < (int) buff_size ; n++){
		fprintf(fout, "%f\n", buff[n]);
	}
	fclose (fout);
}
//-----------------------------------------------------------------------------
void print_mote_buffer (float *buff, uint32_t buff_size, char *szFile)
{
	FILE *fout;
	int n;

	fout = fopen (szFile, "a+");
	for(n = 0 ; n < (int) buff_size ; n++){
		fprintf(fout, "%g\n", buff[n]);
	}
	fclose (fout);
}
//-----------------------------------------------------------------------------
void calc_histogram (float *adResults, uint32_t nSize, int nBins, int fUseZero, char *szFile)
{
	int n, idx;
	int *anHistogram;
	float rMax, rMin, rBin;//, rNominator;

	anHistogram = (int*) calloc (nBins, sizeof (anHistogram[0]));
	rMax = adResults[0];
	if (fUseZero > 0)
		rMin = adResults[0];
	else
		rMin = 0;
	for (n=1 ; n < (int) nSize ; n++) {
		if (fUseZero > 0)
			rMin = min(rMin, adResults[n]);
		rMax = max(rMax, adResults[n]);
	}
	rBin = (rMax - rMin) / nBins;
	printf ("+=======================================================++\n");
	printf ("Maximum: %g\nMinmum: %g\n# of bins: %d\n, Bin: %g\n", rMax, rMin, nBins, rBin);
	printf ("+=======================================================++\n");
//	FILE *fDebug = fopen ("oe_debug.csv", "w+");
//	fprintf (fDebug, "Nominator, Bin, idx, (rNominator/rBin)\n");
	for (n=0 ; n < nBins ; n++)
		anHistogram[n] = 0;
	for (n=0 ; n < (int) nSize ; n++) {
//		rNominator = (adResults[n] - rMin);
		idx = (int) ((adResults[n] - rMin) / rBin);
//		fprintf (fDebug, "%g,%g,%d,%g\n", rNominator, rBin, idx, rNominator/rBin);
		if ((idx >= 0) && (idx < nBins)) {
			anHistogram[idx] = anHistogram[idx] + 1;
		}
	}
	printf ("Histogram callculated\n");
//	fclose (fDebug);
	char sz[1024], szName[1024];
	getcwd (sz, 24);
	FILE *file;
	sprintf (szName, "%s/%s", sz, szFile);
//	sprintf (szName, "%s/hist.csv", sz);
	file = fopen (szName, "w+");
	for (n=0 ; n < 1024 ; n++)
		fprintf (file, "%d\n", anHistogram[n]);
	fclose(file);
	free (anHistogram);
}
//-----------------------------------------------------------------------------
