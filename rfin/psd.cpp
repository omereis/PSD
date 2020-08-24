/* Red Pitaya C API example Acquiring a signal from a buffer
 * This application acquires a signal on a specific channel */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "redpitaya/rp.h"
#include "fpga.h"


int main(int argc, char **argv)
{
	printf ("RF input reader\n");
	system ("cat /opt/redpitaya/fpga/fpga_0.94.bit > /dev/xdevcfg");
	if(rp_Init() != RP_OK){
		fprintf(stderr, "Rp api init failed!\n");
	}

	uint32_t buff_size = 16384;
	float *buff = (float *)malloc(buff_size * sizeof(float));

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
	int i;
	for(i = 0; i < 10; i++){
		printf("%f\n", buff[i]);
	}
	free(buff);
	rp_Release();
	return 0;
}

