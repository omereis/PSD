#include "redpitaya/rp.h"
/*
typedef enum {
    RP_SMP_125M,     //!< Sample rate 125Msps; Buffer time length 131us; Decimation 1
    RP_SMP_15_625M,  //!< Sample rate 15.625Msps; Buffer time length 1.048ms; Decimation 8
    RP_SMP_1_953M,   //!< Sample rate 1.953Msps; Buffer time length 8.388ms; Decimation 64
    RP_SMP_122_070K, //!< Sample rate 122.070ksps; Buffer time length 134.2ms; Decimation 1024
    RP_SMP_15_258K,  //!< Sample rate 15.258ksps; Buffer time length 1.073s; Decimation 8192
    RP_SMP_1_907K    //!< Sample rate 1.907ksps; Buffer time length 8.589s; Decimation 65536
} rp_acq_sampling_rate_t;


typedef enum {
    RP_DEC_1,     //!< Sample rate 125Msps; Buffer time length 131us; Decimation 1
    RP_DEC_8,     //!< Sample rate 15.625Msps; Buffer time length 1.048ms; Decimation 8
    RP_DEC_64,    //!< Sample rate 1.953Msps; Buffer time length 8.388ms; Decimation 64
    RP_DEC_1024,  //!< Sample rate 122.070ksps; Buffer time length 134.2ms; Decimation 1024
    RP_DEC_8192,  //!< Sample rate 15.258ksps; Buffer time length 1.073s; Decimation 8192
    RP_DEC_65536  //!< Sample rate 1.907ksps; Buffer time length 8.589s; Decimation 65536
} rp_acq_decimation_t;
*/

class TSamplingParams {
public:
    TSamplingParams();
    TSamplingParams (const TSamplingParams &other);
    TSamplingParams (rp_acq_sampling_rate_t rate, rp_acq_decimation_t decimation);
    TSamplingParams operator= (const TSamplingParams &other);
    bool operator== (const TSamplingParams &other) const;
    bool operator!= (const TSamplingParams &other) const;
    void Clear ();
// getters / setters
    void SetRate (rp_acq_sampling_rate_t rate);
    rp_acq_sampling_rate_t GetRate () const;
    void SetDecimation (rp_acq_decimation_t decimation);
    rp_acq_decimation_t GetDecimation () const;
protected:
    void AssignAll (const TSamplingParams &other);
private:
    rp_acq_sampling_rate_t m_rate;
    rp_acq_decimation_t m_decimation;
};
