
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
/*
//-----------------------------------------------------------------------------

double WindowAverage (TFloatVec::const_iterator iStart, int nLength)
{
	double dAvg;
	int n;
	TFloatVec::const_iterator i;

	for (i=iStart, n=0, dAvg = 0 ; n < nLength ; n++, i++)
		dAvg += *i;
	return (dAvg / (double) nLength);
}
//-----------------------------------------------------------------------------

void MoveAverageFilter(const TFloatVec &vSignal, TFloatVec &vFiltered, int nWindow)
{
	int n;
	TFloatVec::const_iterator iSignal, iWindow;
	TFloatVec::iterator iFiltered;

	vFiltered.resize (vSignal.size(), 0);
	for (n=0, iSignal=vSignal.begin(), iFiltered=vFiltered.begin() ; n < nWindow / 2 ; n++, iSignal++)
		*iFiltered = *iSignal;
	for (iWindow=vSignal.begin() ; n < (int) vSignal.size() - (nWindow / 2) ; iWindow++, iFiltered++, n++)
		*iFiltered = WindowAverage (iWindow, nWindow);
}
//-----------------------------------------------------------------------------
*/

void TPsdOutput::HandleNew(float *buff, uint32_t buff_size)
{
	TFloatVec vSignal, vFiltered;
	TFloatVec::iterator i;
	double dLong, dShort, t;
	TPsdOutParams out_params;

	ConvertSamples (buff, buff_size, vSignal);
	if ((int) PulsesCount() < m_params.GetSaveRaw ()) {
		MoveAverageFilter(vSignal, vFiltered, 7);
		m_mtxFiltered.push_back (vFiltered);
		m_mtxOut.push_back (vSignal);
	}
	dLong = dShort = t = 0;
	for (i=vSignal.begin() ; i != vSignal.end() ; i++) {
		if (t <= m_params.GetLong()) {
			dLong += *i;
			if (t < m_params.GetShort())
				dShort += *i;
		}
		t += 8e-9;
	}
	out_params.SetLongSum (dLong);
	out_params.SetShortSum (dShort);
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
	SaveMatrix (m_mtxFiltered, "filt.csv");
/*
	size_t row, col;
	FILE *file;
	string str;

	printf ("Save Raw:\n");
	printf ("Matrix Rows: %d\nMatrix Columns: %d\n", m_mtxOut.size(), m_mtxOut[0].size());
	file = fopen (strFile.c_str(), "w+");
	for (col=0 ; col < m_mtxOut[0].size() ; col++) {
		str = "";
		for (row=0 ; row < m_mtxOut.size() ; row++) {
			str += to_string(m_mtxOut[row][col]);
			if (row < (m_mtxOut.size() -1 ))
				str += ",";
		}
		fprintf (file, "%s\n", str.c_str());
	}
	fclose (file);
*/
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
