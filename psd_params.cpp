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
	m_trigger.LoadFromJson(obj["trigger"]);
	return (true);
}
//-----------------------------------------------------------------------------
void TPsdParams::print ()
{
	printf ("Samples: %d\n", GetSamples ());
	printf ("Iterations: %d\n", GetIterations());
	printf ("Short Buffer: %g\n", GetShort());
	printf ("Long Buffer: %g\n", GetLong ());
	printf ("Trigger:\n%s\n", m_trigger.AsString().c_str());
}
//-----------------------------------------------------------------------------
