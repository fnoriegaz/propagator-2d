/*
 * 2D implementation of the acoustic wave equation with variable density
 * Hopefully gets to a mature state...*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "propagator.h"
#include "io.h"
#include "helpers.hpp"


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

	set_models(models, config, "MODEL_P-WAVE_VELOCITY_1.25m.bin", "MODEL_DENSITY_1.25m.bin");
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

int main(int argc, char *argv[]){

	Config config = {};
	Models models = {};
	Fields fields = {};
	CPML cpml = {};
	Source srcs = {};
	Receiver recvs = {};

	std::cout << "initializing...\n";
	Propagator::initialize(&models, &fields, &cpml, &srcs, &recvs, &config);

	Propagator::propagate(&models, &fields, &cpml, &srcs, &recvs, &config);
	Propagator::save_gather("gather.bin", &recvs, &config);

	Propagator::finalize(&models, &fields, &cpml, &srcs, &recvs, &config);
	return 0;
}
