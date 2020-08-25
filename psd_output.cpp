
#include "psd_output.h"

/*
template <class T>
typedef vector < <vector<t> > 
template <class T>
bool CompareMatrix (
*/
TPsdOutput::TPsdOutput()
{
	Clear ();
}

TPsdOutput::TPsdOutput(const TPsdOutput &other)
{
	AssignAll (other);
}

TPsdOutput TPsdOutput::operator= (const TPsdOutput &other)
{
	AssignAll (other);
	return (*this);
}

void TPsdOutput::AssignAll (const TPsdOutput &other)
{
	SetParams(other.GetParams());
	m_mtxOut = other.m_mtxOut;
}

bool TPsdOutput::operator== (const TPsdOutput &other) const
{
	if (GetParams() != other.GetParams())
		return (false);
	if (m_mtxOut != other.m_mtxOut)
		return (false);
	return (true);
}

bool TPsdOutput::operator!= (const TPsdOutput &other) const
{
	return (!(*this == other));
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

void TPsdOutput::Clear ()
{
	TFloatMatrix::iterator imtx;

	for (imtx = m_mtxOut.begin() ; imtx != m_mtxOut.end() ; imtx++)
		imtx->clear ();
	m_mtxOut.clear ();
	m_params.Clear ();
}

void TPsdOutput::SetParams (const TPsdParams &params)
{
	m_params = params;
}

TPsdParams TPsdOutput::GetParams() const
{
	return (m_params);
}

void TPsdOutput::HandleNew(float *buff, uint32_t buff_size)
{
	if ((int) PulsesCount() < m_params.GetSaveRaw ()) {
		AddSamples (buff, buff_size);
	}
}
