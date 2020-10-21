
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

bool TPsdOutput::HandleNew(float *buff, uint32_t buff_size)
{
	TFloatVec vSignal, vFiltered;
	TFloatVec::iterator i;
	double dLong, dShort, t, tStart, tPulse, tEnd, dTotal;
	float rMax = 0;
	TPsdOutParams out_params;
	bool fInPulse, fAdded;
	int n, i0=-1, iEnd=0;

	ConvertSamples (buff, buff_size, vSignal);
	MoveAverageFilter(vSignal, vFiltered, m_params.GetAvgWindow());
	//printf ("Vector filtered\n");
	if ((int) PulsesCount() < m_params.GetSaveRaw ()) {
		m_mtxOut.push_back (vSignal);
		m_mtxFiltered.push_back (vFiltered);
	}
	dLong = dShort = t = tEnd = dTotal = 0;
	tPulse = -1;
	n=0;
	fInPulse = false;
	for (i=vFiltered.begin(), n=0 ; (i != vFiltered.end()) && (tPulse < 0) ; i++, n++) {
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
			if (*i > m_params.GetTimeWindowThreshold ()) {
			//if ((tPulse == 0) && (*i > m_params.GetTimeWindowThreshold ())) {
				tStart = t;
				fInPulse = true;
				i0 = n;
			}
		}
		t += 8e-9;
		n++;
	}
	if (tPulse >= 5e-9) {
		out_params.SetLongSum (dLong);
		out_params.SetShortSum (dShort);
		out_params.SetAmp (rMax);
		out_params.SetPulseLength (tPulse);
		//printf ("Debug: Pulse Length: %g\n", tPulse);
		out_params.SetStart (tStart);
		out_params.SetEnd (tEnd);
		out_params.i0 = i0;
		out_params.iEnd = iEnd;
		out_params.m_dTotal = dTotal;
		m_vPsdParams.push_back (out_params);
		fAdded = true;
		//printf ("Pulse added\n");
	}
	else {
		//printf ("Pulse NOT added\n");
		fAdded = false;
		static bool flag = false;
		TFloatVec::iterator iSig, iFilt;

		if (flag == false) {
			flag = true;
			FILE *file = fopen ("h.csv", "w+");
			printf ("H file opened\n");
			fprintf (file, "Signal, Filtered\n");
			for (iSig=vSignal.begin(), iFilt=vFiltered.begin() ; iSig != vSignal.end() ; iSig++, iFilt++)
				fprintf (file, "%g,%g\n", *iSig, *iFilt);
			//printf ("Filtered vector printed to H\n");
			fprintf (file, "\nDebug\n");
			fprintf (file, "Found pulse length: %g\n", tPulse);
			fprintf (file, "Start index: %d\n", i0);
			tPulse = -1;
			fInPulse = false;
			TFloatVec::iterator iFound;
			float fDebug, fThreshold;
			int idxPass = -1, idxEnd=-1;
			fThreshold = m_params.GetTimeWindowThreshold ();
			for (i=vFiltered.begin(), n=0, t=0 ; (i != vFiltered.end()) && (tPulse < 0) ; i++, n++, t += 8e-9) {
				fDebug = *i;
				if (!fInPulse) {
					if (fDebug >= fThreshold) {
						if (idxPass < 0)
							idxPass = n;
					//if (!fInPulse) {
						tStart = t;
						fInPulse = true;
						i0 = n;
						iFound = i;
					}
				}
				else { // in pulse
					if (fDebug < fThreshold) {
						tPulse = t - tStart;
						idxEnd = n;
					}
				}
			}
			fprintf (file, "InPulse: %d\n", (int) fInPulse);
			fprintf (file, "Pass index: %d\n", idxPass);
			fprintf (file, "Start: %g\n", tStart);
			fprintf (file, "i0: %d\n", i0);
			fprintf (file, "Pulse Length: %g nano\n", tPulse * 1e9);
			fprintf (file, "Pulse end index: %d\n", idxEnd);
			m_params.print (file);
			fprintf (file, "Time window threshold: %g\n", fThreshold);
			//if (iFound != 0)
				//fprintf (file, "Value at found: %g\n", *iFound);
			//else
				//fprintf (file, "Value at found: NULL\n");
			fclose (file);
		}
		//printf ("Wrong pulse length: %g\n", tPulse);
	}
	return (fAdded);
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
		fprintf (file, "%d,%g,%g,%g,%g,%g,%g,%g,%d,%d,%g\n", ++n, i->GetLongSum(), i->GetShortSum(), i->GetAmp(), i->GetPulseLength(), 1e9 * i->GetPulseLength(), i->GetStart(), i->GetEnd(), i->i0, i->iEnd, i->m_dTotal);
		//fprintf (file, "%d,%g,%g,%g,%g,%g,%g,%g\n", n++, i->GetLongSum(), i->GetShortSum(), i->GetAmp(), i->GetPulseLength(), i->GetTau(),
		//											i->GetStart(), i->GetEnd());
	fclose (file);
}
//-----------------------------------------------------------------------------

#include <unistd.h>

bool TPsdOutput::SaveResultsMean (const TPsdParams &in_params, const string &strFileName)
{
	FILE *file;

	if (access (strFileName.c_str(), W_OK) == 0) {
		file = fopen (strFileName.c_str(), "a+");
	}
	else {
		file = fopen (strFileName.c_str(), "w+");
		fprintf (file, "Input Voltage, Input Tau, Long Charge, Short Charge, Amplitude, Pulse length[uSec]\n"); 
	}
	bool f = SaveResultsMean (in_params, file);
	fclose (file);
	return (f);
}
//-----------------------------------------------------------------------------

bool TPsdOutput::SaveResultsMean (const TPsdParams &in_params, FILE *file)
{
	TPsdOutParams po;
	float n;
	TPsdOutParamsVec::iterator i;
	bool f;

	for (i=m_vPsdParams.begin(), n=0 ; i != m_vPsdParams.end() ; i++, n++) {
		po.AddLongSum (i->GetLongSum());
		po.AddShortSum (i->GetShortSum());
		po.AddAmp (i->GetAmp());
		po.AddPulseLength(i->GetPulseLength());
	}
	if (n > 0) {
		po.SetLongSum (po.GetLongSum() / n);
		po.SetShortSum (po.GetShortSum() / n);
		po.SetAmp (po.GetAmp() / n);
		po.SetPulseLength (po.GetPulseLength() / n);
	}
	try {
		if (file) {
			fprintf (file, "%g,%g,%g,%g,%g,%g\n", in_params.GetInputVoltage() * 1e3, in_params.GetInputTau () * 1e6,
												po.GetLongSum(), po.GetShortSum(), po.GetAmp(), 1e6 * po.GetPulseLength());
			f = true;
		}
	}
	catch (exception &e) {
		fprintf (stderr, "%s\n", e.what());
		f = false;
	}
	return (f);
}


