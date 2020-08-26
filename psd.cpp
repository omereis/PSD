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
#include <chrono>
#include <time.h>

#include <iostream>
#include <string>
using namespace std;

#include "redpitaya/rp.h"
#include "psd_params.h"
#include "psd_output.h"
#include "proj_misc.h"

static const string s_strDefaultParamsJson ("psd_params.json");
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
void InitiateSampling (TPsdParams &params);
int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits/* , struct InputParams  *_params */);
//int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits, struct InputParams *_params);
void set_params_defaults (struct InputParams *in_params);
void get_options (int argc, char **argv, TPsdParams &params, string &strParamsJson, bool &fHelp);
void print_usage(char *argv[], TPsdParams &params, const string &strParamsJson);
void print_params (struct InputParams *in_params);
void print_mote_buffer (float *buff, uint32_t buff_size, char *szFile);

void normalize_buff_float (float *buff, uint32_t buff_size);
void print_buffer_volts (float *buff, uint32_t buff_size, char *szFile);
void calc_histogram (float *adResults, uint32_t nSize, int nBins, int fUseZero, char *szFile);
void print_debug (const char *sz);
void set_files_extensions (struct InputParams *in_params);
void ExitWithError (const char * format, ...);
bool read_fast_analog (float *buff, uint32_t buff_size);
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	TPsdParams m_params;
	TPsdOutput psd_results;
	bool fHelp;
	string strParamsJson (s_strDefaultParamsJson); 

	get_options (argc, argv, m_params, strParamsJson, fHelp);
	if (fHelp) {
		print_usage(argv, m_params, s_strDefaultParamsJson);
		exit (0);
	}
	printf ("RF input reader\n");
	system ("cat /opt/redpitaya/fpga/fpga_0.94.bit > /dev/xdevcfg");
	if(rp_Init() != RP_OK){
		fprintf(stderr, "Rp api init failed!\n");
	}
	printf ("===========================================\n");
	m_params.LoadFromJson ("psd_params.json");
	m_params.print();
	printf ("===========================================\n");
	psd_results.SetParams (m_params);

	int n, nValids;
	uint32_t buff_size = m_params.GetSamples();
	float *buff = new float[buff_size];;
	clock_t tStart, tAccumulated;
	InitiateSampling (m_params);
	for (n=0, nValids=0 ; n < m_params.GetIterations() ; n++) {
		tAccumulated = 0;
		if (read_fast_analog (buff, buff_size)) {
			nValids++;
			tStart = clock ();
			psd_results.HandleNew(buff, buff_size);
			tAccumulated += (clock() - tStart);
		}
		printf ("%d iterations completed\r", n);
	}
	printf ("\n");
	delete[] buff;
	rp_Release();
	double dPsdTime = ((double) tAccumulated) / ((double) (CLOCKS_PER_SEC));
	printf ("Running time per single iteration is %s seconds\n", FormatEngineeringUnits (dPsdTime / (double) nValids).c_str());
	//:w
	psd_results.SaveResults ();
	printf("Results Saved\n");
	return 0;
}

//-----------------------------------------------------------------------------
void InitiateSampling (TPsdParams &params)
{
	int nCode;

	if ((nCode = rp_AcqReset()) != RP_OK)
		ExitWithError ("Error in Reset. Code: %d\n", nCode);
	if ((nCode = rp_AcqSetDecimation(params.GetSamplingParams().GetDecimation())) != RP_OK)
		ExitWithError ("Error in Reset. Code: %d\n", nCode);
	if ((nCode = rp_AcqSetTriggerLevel(RP_CH_1, params.GetTrigger().GetLevel())) != RP_OK)
		ExitWithError ("Error in Reset. Code: %d\n", nCode);
	if ((nCode = rp_AcqSetSamplingRate (RP_SMP_125M)) != RP_OK)
		ExitWithError ("Error in Reset. Code: %d\n", nCode);
	if ((nCode = rp_AcqSetTriggerDelay(0)) != RP_OK)
		ExitWithError ("Error in Reset. Code: %d\n", nCode);
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
//int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits, TPsdParams &params)
int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits/*, struct InputParams *in_params*/)
{
	time_t tStart, tNow;
	bool fTrigger, fTimeLimit;
	rp_acq_trig_state_t state = RP_TRIG_STATE_WAITING;

	rp_AcqStart();
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	rp_AcqReset();
	rp_AcqSetDecimation(RP_DEC_1);
	rp_AcqSetTriggerLevel(RP_CH_1, 10e-3); //Trig level is set in Volts while in SCPI
	rp_AcqSetSamplingRate (RP_SMP_125M);
		
	rp_AcqSetTriggerDelay(0);
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);

	rp_AcqStart();


	usleep(1);
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

