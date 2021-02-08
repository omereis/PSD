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
#include <exception>

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "rp.h"
//#include "redpitaya/include/rp.h"
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
typedef struct AdcConvertParams {
	double	gain;
	double	offset;
} TAdcConvert;
//-----------------------------------------------------------------------------
static string g_RedPiayaJsonFile ("red_pitaya.json");
//-----------------------------------------------------------------------------
void InitiateSampling (TPsdParams &params);
int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits, int16_t *ai16Buf);
//int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits/* , struct InputParams  *_params */);
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
bool read_fast_analog (float *buff, uint32_t buff_size, int16_t *ai16Buf, const TAdcConvert &adc_convert);
bool LoadAdcParams (TAdcConvert &adc_convert);
bool LoadAdcParams (const string &strFile, TAdcConvert &adc_convert, string &strErr);
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	TPsdParams m_params;
	TPsdOutput psd_results;
	bool fHelp;
	string strParamsJson (s_strDefaultParamsJson), strErr;
	TAdcConvert adc_convert;

	printf ("Howdi, folks\n");
	try {
		printf ("Copying fpga file...");
		system ("cat /opt/redpitaya/fpga/fpga_0.94.bit > /dev/xdevcfg");
		printf ("...Copied\n");
	}
	catch (exception &e) {
		printf ("Runtime error\n%s", e.what());
	}
	printf ("RF input reader\n");
	if(rp_Init() != RP_OK){
		fprintf(stderr, "Rp api init failed!\n");
	}
	printf ("===========================================\n");

	m_params.LoadFromJson ("psd_params.json");
	get_options (argc, argv, m_params, strParamsJson, fHelp);
	LoadAdcParams (adc_convert);

	if (fHelp) {
		print_usage(argv, m_params, s_strDefaultParamsJson);
		exit (0);
	}
	m_params.print();
	printf ("===========================================\n");

	rp_pinState_t gain;
	float rGain;
	rp_AcqGetGainV(RP_CH_1, &rGain);
	printf ("Channel 1 gain: %g\n", rGain);
	//rp_AcqSetGain(RP_CH_1, RP_HIGH);
	rp_AcqGetGain(RP_CH_1, &gain);
	if (gain == RP_LOW)
		printf ("Low Gain\n");
	else
		printf ("High Gain\n");
	rp_AcqGetGainV (RP_CH_1, &rGain);
	printf ("Library gain: %g\n", rGain);
	rp_calib_params_t tCalib;

	memset (&tCalib, 0, sizeof(tCalib));
	tCalib = rp_GetCalibrationSettings();
	printf ("Channel 1 Calibration settings\n");
	printf ("High gain front end full scale voltage: %g\n", (float) tCalib.fe_ch1_fs_g_hi);
	printf ("Back end full scale voltage: %g\n", (float) tCalib.fe_ch1_fs_g_hi);
	//exit (0);
	printf ("===========================================\n");
	psd_results.SetParams (m_params);

	int n, nValids;
	uint32_t buff_size = m_params.GetSamples();
	float *buff = new float[buff_size];
	//int16_t *ai16Buf = new int16_t[buff_size];
	clock_t tStart, tAccumulated;
	InitiateSampling (m_params);
	//printf ("Starting iterations\n");
	for (n=0, nValids=0 ; n < m_params.GetIterations() ; /*n++*/) {
		tAccumulated = 0;
		if (read_fast_analog (buff, buff_size, NULL, adc_convert)) {
			//printf ("Input read\n");
		//if (read_fast_analog (buff, buff_size, ai16Buf)) {
			nValids++;
			tStart = clock ();
			if (psd_results.HandleNew(buff, buff_size))
				n++;
			//printf ("New vector handled\n");
			tAccumulated += (clock() - tStart);
		}
		printf ("%d iterations completed\r", n);
	}
	printf ("\n");
	printf ("So far so good\n");
	delete[] buff;
	//delete[] ai16Buf;
	rp_Release();
	double dPsdTime = ((double) tAccumulated) / ((double) (CLOCKS_PER_SEC));
	printf ("Running time per single iteration is %s seconds\n", FormatEngineeringUnits (dPsdTime / (double) nValids).c_str());
	//:w
	psd_results.SaveResults ();
	//FILE *fileSum;
	//fileSum = fopen ("psd_sum.csv", "w+");
	//psd_results.SaveResultsMean (fileSum);
	psd_results.SaveResultsMean (m_params, "psd_sum.csv");
	//psd_results.SaveResultsMean (fileSum);
	//fclose (fileSum);
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
	if ((nCode = rp_AcqSetTriggerLevel(RP_T_CH_1, params.GetTrigger().GetLevel())) != RP_OK)
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

