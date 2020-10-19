#ifndef	_PSD_OUT_PARAMS_H
#define	_PSD_OUT_PARAMS_H
//-----------------------------------------------------------------------------
#include <vector>
using namespace std;
//-----------------------------------------------------------------------------
class TPsdOutParams;
typedef vector<TPsdOutParams> TPsdOutParamsBaseVec;
//-----------------------------------------------------------------------------
class TPsdOutParams {
public:
	TPsdOutParams ();
	TPsdOutParams (const TPsdOutParams &other);
	TPsdOutParams operator= (const TPsdOutParams &other);
	bool operator== (const TPsdOutParams &other) const;
	bool operator!= (const TPsdOutParams &other) const;
	void Clear ();
	void AddShortSum (double d);
	void AddLongSum (double d);
	void AddAmp (double d);
	void AddPulseLength(double d);
// Getters/Setters
	void SetShortSum (double dShortSum);
	double GetShortSum () const;
	void SetLongSum (double dLongSum);
	double GetLongSum () const;
	void SetAmp (double dAmp);
	double GetAmp () const;
	void SetPulseLength (double dLength);
	double GetPulseLength () const;
	void SetTau (double dTau);
	double GetTau () const;
	void SetStart (double dStart);
	double GetStart () const;
	void SetEnd (double dEnd);
	double GetEnd () const;
	int i0, iEnd;
	double m_dTotal;
protected:
	void AssignAll 	(const TPsdOutParams &other);
private:
	double m_dShortSum;
	double m_dLongSum;
	double m_dMaxAmp;
	double m_dPulseLength;
	double m_dTau;
	double m_dStart;
	double m_dEnd;
};
//-----------------------------------------------------------------------------
class TPsdOutParamsVec : public TPsdOutParamsBaseVec {
public:
	TPsdOutParamsVec();
};
//-----------------------------------------------------------------------------
#endif
