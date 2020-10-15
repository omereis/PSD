/* Red Pitaya C API example Acquiring a signal from a buffer
 * This application acquires a signal on a specific channel */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "redpitaya/rp.h"
#include "fpga.h"


int main(int argc, char **argv)
{
	int nPrintSize = 10;

	printf("argc=%d\n", argc);
	if (argc > 1) {
		try {
			nPrintSize = atoi(argv[1]);
			printf ("New print size\n");
		}
		catch(...) {
			printf ("%s could not be converted to integer\n", argv[1]);
		}
	}
	printf ("RF input reader\n");
	system ("cat /opt/redpitaya/fpga/fpga_0.94.bit > /dev/xdevcfg");
	if(rp_Init() != RP_OK){
		fprintf(stderr, "Rp api init failed!\n");
	}

	uint32_t buff_size = 16384;
	float *buff = (float *)malloc(buff_size * sizeof(float));
	uint16_t *auiBuffer = (uint16_t*) calloc(buff_size, sizeof(auiBuffer[0]));

	rp_AcqReset();
	rp_AcqSetDecimation(RP_DEC_1);
	rp_AcqSetTriggerLevel(RP_CH_1, 10e-3); //Trig level is set in Volts while in SCPI
	rp_AcqSetSamplingRate (RP_SMP_125M);
	rp_AcqSetTriggerDelay(0);

	rp_AcqStart();

	usleep(1);
	rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
	rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;

	while(1){
		rp_AcqGetTriggerState(&state);
		if(state == RP_TRIG_STATE_TRIGGERED){
			break;
		}
	}

	rp_AcqGetOldestDataV(RP_CH_1, &buff_size, buff);
	rp_AcqGetOldestDataRow(RP_CH_1, &buff_size, auiBuffer);
	int i;
	for(i = 0; i < nPrintSize ; i++){
		printf("%f,%d\n", buff[i], auiBuffer[i]);
	}
	free(buff);
	free(auiBuff);
	rp_Release();
	return 0;
}

