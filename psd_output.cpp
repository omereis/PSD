
/*---------------------------------------------------------------------------*\
| P S D _ O U T P U T . C P P                                                 |
\*---------------------------------------------------------------------------*/
#include "proj_misc.h"
#include <stdio.h>
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
//-----------------------------------------------------------------------------

TPsdOutput::TPsdOutput(const TPsdOutput &other)
{
	AssignAll (other);
}
//-----------------------------------------------------------------------------

TPsdOutput TPsdOutput::operator= (const TPsdOutput &other)
{
	AssignAll (other);
	return (*this);
}
//-----------------------------------------------------------------------------

void TPsdOutput::AssignAll (const TPsdOutput &other)
{
	SetParams(other.GetParams());
	m_mtxOut = other.m_mtxOut;
}
//-----------------------------------------------------------------------------

bool TPsdOutput::operator== (const TPsdOutput &other) const
{
	if (GetParams() != other.GetParams())
		return (false);
	if (m_mtxOut != other.m_mtxOut)
		return (false);
	return (true);
}
//-----------------------------------------------------------------------------

bool TPsdOutput::operator!= (const TPsdOutput &other) const
{
	return (!(*this == other));
}
//-----------------------------------------------------------------------------

void TPsdOutput::ConvertSamples (float *afBuffer, uint32_t nSize, TFloatVec &vSignal)
{
	uint32_t n;

	for (n=0 ; n < nSize ; n++)
		vSignal.push_back (afBuffer[n]);
}
//-----------------------------------------------------------------------------

uint32_t TPsdOutput::PulsesCount () const
{
	return ((uint32_t) m_mtxOut.size());
}
//-----------------------------------------------------------------------------

void TPsdOutput::Clear ()
{
	TFloatMatrix::iterator imtx;

	for (imtx = m_mtxOut.begin() ; imtx != m_mtxOut.end() ; imtx++)
		imtx->clear ();
	m_mtxOut.clear ();
	m_params.Clear ();
}
//-----------------------------------------------------------------------------

void TPsdOutput::SetParams (const TPsdParams &params)
{
	m_params = params;
}
//-----------------------------------------------------------------------------

TPsdParams TPsdOutput::GetParams() const
{
	return (m_params);
}
//-----------------------------------------------------------------------------

void TPsdOutput::HandleNew(float *buff, uint32_t buff_size)
{
	TFloatVec vSignal, vFiltered;
	TFloatVec::iterator i;
	double dLong, dShort, t, tStart, tPulse;
	float rMax = 0;
	TPsdOutParams out_params;
	bool fInPulse;

	ConvertSamples (buff, buff_size, vSignal);
	MoveAverageFilter(vSignal, vFiltered, m_params.GetAvgWindow());
	if ((int) PulsesCount() < m_params.GetSaveRaw ()) {
		m_mtxOut.push_back (vSignal);
		m_mtxFiltered.push_back (vFiltered);
	}
	tPulse = dLong = dShort = t = 0;
	fInPulse = false;
	for (i=vFiltered.begin() ; i != vFiltered.end() ; i++) {
		rMax = max (rMax, *i);
		if (t <= m_params.GetLong()) {
			dLong += *i;
			if (t < m_params.GetShort())
				dShort += *i;
		}
		if (!fInPulse) {
			if ((tPulse == 0) && (*i > m_params.GetTimeWindowThreshold ())) {
				tStart = t;
				fInPulse = true;
			}
		}
		else {
			if (*i < m_params.GetTimeWindowThreshold ()) {
				tPulse = t - tStart;
				fInPulse = false;
			}
		}
		t += 8e-9;
	}
	out_params.SetLongSum (dLong);
	out_params.SetShortSum (dShort);
	out_params.SetAmp (rMax);
	out_params.SetPulseLength (tPulse);
	m_vPsdParams.push_back (out_params);
}
//-----------------------------------------------------------------------------

void TPsdOutput::SaveResults ()
{
	SaveRaw (m_params.GetRawFile());
	SavePsd (m_params.GetPsdFile());
}
//-----------------------------------------------------------------------------

void SaveMatrix (TFloatMatrix &mtx, const string &strFile)
{
	size_t row, col;
	FILE *file;
	string str;

	printf ("Save Raw:\n");
	printf ("Matrix Rows: %d\nMatrix Columns: %d\n", mtx.size(), mtx[0].size());
	file = fopen (strFile.c_str(), "w+");
	for (col=0 ; col < mtx[0].size() ; col++) {
		str = "";
		for (row=0 ; row < mtx.size() ; row++) {
			str += to_string(mtx[row][col]);
			if (row < (mtx.size() -1 ))
				str += ",";
		}
		fprintf (file, "%s\n", str.c_str());
	}
	fclose (file);
}
//-----------------------------------------------------------------------------

void TPsdOutput::SaveRaw (const string &strFile)
{
	SaveMatrix (m_mtxOut, strFile);
	printf ("\n\nSaving Filtered Matrix\n");
	SaveMatrix (m_mtxFiltered, "filt.csv");
}
//-----------------------------------------------------------------------------

void TPsdOutput::SavePsd (const string &strFile)
{
	FILE *file;
	TPsdOutParamsVec::iterator i;
	int n;

	file = fopen (strFile.c_str(), "w+");
	fprintf (file, "No, Long, Short, Vmax, Length\n");
	for (i=m_vPsdParams.begin(), n=0 ; i != m_vPsdParams.end() ; i++)
		fprintf (file, "%d,%g,%g,%g,%g\n", n++, i->GetLongSum(), i->GetShortSum(), i->GetAmp(), i->GetPulseLength());
	fclose (file);
}
