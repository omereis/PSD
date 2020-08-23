
#include "bimam_types.h"
#include <string>
#include "redpitaya/rp.h"
#include "trig_params.h"
#include "sampling_params.h"
using namespace std;

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
};
