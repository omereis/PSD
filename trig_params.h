#ifndef	TRIG_PARAMS_H
#define	TRIG_PARAMS_H

#include <string>
using namespace std;
#include <jsoncpp/json/json.h>
#include "rp.h"
//#include "redpitaya/rp.h"

class TTriggerParams {
public:
	TTriggerParams ();
	TTriggerParams (const TTriggerParams &other);
	TTriggerParams (float rLevel, int32_t nDelay, rp_acq_trig_src_t src);
	TTriggerParams operator= (const TTriggerParams &other);
	bool operator== (const TTriggerParams &other) const;
	bool operator!= (const TTriggerParams &other) const;
	void Clear ();
// I/O
	void LoadFromJson(Json::Value &obj);
	string AsString ();
// getters/setters
	void SetLevel (float fLevel);
	float GetLevel () const;
	void SetDelay (int32_t nDelay);
	int32_t GetDelay () const;
	void SetTriggerSource (rp_acq_trig_src_t src);
	rp_acq_trig_src_t GetTriggerSrc () const;
protected:
	void AssignAll (const TTriggerParams &other);
private:
	float m_rLevel;
	int32_t m_nDelay;
	rp_acq_trig_src_t m_src;
};
#endif
