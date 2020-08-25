#ifndef	_PSD_PARAMS_H
#define	_PSD_PARAMS_H

#include "bimam_types.h"
#include <string>
#include "redpitaya/rp.h"
#include "trig_params.h"
#include "sampling_params.h"
using namespace std;

/*
commands
	h - help
	i - iterations (-1 == inf)
	j - input json parameters
	l - long
	n - samples (buffer length)
	p - output psd results
	r - output raw name
	s - short length
	t - trigger levet
	w - raw pulses to save
*/

class TPsdParams {
public:
    TPsdParams ();
    TPsdParams (const TPsdParams &other);
    TPsdParams operator= (const TPsdParams &other);
    bool operator== (const TPsdParams &other) const;
    bool operator!= (const TPsdParams &other) const;
    void Clear ();

// I/O
	bool LoadFromJson (const string &strJson);
	void print ();

	void SetTriggerLevel (float rTrigger);
// getters/setters
    void SetTrigger (const TTriggerParams &trigger);
    TTriggerParams GetTrigger() const;
    void SetSamplingParams (const TSamplingParams &params);
    TSamplingParams GetSamplingParams () const;
    void SetSamples (int nSamples);
    int GetSamples () const;
    void SetIterations (int nIterations);
    int GetIterations () const;
    void SetShort (float rShort);
    float GetShort () const;
    void SetLong (float rLong);
    float GetLong () const;
	void SetPulses (int nPulses);
	int GetPulses () const;
	void SetSaveRaw(int nSaveRaw);
	int GetSaveRaw () const;
	void SetJsonFile (const string &strJson);
	string GetJsonFile () const;
	void SetPsdFile (const string &strPsdFile);
	string GetPsdFile () const;
	void SetRawFile (const string &strFile);
	string GetRawFile() const;
protected:
    void AssignAll (const TPsdParams &other);
private:
    TTriggerParams m_trigger;
    TSamplingParams m_sampling_params;
    int m_nSamples;
    int m_nIterations;
    float m_rShort;
    float m_rLong;
	int m_nPulses;
	int m_nSaveRaw;
	string m_strJsonFile;
	string m_strPsdFile;
	string m_strRawFile;
};
#endif
