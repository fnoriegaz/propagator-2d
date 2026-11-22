#pragma once

#include <math.h>

#include <cuda_runtime.h>

#include "propagator/structs.h"
#include "io.h"


#define CUDA_CHECK_CALL(call) do {							\
	cudaError_t e_ = (call);							\
	if(e_ != cudaSuccess){								\
		printf("%s:%d: %s\n", __FILE__, __LINE__,cudaGetErrorString(e_));	\
		exit(EXIT_FAILURE);							\
		}									\
}while (0);


void print_config(Config *config);

void init_config_default(Config *config, ...);

void init_models(Models *models, Config *config);

void init_fields(Fields *fields, Config *config);

void init_cpml(CPML *cpml, Config *config);

void init_source_default(Source *source, Config *config);

void init_receiver_default(Receiver *receiver, Config *config);

void clean_models(Models *models);

void clean_fields(Fields *fields);

void clean_cpml(CPML *cpml);

void clean_source(Source *source);

void clean_receiver(Receiver *receiver);

bool set_models(Models *models, Config *config);

