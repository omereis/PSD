#ifndef	PSD_OUTPUT_H
#define	PSD_OUTPUT_H

#include <vector>
using namespace std;
#include "bimam_types.h"
#include "psd_params.h"
#include "psd_out_params.h"
#include "redpitaya/rp.h"
#include <stdio.h>

class TPsdOutput {
public:
	TPsdOutput();
	TPsdOutput(const TPsdOutput &other);
	TPsdOutput operator= (const TPsdOutput &other);
	bool operator== (const TPsdOutput &other) const;
	bool operator!= (const TPsdOutput &other) const;
	void Clear ();
	void SetParams (const TPsdParams &params);
	TPsdParams GetParams() const;
	uint32_t PulsesCount () const;
	bool HandleNew(float *buff, uint32_t buff_size);
	void HandleNew(float *buff, int16_t *ai16Buf, uint32_t buff_size);
	bool SaveResultsMean (const TPsdParams &in_params, const string &strFileName);
	bool SaveResultsMean (const TPsdParams &in_params, FILE *file);
	void SaveResults ();
	void SaveRaw (const string &strFile);
	void SavePsd (const string &strFile);
	int i0, iEnd;
protected:
	void ConvertSamples (float *afBuffer, uint32_t nSize, TFloatVec &vSignal);
	void AssignAll (const TPsdOutput &params);
private:
	TPsdParams m_params;
	TFloatMatrix m_mtxOut, m_mtxFiltered, m_mtxRaw;
	TPsdOutParamsVec m_vPsdParams;
};
#endif


