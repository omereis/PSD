#ifndef	PSD_OUTPUT_H
#define	PSD_OUTPUT_H

#include <vector>
using namespace std;
#include "bimam_types.h"
#include "redpitaya/rp.h"

class TPsdOutput {
public:
	TPsdOutput();

	void AddSamples (float *afBuffer, uint32_t nSize);
	uint32_t PulsesCount () const;
protected:
private:
	TFloatMatrix m_mtxOut;
};
#endif


