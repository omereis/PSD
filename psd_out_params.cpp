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

}
//-----------------------------------------------------------------------------
void TPsdOutParams::AssignAll 	(const TPsdOutParams &other)
{

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
TPsdOutParamsVec::TPsdOutParamsVec ()
	: TPsdOutParamsBaseVec ()
{
}
//-----------------------------------------------------------------------------
