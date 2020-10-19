/* Red Pitaya C API example Acquiring a signal from a buffer
 * This application acquires a signal on a specific channel */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "redpitaya/rp.h"
#include "fpga.h"

rp_acq_decimation_t GetCmdLineDecimation (int argc, char *argv[]);
void print_decimation (rp_acq_decimation_t decCurrent);

int main(int argc, char **argv)
{
	rp_acq_decimation_t decimation, decCurrent;

        /* Print error, if rp_Init() function failed */
	printf ("RF input reader\n");

/*
	int nFpgaInit;
	nFpgaInit = osc_fpga_init();
	printf ("FPGA Init Code: %d\n", nFpgaInit);
	nFpgaInit = osc_fpga_reset ();
	printf ("FPGA Reset Code: %d\n", nFpgaInit);
*/
	system ("cat /opt/redpitaya/fpga/fpga_0.94.bit > /dev/xdevcfg");
	printf ("bit file copied\n");
        if(rp_Init() != RP_OK){

                fprintf(stderr, "Rp api init failed!\n");
        }

        /*LOOB BACK FROM OUTPUT 2 - ONLY FOR TESTING*/
/*
        rp_GenReset();
        rp_GenFreq(RP_CH_1, 20000.0);
        rp_GenAmp(RP_CH_1, 1.0);
        rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
        rp_GenOutEnable(RP_CH_1);
*/

        uint32_t buff_size = 16384;
        float *buff = (float *)malloc(buff_size * sizeof(float));
		int16_t* auiBuffer = (int16_t*) calloc (buff_size, sizeof (auiBuffer[0]));

        rp_AcqReset();
		decimation = GetCmdLineDecimation (argc, argv);
        rp_AcqSetDecimation(decimation);
        rp_AcqGetDecimation(&decCurrent);
		print_decimation (decCurrent);
        //rp_AcqSetDecimation(RP_DEC_1);
        rp_AcqSetTriggerLevel(RP_CH_1, 1e-3); //Trig level is set in Volts while in SCPI
		//rp_AcqSetSamplingRate (RP_SMP_125M);
		rp_AcqSetSamplingRate (RP_SMP_15_625M);
		
        rp_AcqSetTriggerDelay(0);

        // there is an option to select coupling when using SIGNALlab 250-12
        // rp_AcqSetAC_DC(RP_CH_1, RP_AC); // enables AC coupling on channel 1

        // by default LV level gain is selected
        // rp_AcqSetGain(RP_CH_1, RP_LOW); // user can switch gain using this command

        rp_AcqStart();

        /* After acquisition is started some time delay is needed in order to acquire fresh samples in to buffer*/
        /* Here we have used time delay of one second but you can calculate exact value taking in to account buffer*/
        /*length and smaling rate*/

	usleep(1);
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;
	short int fTrigger=0, fTimeout=0;
	clock_t cStart;
	//while(1){
	cStart = clock();
	double dDiff;
	while((fTrigger == 0) && (fTimeout == 0)){
	//	printf ("Preparing to read trigger\n");
		rp_AcqGetTriggerState(&state);
		//printf ("Read trigger\n");
		if (state == RP_TRIG_STATE_TRIGGERED)//{
			fTrigger = 1;
		dDiff = (clock() - cStart);
		dDiff /= (double) CLOCKS_PER_SEC;
		//printf ("Diff: %g\n", dDiff);
		if (dDiff >= 3)
		//if (((double) (clock() - cStart) / (double) CLOCKS_PER_SEC) >=2)
			fTimeout = 1;
		//else
			//printf ("Time Diff: %g\n", dDiff);
			//break;
		//}
	}
	if (fTrigger)
		printf ("Triggered\n");
	else
		printf ("Timeout: %g\n", dDiff);

	rp_AcqGetOldestDataV(RP_CH_1, &buff_size, buff);
	uint32_t uiTriggerPos, uiLen=buff_size;//15000;
	rp_AcqGetWritePointerAtTrig (&uiTriggerPos);
	rp_AcqGetDataRaw (RP_CH_1, uiTriggerPos, &uiLen, auiBuffer);
	//rp_AcqGetOldestDataRaw(RP_CH_1, &buff_size, auiBuffer);
	int i, nPrintLen=buff_size;//15000;
		FILE *file = stdout;
		if (argc > 1) {
			file = fopen (argv[1], "w+");
			nPrintLen = buff_size;
		}
	for(i = 0; i < nPrintLen ; i++){
	//for(i = 0; i < 1000; i++){
        //for(i = 0; i < buff_size; i++){
                //printf("%f\n", buff[i]);
                //fprintf(file, "%f,%d\n", buff[i], auiBuffer[i]);
                fprintf(file, "%d\n", auiBuffer[i]);
        }
		if (argc > 2)
			fclose (file);
        /* Releasing resources */
        free(buff);
		free (auiBuffer);
        rp_Release();
        return 0;
}

rp_acq_decimation_t GetCmdLineDecimation (int argc, char *argv[])
{
	rp_acq_decimation_t decimation;
	int nCode;

	decimation = RP_DEC_1;
	if (argc > 2) {
		nCode = atoi(argv[2]);
		if (nCode == 8)
			decimation = RP_DEC_8;
		else if (nCode == 64)
			decimation = RP_DEC_64;
		else if (nCode == 1024)
			decimation = RP_DEC_1024;
		else
			decimation = RP_DEC_1;
	}
	return (decimation);
}

void print_decimation (rp_acq_decimation_t decCurrent)
{
	char *sz;

	printf ("Decimation: ");
	if (decCurrent == RP_DEC_1)
		sz = "1";
	else if (decCurrent == RP_DEC_8)
		sz = "8";
	else if (decCurrent == RP_DEC_64)
		sz = "64";
	else if (decCurrent == RP_DEC_1024)
		sz = "1024";
	else
		sz = "other";
	printf (" %s\n", sz);

}
