#ifndef	PSD_OUTPUT_H
#define	PSD_OUTPUT_H

#include <vector>
using namespace std;
#include "bimam_types.h"
#include "psd_params.h"
#include "psd_out_params.h"
#include "redpitaya/rp.h"

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
	void HandleNew(float *buff, uint32_t buff_size);
	void SaveResults ();
	void SaveRaw (const string &strFile);
	void SavePsd (const string &strFile);
protected:
	void ConvertSamples (float *afBuffer, uint32_t nSize, TFloatVec &vSignal);
	void AssignAll (const TPsdOutput &params);
private:
	TPsdParams m_params;
	TFloatMatrix m_mtxOut;
	TPsdOutParamsVec m_vPsdParams;
};
#endif


