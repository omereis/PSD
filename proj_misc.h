#ifndef  PROJ_MISC_H
#define  PROJ_MISC_H

#include "bimam_types.h"
#include <string>
#include <vector>
using namespace std;

#include "rp.h"
//#include "redpitaya/rp.h"

string FormatWithComma (int num);
rp_acq_sampling_rate_t rate_from_name (const string &strName);
string sampling_rate_name (rp_acq_sampling_rate_t rate);
string decimation_name (rp_acq_decimation_t dec);
rp_acq_decimation_t decimation_name (const string &strName);
string trigger_source_name (rp_acq_trig_src_t src);
rp_acq_trig_src_t trigger_source_from_name (const string &strName);
void ExitWithError (const char * format, ...);
string FormatEngineeringUnits (double x);
double WindowAverage (TFloatVec::const_iterator iStart, int nLength);
void MoveAverageFilter(const TFloatVec &vSignal, TFloatVec &vFiltered, int nWindow);
#endif
