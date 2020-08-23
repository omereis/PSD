
#include "psd_output.h"

TPsdOutput::TPsdOutput()
{
	m_mtxOut.clear ();
}

void TPsdOutput::AddSamples (float *afBuffer, uint32_t nSize)
{
	uint32_t n;
	TFloatVec v;

	for (n=0 ; n < nSize ; n++)
		v.push_back (afBuffer[n]);
	m_mtxOut.push_back (v);
}

uint32_t TPsdOutput::PulsesCount () const
{
	return ((uint32_t) m_mtxOut.size());
}

