#include <stdarg.h>
#include "proj_misc.h"
#include <algorithm>

//-----------------------------------------------------------------------------
string string_to_upper (const string &strBase)
{
    string str (strBase);
	string::iterator istr;

	for (istr=str.begin() ; istr != str.end() ; istr++)
		*istr = toupper (*istr);
    return (str);
}
//-----------------------------------------------------------------------------
string sampling_rate_name (rp_acq_sampling_rate_t rate)
{
    string strName;

    if (rate == RP_SMP_125M)
		strName = "RP_SMP_125M";
    else if (rate == RP_SMP_15_625M)
		strName = "RP_SMP_15_625M";
    else if (rate == RP_SMP_1_953M)
		strName = "RP_SMP_1_953M";
    else if (rate == RP_SMP_122_070K)
		strName = "RP_SMP_122_070K";
    else if (rate == RP_SMP_15_258K)
		strName = "RP_SMP_15_258K";
    else if (rate == RP_SMP_1_907K)
		strName = "RP_SMP_1_907K";
	else
		strName = "sampling rate error";
	return (strName);
}
//-----------------------------------------------------------------------------
rp_acq_sampling_rate_t rate_from_name (const string &strName)
{
    rp_acq_sampling_rate_t rate;
    std::string str = string_to_upper(strName);

	if (str == "RP_SMP_125M")
		rate = RP_SMP_125M;
	else if (str == "RP_SMP_15_625M")
		rate = RP_SMP_15_625M;
	else if (str == "RP_SMP_1_953M")
		rate = RP_SMP_1_953M;
	else if (str == "RP_SMP_122_070K")
		rate = RP_SMP_122_070K;
	else if (str == "RP_SMP_15_258K")
		rate = RP_SMP_15_258K;
	else if (str == "RP_SMP_1_907K")
		rate = RP_SMP_1_907K;
	else;
		rate = RP_SMP_125M; // default
	return (rate);
}
//-----------------------------------------------------------------------------
string decimation_name (rp_acq_decimation_t dec)
{
	string str;

    if (dec ==RP_DEC_1)
		str = "RP_DEC_1";
    else if (dec ==RP_DEC_8)
		str = "RP_DEC_8";
    else if (dec ==RP_DEC_64)
		str = "RP_DEC_64";
    else if (dec ==RP_DEC_1024)
		str = "RP_DEC_1024";
    else if (dec ==RP_DEC_8192)
		str = "RP_DEC_8192";
    else if (dec ==RP_DEC_65536)
		str = "RP_DEC_65536";
	else 
		str = "decimation error";
    return (str);
}
//-----------------------------------------------------------------------------
rp_acq_decimation_t decimation_name (const string &strName)
{
	rp_acq_decimation_t dec;
    std::string str = string_to_upper(strName);
	
    if (str == "RP_DEC_1")
		dec = RP_DEC_1;
    else if (str == "RP_DEC_8")
		dec = RP_DEC_8;
    else if (str == "RP_DEC_64")
		dec = RP_DEC_64;
    else if (str == "RP_DEC_1024")
		dec = RP_DEC_1024;
    else if (str == "RP_DEC_8192")
		dec = RP_DEC_8192;
    else if (str == "RP_DEC_65536")
		dec = RP_DEC_65536;
	else
		dec = RP_DEC_1; // default
    return (dec);
}
//-----------------------------------------------------------------------------
string trigger_source_name (rp_acq_trig_src_t src)
{
	string str;

    if (src == RP_TRIG_SRC_DISABLED)
		str = "RP_TRIG_SRC_DISABLED";
    else if (src == RP_TRIG_SRC_NOW)
		str = "RP_TRIG_SRC_NOW";
    else if (src == RP_TRIG_SRC_CHA_PE)
		str = "RP_TRIG_SRC_CHA_PE";
    else if (src == RP_TRIG_SRC_CHA_NE)
		str = "RP_TRIG_SRC_CHA_NE";
    else if (src == RP_TRIG_SRC_CHB_PE)
		str = "RP_TRIG_SRC_CHB_PE";
    else if (src == RP_TRIG_SRC_CHB_NE)
		str = "RP_TRIG_SRC_CHB_NE";
    else if (src == RP_TRIG_SRC_EXT_PE)
		str = "RP_TRIG_SRC_EXT_PE";
    else if (src == RP_TRIG_SRC_EXT_NE)
		str = "RP_TRIG_SRC_EXT_NE";
    else if (src == RP_TRIG_SRC_AWG_PE)
		str = "RP_TRIG_SRC_AWG_PE";
    else if (src == RP_TRIG_SRC_AWG_NE)
		str = "RP_TRIG_SRC_AWG_NE";
    else
		str = "trigger source error";
	return (str);
}
//-----------------------------------------------------------------------------
rp_acq_trig_src_t trigger_source_from_name (const string &strName)
{
	rp_acq_trig_src_t src;
    std::string str = string_to_upper(strName);

    if (str == "RP_TRIG_SRC_DISABLED")
		src = RP_TRIG_SRC_DISABLED;
    else if (str == "RP_TRIG_SRC_NOW")
		src = RP_TRIG_SRC_NOW;
    else if (str == "RP_TRIG_SRC_CHA_PE")
		src = RP_TRIG_SRC_CHA_PE;
    else if (str == "RP_TRIG_SRC_CHA_NE")
		src = RP_TRIG_SRC_CHA_NE;
    else if (str == "RP_TRIG_SRC_CHB_PE")
		src = RP_TRIG_SRC_CHB_PE;
    else if (str == "RP_TRIG_SRC_CHB_NE")
		src = RP_TRIG_SRC_CHB_NE;
    else if (str == "RP_TRIG_SRC_EXT_PE")
		src = RP_TRIG_SRC_EXT_PE;
    else if (str == "RP_TRIG_SRC_EXT_NE")
		src = RP_TRIG_SRC_EXT_NE;
    else if (str == "RP_TRIG_SRC_AWG_PE")
		src = RP_TRIG_SRC_AWG_PE;
    else if (str == "RP_TRIG_SRC_AWG_NE")
		src = RP_TRIG_SRC_AWG_NE;
	return (src);
}
//-----------------------------------------------------------------------------
void ExitWithError (const char * format, ...)
{
/*
Source: Example in http://www.cplusplus.com/reference/cstdio/vsprintf/
*/
	char szBuffer[1024];
	va_list args;

	va_start (args, format);
	vsprintf (szBuffer,format, args);
	va_end (args);
	fprintf (stderr, "%s\n", szBuffer);
	exit (-1);
}

