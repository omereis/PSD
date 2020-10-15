
/*---------------------------------------------------------------------------*\
| P S D _ O U T P U T . C P P                                                 |
\*---------------------------------------------------------------------------*/
#include "proj_misc.h"
#include <stdio.h>
#include "psd_output.h"
#include <math.h>

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
	double dLong, dShort, t, tStart, tPulse, tEnd, dTotal;
	float rMax = 0;
	TPsdOutParams out_params;
	bool fInPulse;
	int n, i0=0, iEnd=0;

	ConvertSamples (buff, buff_size, vSignal);
	MoveAverageFilter(vSignal, vFiltered, m_params.GetAvgWindow());
	if ((int) PulsesCount() < m_params.GetSaveRaw ()) {
		m_mtxOut.push_back (vSignal);
		m_mtxFiltered.push_back (vFiltered);
	}
	tPulse = dLong = dShort = t = tEnd = dTotal = 0;
	n=0;
	fInPulse = false;
	for (i=vFiltered.begin() ; i != vFiltered.end() ; i++) {
		dTotal += *i;
		rMax = max (rMax, *i);
		if (fInPulse) {
			if ((t - tStart) <= m_params.GetLong()) {
				dLong += *i;
				if ((t - tStart) < m_params.GetShort())
					dShort += *i;
			}
			if (*i < m_params.GetTimeWindowThreshold ()) {
				tPulse = t - tStart;
				tEnd = t;
				fInPulse = false;
				iEnd = n;
			}
		}
		else {
		//if (!fInPulse) {
			if ((tPulse == 0) && (*i > m_params.GetTimeWindowThreshold ())) {
				tStart = t;
				fInPulse = true;
				i0 = n;
			}
		}
/*
		else {
			if ((fInPulse) && (*i < m_params.GetTimeWindowThreshold ())) {
				tPulse = t - tStart;
				tEnd = t;
				fInPulse = false;
			}
		}
*/
		t += 8e-9;
		n++;
	}
	out_params.SetLongSum (dLong);
	out_params.SetShortSum (dShort);
	out_params.SetAmp (rMax);
	out_params.SetPulseLength (tPulse);
	out_params.SetStart (tStart);
	out_params.SetEnd (tEnd);
	out_params.i0 = i0;
	out_params.iEnd = iEnd;
	out_params.m_dTotal = dTotal;
/*
	if (rMax > 0) {
		double dDenom, dTau = 0;

		dDenom = m_params.GetTimeWindowThreshold() / rMax;
		if (dDenom > 0)
			dTau = tPulse / log(dDenom);
		out_params.SetTau (dTau);
	}
*/
	m_vPsdParams.push_back (out_params);
}
//-----------------------------------------------------------------------------

/*
void TPsdOutput::HandleNew(float *buff, int16_t *ai16Buf, uint32_t buff_size)
{
	TFloatVec vSignal, vFiltered, vRaw;
	TFloatVec::iterator i;
	double dLong, dShort, t, tStart, tPulse;
	float rMax = 0;
	TPsdOutParams out_params;
	bool fInPulse;

	ConvertSamples (buff, buff_size, vSignal);
	MoveAverageFilter(vSignal, vFiltered, m_params.GetAvgWindow());
	if ((int) PulsesCount() < m_params.GetSaveRaw ()) {
		//int n;
		//vRaw.resize(buff_size);
		//for (i=vRaw.begin(), n=0 ; i != vRaw.end() ; i++, n++)
		//	*i = (float) ai16Buf[n];
		m_mtxOut.push_back (vSignal);
		m_mtxFiltered.push_back (vFiltered);
		//m_mtxRaw.push_back (vRaw);
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
*/

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
	//SaveMatrix (m_mtxRaw, "raw.csv");
}
//-----------------------------------------------------------------------------

void TPsdOutput::SavePsd (const string &strFile)
{
	FILE *file;
	TPsdOutParamsVec::iterator i;
	int n;

	file = fopen (strFile.c_str(), "w+");
	fprintf (file, "No, Long, Short, Vmax, Length, Len[nano],Start,End,total\n");
	for (i=m_vPsdParams.begin(), n=0 ; i != m_vPsdParams.end() ; i++)
		fprintf (file, "%d,%g,%g,%g,%g,%g,%g,%g,%d,%d,%g\n", n++, i->GetLongSum(), i->GetShortSum(), i->GetAmp(), i->GetPulseLength(), 1e9 * i->GetPulseLength(), i->GetStart(), i->GetEnd(), i->i0, i->iEnd, i->m_dTotal);
		//fprintf (file, "%d,%g,%g,%g,%g,%g,%g,%g\n", n++, i->GetLongSum(), i->GetShortSum(), i->GetAmp(), i->GetPulseLength(), i->GetTau(),
		//											i->GetStart(), i->GetEnd());
	fclose (file);
}
//-----------------------------------------------------------------------------

bool TPsdOutput::SaveResultsMean (FILE *file)
{
	TPsdOutParams po;
	float n;
	TPsdOutParamsVec::iterator i;
	bool f;

	for (i=m_vPsdParams.begin(), n=0 ; i != m_vPsdParams.end() ; i++, n++) {
		po.AddShortSum (i->GetShortSum());
		po.AddLongSum (i->GetLongSum());
		po.AddAmp (i->GetAmp());
		po.AddPulseLength(i->GetPulseLength());
	}
	if (n > 0) {
		po.SetShortSum (po.GetLongSum() / n);
		po.SetLongSum (po.GetLongSum() / n);
		po.SetAmp (po.GetAmp() / n);
		po.SetPulseLength (po.GetPulseLength() / n);
	}
	try {
		if (file) {
			fprintf (file, "%g,%g,%g,%g,%g\n", po.GetLongSum(), po.GetShortSum(), po.GetAmp(), po.GetPulseLength(), 1e9 * po.GetPulseLength());
			f = true;
		}
	}
	catch (exception &e) {
		fprintf (stderr, "%s\n", e.what());
		f = false;
	}
	return (f);
}


