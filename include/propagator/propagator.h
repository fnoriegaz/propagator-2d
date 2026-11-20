#pragma once

#include "propagator/structs.h"

namespace Propagator{
void initialize(Models *models, Fields *fields, CPML *cpml, Source *srcs, Receiver *recvs, Config *config);

void save_gather(const char *filename, Receiver *receiver, Config *config);

void finalize(Models *models, Fields *fields, CPML *cpml, Source *srcs, Receiver *recvs, Config *config);

void propagate(Models *models, Fields *fields, CPML *cpml, Source *sources, Receiver *receivers, Config *config);
}