string FormatWithComma (int num)
{
	vector<string> vstr;
	char sz[10];
	const char *szFormat;
	int n;
	string strResult;

	while (num > 0) {
		n = num % 1000;
		//printf ("num=%d, n=%d\n", num, n);
		if (num >= 1000)
			szFormat = "%03d";
		else
			szFormat = "%d";
		sprintf (sz, szFormat, n);
		//printf ("converted string:%s\n", sz);
		vstr.push_back (string(sz));
		num /= 1000;
	}
	reverse(vstr.begin(), vstr.end());
	for (n=0 ; n < (int) vstr.size() ; n++) {
		strResult += vstr[n];
		if (n < (int) (vstr.size() - 1))
			strResult += ",";
	}
	return (strResult);
}

string EngineeringNotation (int nLog)
{
	string str;

	if (nLog == 3)
		str = "Kilo";
	else if (nLog == 6)
		str = "Mega";
	else if (nLog == 9)
		str = "Giga";
	else if (nLog == 12)
		str = "Tera";
	else if (nLog == -3)
		str = "Milli";
	else if (nLog == -6)
		str = "Micro";
	else if (nLog == -9)
		str = "Nano";
	else if (nLog == -12)
		str = "Pico";
	else
		str = "";
	return (str);
}

string FormatEngineeringUnits (double x)
{
	int exp = 0, sign = 1;
	string strNum ;

	if (x < 0.0) {
		x = -x;
		sign = -sign;
    }
	while (x >= 1000.0) {
		x /= 1000.0;
		exp += 3;
	}
	while (x < 1.0) {
		x *= 1000.0;
		exp -= 3;
	}
	if (sign < 0)
		x = -x;
	strNum = to_string(x) + " " + EngineeringNotation (exp);
	return (strNum);
/*
	static char[] incPrefixes = new[] { 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y' };
	//static char[] decPrefixes = new[] { "milli", "micro", "nano"};
	char[] decPrefixes = new[] { 'm', '\u03bc', 'n', 'p', 'f', 'a', 'z', 'y' };
	double dLog, dNum;
	int nLog;
	srting strNum, strPostfix;

	d = abs(d);
	dLog = log10(d);
	nLog = ((int) dLog) / 3;
	nLog *= 3;
	strNotation = EngineeringNotation (nLog);
	if (nLog > 0) {
		if (d > 1) {
			dNum /= pow(10.0, double (nLog));
		}
		else if (d < 1) {
		}
	}
	else {
		dLog = log10(dLog);
		nLogIdx = ((int) log10 (d)) / 3;
	}
*/
}
//-----------------------------------------------------------------------------
