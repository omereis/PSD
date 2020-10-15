#ifndef  __BIMAM_TYPES_H
#define  __BIMAM_TYPES_H

#include <vector>
using namespace std;

#ifndef  TFloatVec
typedef vector<float> TFloatVec;
#endif

#ifndef  TFloatMatrix
typedef vector<TFloatVec> TFloatMatrix;
#endif

#ifndef  TIntVec
typedef vector<int> TIntVec;
#endif

#endif
