
#include "sampling_params.h"
//-----------------------------------------------------------------------------
TSamplingParams::TSamplingParams()
{
    Clear ();
}
//-----------------------------------------------------------------------------
TSamplingParams::TSamplingParams (const TSamplingParams &other)
{
    AssignAll (other);
}
//-----------------------------------------------------------------------------
TSamplingParams::TSamplingParams (rp_acq_sampling_rate_t rate, rp_acq_decimation_t decimation)
{
    m_rate = rate;
    m_decimation = decimation;
}
//-----------------------------------------------------------------------------
TSamplingParams TSamplingParams::operator= (const TSamplingParams &other)
{
    AssignAll (other);
    return (*this);
}
//-----------------------------------------------------------------------------
bool TSamplingParams::operator== (const TSamplingParams &other) const
{
    if (GetRate () != other.GetRate ())
        return (false);
    if (GetDecimation () != other.GetDecimation ())
        return (false);
    return (true);
}
//-----------------------------------------------------------------------------
bool TSamplingParams::operator!= (const TSamplingParams &other) const
{
    return (!(*this == other));
}
//-----------------------------------------------------------------------------
void TSamplingParams::Clear ()
{
    SetRate (RP_SMP_125M);
    SetDecimation (RP_DEC_1);
}
//-----------------------------------------------------------------------------
void TSamplingParams::AssignAll (const TSamplingParams &other)
{
    SetRate (other.GetRate ());
    SetDecimation (other.GetDecimation());
}
//-----------------------------------------------------------------------------
void TSamplingParams::SetRate (rp_acq_sampling_rate_t rate)
{
    m_rate = rate;
}
//-----------------------------------------------------------------------------
rp_acq_sampling_rate_t TSamplingParams::GetRate () const
{
    return (m_rate);
}
//-----------------------------------------------------------------------------
void TSamplingParams::SetDecimation (rp_acq_decimation_t decimation)
{
    m_decimation = decimation;
}
//-----------------------------------------------------------------------------
rp_acq_decimation_t TSamplingParams::GetDecimation () const
{
    return (m_decimation);
}
//-----------------------------------------------------------------------------
