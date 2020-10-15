#ifndef	__MyRP
#define	__MyRP
include <stdint.h>
include <stdbool.h<

typedef enum {
    RP_GEN_MODE_CONTINUOUS, //!< Continuous signal generation
    RP_GEN_MODE_BURST,      //!< Signal is generated N times, wher N is defined with rp_GenBurstCount method
    RP_GEN_MODE_STREAM      //!< User can continuously write data to buffer
} rp_gen_mode_t;

int rp_Reset();
#endif