void get_options (int argc, char **argv, TPsdParams &params, string &strParamsJson , bool &fHelp)
{
	int c;
/*
commands
	f - options file in JSON
	h - help
	i - iterations (-1 == inf)
	j - input json parameters
	l - long
	n - samples (buffer length)
	p - output psd results
	r - output raw name
	s - short length
	t - trigger levet

	w - raw pulses to save
*/
	//set_params_defaults (in_params);

	fHelp = false;
	while ((c = getopt (argc, argv, "hpt:n:f:d:i:s:H:")) != -1)
		switch (c) {
			case 'f':
			case 'F':
				strParamsJson = string(optarg);
				break;
			default:
			case 'h':
				fHelp = true;
				break;
			case 'i':
			case 'I':
				params.SetIterations(atoi(optarg));
				break;
			case 'j':
			case 'J':
				strParamsJson = string(optarg);
				break;
			case 'l':
			case 'L':
				params.SetLong (atof (optarg));
				break;
			case 'n':
			case 'N':
				params.SetSamples (atoi (optarg));
				break;
			case 'p':
			case 'P':
				params.SetPsdFile (optarg);
				break;
			case 'r':
			case 'R':
				params.SetSaveRaw (atoi(optarg));
				break;
			case 's':
			case 'S':
				params.SetShort (atof (optarg));
				break;
			case 't':
			case 'T':
				params.SetTriggerLevel (atof (optarg));
				break;
			case 'w':
			case 'W':
				params.SetRawFile(optarg);
				break;
		}
} 



/*
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
*/
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
void print_usage(char *argv[], TPsdParams &params, const string &strParamsJson)
{
	string strUsage;
	string strRawFile (params.GetRawFile());

	if (strRawFile.length() == 0)
	strRawFile = "(none)";

	strUsage = 
		string ("=======================================\n") +
		string ("Red Pitaya PSD Analyzer\n") +
		string ("Synopsis:\n") +
		string (argv[0]) + string (" -h -i <iterations> -l <long> -n <samples> -p <output PSD> -r <output raw>\n") +
		string ("        -s <short length> -t <trigger level> -w <raw pulses to save>\n") +
		string ("Where\t\t\t\t\t| Defaults\n") +
		string ("    f - Option File in JSON format.\t| ") + string("default: '") + strParamsJson + ("'\n") +
		string ("    h - help\t\t\t\t| false\n") +
		string ("    i - iterations (-1 == inf)\t\t| ") +  FormatWithComma(params.GetIterations()) + "\n" +
		string ("    j - input json parameters\t\t| ") + strParamsJson + "\n" +
		string ("    l - long persiod [nSec]\t\t| ") + to_string(params.GetLong()) + "\n" +
		string ("    n - samples (buffer length)\t\t| ") + FormatWithComma (params.GetSamples()) + "\n" +
		string ("    p - output psd results\t\t| ") + params.GetPsdFile() + "\n" +
		string ("    r - output raw name\t\t\t| ") + strRawFile + "\n" +
		string ("    s - short period [nSec]\t\t| ") + to_string(params.GetLong()) + "\n" +
		string ("    t - trigger levet\t\t\t| ") + to_string(params.GetTrigger().GetLevel()) + "\n" +
		string ("    w - raw pulses to save\t\t| ") +  to_string(params.GetSaveRaw()) + "\n" +
		string ("=====================================================================\n");
		//string ("Defaults:\n") +
		//string ("    Iterations: ") + to_string(params.GetIterations());
	printf ("%s\n", strUsage.c_str());
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

bool read_fast_analog (float *buff, uint32_t buff_size)
{
	rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;
	time_t tStart, tNow;
	bool fTrigger = false, fTimeout = false;

	rp_AcqStart();
	usleep(1);
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	time (&tStart);
	while((fTrigger == false) && (fTimeout == false)){
		rp_AcqGetTriggerState(&state);
		if(state == RP_TRIG_STATE_TRIGGERED){
			fTrigger = true;
		}
		else {
			time (&tNow);
			if (difftime (tNow, tStart) >= 3)
				fTimeout = true;
		}
	}
	if (fTrigger) {
		uint32_t nTrigPos;
		rp_AcqGetWritePointerAtTrig (&nTrigPos);
		rp_AcqGetDataV(RP_CH_1, nTrigPos-100, &buff_size, buff); // 80 nSec before trigger
	}
	else
		printf ("Timeout\n");

/*
	int i;
	for(i = 0; i < 10; i++){
		printf("%f\n", buff[i]);
	}
*/
	return (fTrigger);
}
//-----------------------------------------------------------------------------
