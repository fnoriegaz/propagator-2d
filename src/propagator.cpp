/*
 * 2D implementation of the acoustic wave equation with variable density
 * Hopefully gets to a mature state...*/

#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "helpers.hpp"
#include "io.h"
#include "propagator/propagator.h"


namespace Propagator{
void initialize(Models *models, Fields *fields, CPML *cpml, Source *srcs, Receiver *recvs, Config *config){

	config->init = init_config_default;
	cpml->init = init_cpml;
	models->init = init_models;
	fields->init = init_fields;
	srcs->init = init_source_default;
	recvs->init = init_receiver_default;

	config->init(config);
	cpml->init(cpml, config);
	models->init(models, config);
	fields->init(fields, config);
	srcs->init(srcs, config);
	recvs->init(recvs, config);

	set_models(models, config, "/home/fabian/Documents/git/propagator-2d/MODEL_P-WAVE_VELOCITY_1.25m.bin", "/home/fabian/Documents/git/propagator-2d/MODEL_DENSITY_1.25m.bin");
}

void save_gather(const char *filename, Receiver *receiver, Config *config){
	save_binary_generic(filename, receiver->h_gather, config->timesamples*config->n_receivers);

}

void finalize(Models *models, Fields *fields, CPML *cpml, Source *srcs, Receiver *recvs, Config *config){
	
	std::cout <<"Finalizing...\n";
	cpml->clean = clean_cpml;
	models->clean = clean_models;
	fields->clean = clean_fields;
	srcs->clean = clean_source;
	recvs->clean = clean_receiver;

	cpml->clean(cpml);
	fields->clean(fields);
	models->clean(models);
	srcs->clean(srcs);
	recvs->clean(recvs);

	std::cout << "Finalize completed\n";
}
}

