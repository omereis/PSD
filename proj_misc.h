#ifndef  PROJ_MISC_H
#define  PROJ_MISC_H

#include <string>
using namespace std;

#include "redpitaya/rp.h"

rp_acq_sampling_rate_t rate_from_name (const string &strName);
string sampling_rate_name (rp_acq_sampling_rate_t rate);
string decimation_name (rp_acq_decimation_t dec);
rp_acq_decimation_t decimation_name (const string &strName);
string trigger_source_name (rp_acq_trig_src_t src);
rp_acq_trig_src_t trigger_source_from_name (const string &strName);
void ExitWithError (const char * format, ...);
#endif
