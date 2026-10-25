#ifndef PROPAGATOR_CUH
#define PROPAGATOR_CUH

#include "propagator.h"

#ifdef __cplusplus
extern "C" {
	void propagate(Models *models, Fields *fields, CPML *cpml, Config config);
}
#endif

#endif