//int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits/*, struct InputParams *in_params*/)
int read_input_volts (float *buff, uint32_t buff_size, int *pnWaits, int16_t *ai16Buf)
{
	time_t tStart, tNow;
	bool fTrigger, fTimeLimit;
	rp_acq_trig_state_t state = RP_TRIG_STATE_WAITING;

	rp_AcqStart();
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	rp_AcqReset();
	rp_AcqSetDecimation(RP_DEC_1);
	rp_AcqSetTriggerLevel(RP_T_CH_1, 10e-3); //Trig level is set in Volts while in SCPI
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
		if (ai16Buf != NULL)
			rp_AcqGetDataRaw(RP_CH_1, nTrigPos-100, &buff_size, ai16Buf); // 80 nSec before trigger
	}
	else
		printf ("\nTimeout\n");
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

	//printf ("This is get_options\n");
	fHelp = false;
	while ((c = getopt (argc, argv, "hpt:n:f:d:i:s:H:v:T:")) != -1)
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
				params.SetTriggerLevel (atof (optarg));
				printf ("Trigger set to %g\n", atof(optarg) * 1e3);
				break;
			case 'T':
				params.SetInputTau (atof (optarg));
				fHelp = false;
				break;
			case 'v':
				params.SetInputVoltage (atof (optarg));
				fHelp = false;
				break;
			case 'w':
			case 'W':
				params.SetRawFile(optarg);
				break;
		}
	printf ("Copleted get_options\n");
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
		string ("    l - long persiod [nSec]\t\t| ") + to_string(params.GetLong() * 1e9) + "\n" +
		string ("    n - samples (buffer length)\t\t| ") + FormatWithComma (params.GetSamples()) + "\n" +
		string ("    p - output psd results\t\t| ") + params.GetPsdFile() + "\n" +
		string ("    r - output raw name\t\t\t| ") + strRawFile + "\n" +
		string ("    s - short period [nSec]\t\t| ") + to_string(params.GetLong() *1e9) + "\n" +
		string ("    t - trigger levet\t\t\t| ") + to_string(params.GetTrigger().GetLevel()) + "\n" +
		string ("    w - raw pulses to save\t\t| ") +  to_string(params.GetSaveRaw()) + "\n" +
		string ("    v - Input Voltage\t\t\t| ") + to_string (params.GetInputVoltage() * 1e3) + "\n" +
		string ("    T - Input Tau\t\t\t| ") + to_string(params.GetInputTau() * 1e6) + "\n";
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

//bool read_fast_analog (float *buff, uint32_t buff_size, int16_t *ai16Buf)
bool read_fast_analog (float *buff, uint32_t buff_size, int16_t *ai16Buf, const TAdcConvert &adc_convert)
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
		//fprintf (stderr, "Found Trigger\n");
		uint32_t nTrigPos;
		int16_t *aiBuffer = (int16_t*) calloc(buff_size, sizeof(aiBuffer[0]));
		rp_AcqGetWritePointerAtTrig (&nTrigPos);
		//rp_AcqGetDataV(RP_CH_1, nTrigPos-100, &buff_size, buff); // 80 nSec before trigger
		rp_AcqGetDataRaw(RP_CH_1, nTrigPos-100, &buff_size, aiBuffer); // 80 nSec before trigger
		static bool fWritten = false;
		//for (int n=0 ; n < (int) buff_size ; n++)
			//aiBuffer[n] += 150;//buff[n] = adc_convert.offset + adc_convert.gain * (float) (aiBuffer[n]);//25.336;
		float dMin = 1e10;
		for (int n=0 ; n < (int) buff_size ; n++) {
			buff[n] = adc_convert.offset + adc_convert.gain * (float) (aiBuffer[n]);//25.336;
			dMin = min (dMin, buff[n]);
		}
		for (int n=0 ; n < (int) buff_size ; n++)
			buff[n] -= dMin;
		if (!fWritten) {
			fWritten = true;
			FILE *fileDebug = fopen ("raw.csv", "w+");
			for (int m=0 ; m < (int) buff_size ; m++)
				fprintf (fileDebug, "%d,%g\n", aiBuffer[m] + 150, buff[m]);
			fclose (fileDebug);
		}
		free (aiBuffer);
		if (ai16Buf != NULL)
			rp_AcqGetDataRaw(RP_CH_1, nTrigPos-100, &buff_size, ai16Buf); // 80 nSec before trigger
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
//-----------------------------------------------------------------------------
bool LoadAdcParams (TAdcConvert &adc_convert)
{
	bool f;
	string strErr;

	if ((f = LoadAdcParams (g_RedPiayaJsonFile, adc_convert, strErr)) == false) {
		printf ("Error loading A/D coeficients:\n%s\n", strErr.c_str());
		exit (-1);
	}
	else {
		printf("A/D Coeficients Loaded\n");
		printf ("Gain: %g\n", adc_convert.gain);
		printf ("Offset: %g\n", adc_convert.offset);
	}
	return (f);
}
//-----------------------------------------------------------------------------
bool LoadAdcParams (const string &strFile, TAdcConvert &adc_convert, string &strErr)
{
	bool fLoad;

	try {
		memset (&adc_convert, 0, sizeof (adc_convert));
		string str (strFile);
		ifstream if_s (str);
		ifstream ifs(strFile);
		Json::Reader reader;
		Json::Value obj;

		reader.parse (ifs, obj);
		adc_convert.gain = obj["gain"].asDouble();
		adc_convert.offset = obj["offset"].asDouble();

		fLoad = true;
		ifs.close();
	}
	catch (exception &e) {
		strErr = e.what();
		fLoad = false;
	}
	return (fLoad);
}
//-----------------------------------------------------------------------------
