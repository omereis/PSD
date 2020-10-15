#include "psd_out_params.h"
//-----------------------------------------------------------------------------
TPsdOutParams::TPsdOutParams ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TPsdOutParams::TPsdOutParams (const TPsdOutParams &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
TPsdOutParams TPsdOutParams::operator= (const TPsdOutParams &other)
{
    AssignAll (other);
    return (*this);
}
//-----------------------------------------------------------------------------
bool TPsdOutParams::operator== (const TPsdOutParams &other) const
{
	if (GetShortSum () != other.GetShortSum ())
		return (false);
	if (GetLongSum () != other.GetLongSum ())
		return (false);
	if (GetAmp () != other.GetAmp ())
		return (false);
	if (GetPulseLength () != other.GetPulseLength ())
		return (false);
	if (GetTau()          != other.GetTau())
		return (false);
	if (GetStart() != other.GetStart())
		return (false);
	if (GetEnd() != other.GetEnd())
		return (false);
    return (true);
}
//-----------------------------------------------------------------------------
bool TPsdOutParams::operator!= (const TPsdOutParams &other) const
{
    return (!(*this == other));
}
//-----------------------------------------------------------------------------
void TPsdOutParams::Clear ()
{
	SetShortSum (0);
	SetLongSum (0);
	SetAmp (0);
	SetPulseLength (0);
	SetTau (0);
	SetStart (0);
	SetEnd (0);
	i0 = iEnd = 0;
	m_dTotal = 0;
}
//-----------------------------------------------------------------------------
void TPsdOutParams::AssignAll 	(const TPsdOutParams &other)
{
	SetShortSum (other.GetShortSum());
	SetLongSum (other.GetLongSum());
	SetAmp (other.GetAmp());
	SetPulseLength (other.GetPulseLength());
	SetTau (other.GetTau());
	SetEnd (other.GetEnd());
	SetStart (other.GetStart());
	i0 = other.i0;
	iEnd = other.iEnd;
	m_dTotal = other.m_dTotal ;
}
//-----------------------------------------------------------------------------
void TPsdOutParams::SetShortSum (double dShortSum)
{
    m_dShortSum = dShortSum;
}
//-----------------------------------------------------------------------------
double TPsdOutParams::GetShortSum () const
{
    return (m_dShortSum);
}
//-----------------------------------------------------------------------------
void TPsdOutParams::SetLongSum (double dLongSum)
{
    m_dLongSum = dLongSum;
}
//-----------------------------------------------------------------------------
double TPsdOutParams::GetLongSum () const
{
    return (m_dLongSum);
}
//-----------------------------------------------------------------------------
void TPsdOutParams::SetAmp (double dAmp)
{
    m_dMaxAmp = dAmp;
}
//-----------------------------------------------------------------------------
double TPsdOutParams::GetAmp () const
{
    return (m_dMaxAmp);
}
//-----------------------------------------------------------------------------
void TPsdOutParams::SetPulseLength (double dLength)
{
    m_dPulseLength = dLength;
}
//-----------------------------------------------------------------------------
double TPsdOutParams::GetPulseLength () const
{
    return (m_dPulseLength);
}
//-----------------------------------------------------------------------------
void TPsdOutParams::SetTau (double dTau)
{
	m_dTau = dTau;
}
//-----------------------------------------------------------------------------
double TPsdOutParams::GetTau () const
{
	return (m_dTau);
}
void TPsdOutParams::SetStart (double dStart)
{
	m_dStart = dStart;
}
//-----------------------------------------------------------------------------
double TPsdOutParams::GetStart () const
{
	return (m_dStart);
}
//-----------------------------------------------------------------------------
void TPsdOutParams::SetEnd (double dEnd)
{
	m_dEnd = dEnd;
}
//-----------------------------------------------------------------------------
double TPsdOutParams::GetEnd () const
{
	return (m_dEnd);
}
//-----------------------------------------------------------------------------

void TPsdOutParams::AddShortSum (double d)
{
	SetShortSum (GetShortSum() + d);
}
//-----------------------------------------------------------------------------

void TPsdOutParams::AddLongSum (double d)
{
	SetLongSum (GetLongSum() + d);
}
//-----------------------------------------------------------------------------

void TPsdOutParams::AddAmp (double d)
{
	SetAmp (GetAmp() + d);
}
//-----------------------------------------------------------------------------

void TPsdOutParams::AddPulseLength (double d)
{
	SetPulseLength (GetPulseLength() + d);
}
//-----------------------------------------------------------------------------
TPsdOutParamsVec::TPsdOutParamsVec ()
	: TPsdOutParamsBaseVec ()
{
}
//-----------------------------------------------------------------------------
