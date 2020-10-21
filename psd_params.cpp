#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include "psd_params.h"
#include "proj_misc.h"

//-----------------------------------------------------------------------------
TPsdParams::TPsdParams ()
{
	Clear ();
}

//-----------------------------------------------------------------------------
TPsdParams::TPsdParams (const TPsdParams &other)
{
	AssignAll (other);
}

//-----------------------------------------------------------------------------
TPsdParams TPsdParams::operator= (const TPsdParams &other)
{
	AssignAll (other);
	return (*this);
}

//-----------------------------------------------------------------------------
bool TPsdParams::operator== (const TPsdParams &other) const
{
    if (GetTrigger () != other.GetTrigger())
		return (false);
    if (GetSamplingParams () != other.GetSamplingParams ())
		return (false);
    if (GetSamples () != other.GetSamples ())
		return (false);
    if (GetIterations () != other.GetIterations ())
		return (false);
    if (GetShort () != other.GetShort ())
		return (false);
    if (GetLong () != other.GetLong ())
		return (false);
    if (GetPulses () != other.GetPulses ())
		return (false);
	if (GetSaveRaw () != other.GetSaveRaw ())
		return (false);
	if (GetJsonFile() != other.GetJsonFile())
		return (false);
	if (GetPsdFile() != other.GetPsdFile())
		return (false);
	if (GetRawFile() != other.GetRawFile())
		return (false);
	if (GetAvgWindow () != other.GetAvgWindow())
		return (false);
	if (GetTimeWindowThreshold () != other.GetTimeWindowThreshold () )
		return (false);
	if (GetInputTau() != other.GetInputTau())
		return (false);
	if (GetInputVoltage() != other.GetInputVoltage())
		return (false);
	return (true);
}

//-----------------------------------------------------------------------------
bool TPsdParams::operator!= (const TPsdParams &other) const
{
	return (!(*this == other));
}

