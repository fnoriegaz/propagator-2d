/*
 * 2D implementation of the acoustic wave equation with variable density
 * Hopefully gets to a mature state...*/

#include <argp.h>
#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "helpers.hpp"
#include "io.h"
#include "propagator/propagator.h"


namespace Propagator{
bool load_config_default(Config *config, Source *srcs, Receiver *recvs){
	config->init = init_config_default;
	config->init(config);

	config->n_receivers = config->cols - 2 * config->cpml_width;
	config->default_config = true;

	srcs->init = init_source_default;
	recvs->init = init_receiver_default;

	return true;
}

bool load_config(Config *config, const char *config_path){
	FILE *fid = fopen(config_path, "r");
	if(fid == NULL || fid == nullptr){
		std::cout << "Could not open file for read: " << config_path << "\n";
		return false;
	}
	char *lineptr = NULL;
	int read_bytes = 0;
	size_t size = 0;
	while((read_bytes = getline(&lineptr, &size, fid)) != -1){
		std::cout << "Read: " << lineptr << "\n";
	}

	return true;
}

void initialize(Models *models, Fields *fields, CPML *cpml, Source *srcs, Receiver *recvs, Config *config){


	cpml->init = init_cpml;
	models->init = init_models;
	fields->init = init_fields;

	cpml->init(cpml, config);
	models->init(models, config);
	fields->init(fields, config);
	srcs->init(srcs, config);
	recvs->init(recvs, config);

	set_models(models, config);
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

