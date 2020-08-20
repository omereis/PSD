
#include "trig_params.h"
#include "proj_misc.h"

//-----------------------------------------------------------------------------
TTriggerParams::TTriggerParams ()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TTriggerParams::TTriggerParams (const TTriggerParams &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
TTriggerParams::TTriggerParams (float rLevel, int32_t nDelay, rp_acq_trig_src_t src)
{
    Clear ();
    m_rLevel = rLevel;
    m_nDelay = nDelay;
    m_src    = src;
}
//-----------------------------------------------------------------------------
TTriggerParams TTriggerParams::operator= (const TTriggerParams &other)
{
    AssignAll (other);
    return (*this);
}
//-----------------------------------------------------------------------------
bool TTriggerParams::operator== (const TTriggerParams &other) const
{
	if (GetLevel()      != other.GetLevel ())
        return (false);
	if (GetDelay()      != other.GetDelay ())
        return (false);
	if (GetTriggerSrc() != other.GetTriggerSrc ())
        return (false);
    return (true);
}
//-----------------------------------------------------------------------------
bool TTriggerParams::operator!= (const TTriggerParams &other) const
{
    return (!(*this == other));
}
//-----------------------------------------------------------------------------
void TTriggerParams::Clear ()
{
	SetLevel (10e-3);
	SetDelay (100);
	SetTriggerSource (RP_TRIG_SRC_CHA_PE);
}
//-----------------------------------------------------------------------------
void TTriggerParams::SetLevel (float fLevel)
{
    m_rLevel = fLevel;
}
//-----------------------------------------------------------------------------
float TTriggerParams::GetLevel () const
{
    return (m_rLevel);
}
//-----------------------------------------------------------------------------
void TTriggerParams::SetDelay (int32_t nDelay)
{
    if (m_nDelay >= 0)
        m_nDelay = nDelay;
}
//-----------------------------------------------------------------------------
int32_t TTriggerParams::GetDelay () const
{
    return (m_nDelay);
}
//-----------------------------------------------------------------------------
void TTriggerParams::SetTriggerSource (rp_acq_trig_src_t src)
{
    m_src = src;
}
//-----------------------------------------------------------------------------
rp_acq_trig_src_t TTriggerParams::GetTriggerSrc () const
{
    return m_src;
}
//-----------------------------------------------------------------------------
void TTriggerParams::AssignAll (const TTriggerParams &other)
{
	SetLevel (other.GetLevel ());
	SetDelay (other.GetDelay ());
	SetTriggerSource (other.GetTriggerSrc ());
}
//-----------------------------------------------------------------------------
string TTriggerParams::AsString ()
{
	string str;

	str = "Level: " + std::to_string (1e3 * GetLevel()) + string (" milliVolts\n");
	str += "Delay: " + std::to_string (GetDelay()) + "\n";
	str += "Source: " + trigger_source_name (GetTriggerSrc()) + "\n";
	return (str);
}
//-----------------------------------------------------------------------------