//-----------------------------------------------------------------------------
void TPsdParams::Clear ()
{
    SetTrigger (TTriggerParams (10e-3, 0, RP_TRIG_SRC_CHA_PE));
    SetSamplingParams (TSamplingParams (RP_SMP_125M, RP_DEC_1));
    SetSamples (6250);
    SetIterations (1000);
    SetShort (200);
    SetLong (500);
	SetPulses (0);
	SetSaveRaw (0);
	SetJsonFile ("psd_params.json");
	SetPsdFile("psd_results.csv");
	SetRawFile ("");
	SetAvgWindow (7);
	SetTimeWindowThreshold (1e-3);
	SetInputTau (0);
	SetInputVoltage (0);
}
//-----------------------------------------------------------------------------
void TPsdParams::AssignAll (const TPsdParams &other)
{
    SetTrigger (other.GetTrigger());
    SetSamplingParams (other.GetSamplingParams ());
    SetSamples (other.GetSamples ());
    SetIterations (other.GetIterations ());
    SetShort (other.GetShort ());
    SetLong (other.GetLong ());
	SetPulses (other.GetPulses ());
	SetSaveRaw (other.GetSaveRaw());
	SetJsonFile (other.GetJsonFile());
	SetPsdFile (other.GetPsdFile());
	SetRawFile (other.GetRawFile());
	SetAvgWindow (other.GetAvgWindow());
	SetTimeWindowThreshold (other.GetTimeWindowThreshold ());
	SetInputTau (other.GetInputTau());
	SetInputVoltage (other.GetInputVoltage());
}
//-----------------------------------------------------------------------------
void TPsdParams::SetTrigger (const TTriggerParams &trigger)
{
	m_trigger = trigger;
}
//-----------------------------------------------------------------------------
TTriggerParams TPsdParams::GetTrigger() const
{
	return (m_trigger);
}
//-----------------------------------------------------------------------------
void TPsdParams::SetSamplingParams (const TSamplingParams &sampling_params)
{
	m_sampling_params = sampling_params;
}
//-----------------------------------------------------------------------------
TSamplingParams TPsdParams::GetSamplingParams () const
{
	return (m_sampling_params);
}
//-----------------------------------------------------------------------------
void TPsdParams::SetSamples (int nSamples)
{
	m_nSamples = nSamples;
}
//-----------------------------------------------------------------------------
int TPsdParams::GetSamples () const
{
	return (m_nSamples);
}
//-----------------------------------------------------------------------------
void TPsdParams::SetIterations (int nIterations)
{
	m_nIterations = nIterations;
}
//-----------------------------------------------------------------------------
int TPsdParams::GetIterations () const
{
	return (m_nIterations);
}
//-----------------------------------------------------------------------------
void TPsdParams::SetShort (float rShort)
{
	m_rShort = rShort;
}
//-----------------------------------------------------------------------------
float TPsdParams::GetShort () const
{
	return (m_rShort);
}
//-----------------------------------------------------------------------------
void TPsdParams::SetLong (float rLong)
{
	m_rLong = rLong;
}
//-----------------------------------------------------------------------------
float TPsdParams::GetLong () const
{
	return (m_rLong);
}
//-----------------------------------------------------------------------------
bool TPsdParams::LoadFromJson (const string &strJson)
{
	ifstream ifs(strJson);
	Json::Reader reader;
	Json::Value obj;

	reader.parse (ifs, obj);
	SetSamples (obj["samples"].asInt());
	SetIterations (obj["iterations"].asInt());
	SetLong (obj["long"].asFloat());
	SetShort (obj["short"].asFloat());
	SetPulses(obj["pulses"].asInt());
	SetSaveRaw (obj["print_raw"].asInt());
	SetRawFile (obj["raw_file"].asString());
	SetAvgWindow (obj["filter_window"].asInt());
	SetTimeWindowThreshold (obj["TimeWindowThreshold"].asFloat());
	// time_window_threshold
	m_trigger.LoadFromJson(obj["trigger"]);
	m_sampling_params.LoadFromJson (obj["sampling"]);
	return (true);
}
//-----------------------------------------------------------------------------
void TPsdParams::print ()
{
	print (stdout);
}
void TPsdParams::print (FILE* file)
{
	fprintf (file, "Samples: %d\n", GetSamples ());
	fprintf (file, "Iterations: %d\n", GetIterations());
	fprintf (file, "Short Buffer: %s\n", FormatEngineeringUnits (GetShort()).c_str());
	fprintf (file, "Long Buffer: %s\n", FormatEngineeringUnits (GetLong ()).c_str());
	fprintf (file, "PSD output file: '%s'\n", GetPsdFile().c_str());
	fprintf (file, "Pulses to save: %d\n", GetPulses ());
	fprintf (file, "Raw signals to save: %d\n", GetSaveRaw ());
	fprintf (file, "Raw File Name: '%s'\n", GetRawFile().c_str());
	fprintf (file, "Trigger:\n%s\n", m_trigger.AsString().c_str());
	fprintf (file, "Sampling:\n%s\n", m_sampling_params.AsString().c_str());
	fprintf (file, "Input Tau: %g microSeconds\n", GetInputTau() * 1e6);
	fprintf (file, "Input Voltage: %g millivolts\n", GetInputVoltage() * 1e3); 
	fprintf (file, "Time Window Threshold: %g\n", GetTimeWindowThreshold());
}
//-----------------------------------------------------------------------------
void TPsdParams::SetPulses (int nPulses)
{
	m_nPulses = nPulses;
}
//-----------------------------------------------------------------------------
int TPsdParams::GetPulses () const
{
	return (m_nPulses);
}
//-----------------------------------------------------------------------------
void TPsdParams::SetSaveRaw(int nSaveRaw)
{
	m_nSaveRaw = nSaveRaw;
}
//-----------------------------------------------------------------------------
int TPsdParams::GetSaveRaw () const
{
	return (m_nSaveRaw);
}
//-----------------------------------------------------------------------------
void TPsdParams::SetJsonFile (const string &strJson)
{
	m_strJsonFile = strJson;
}
//-----------------------------------------------------------------------------
string TPsdParams::GetJsonFile () const
{
	return (m_strJsonFile);
}
//-----------------------------------------------------------------------------
void TPsdParams::SetPsdFile (const string &strPsdFile)
{
	m_strPsdFile = strPsdFile;
}
//void TPsdParams::SetPsdFile ("psd_results.csv");
//-----------------------------------------------------------------------------
string TPsdParams::GetPsdFile () const
{
	return (m_strPsdFile);
}
//-----------------------------------------------------------------------------
void TPsdParams::SetTriggerLevel (float rTrigger)
{
	m_trigger.SetLevel (rTrigger);
	//printf ("\n\nTrigger Level set to %s\n\n", m_trigger.AsString().c_str());
}
//-----------------------------------------------------------------------------
void TPsdParams::SetRawFile (const string &strFile)
{
	m_strRawFile = strFile;
}
//-----------------------------------------------------------------------------

string TPsdParams::GetRawFile() const
{
	return (m_strRawFile);
}
//-----------------------------------------------------------------------------

void TPsdParams::SetAvgWindow (int nAvgWin)
{
	m_nAvgWin = nAvgWin;
}
//-----------------------------------------------------------------------------

int TPsdParams::GetAvgWindow () const
{
	return (m_nAvgWin);
}

//-----------------------------------------------------------------------------
void TPsdParams::SetTimeWindowThreshold (float rThreshold)
{
	m_rTimeWindowThreshold = rThreshold;
}

//-----------------------------------------------------------------------------
float TPsdParams::GetTimeWindowThreshold () const
{
	return (m_rTimeWindowThreshold);
}

void TPsdParams::SetInputTau (double d)
{
	m_dTau = d;
}

//-----------------------------------------------------------------------------

double TPsdParams::GetInputTau () const
{
	return (m_dTau);
}

//-----------------------------------------------------------------------------
void TPsdParams::SetInputVoltage (double d)
{
	m_dInVoltage = d;
}

//-----------------------------------------------------------------------------
double TPsdParams::GetInputVoltage () const
{
	return (m_dInVoltage);
}

