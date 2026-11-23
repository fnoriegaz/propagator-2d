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
	init_config_default(config);

	config->n_receivers = config->cols - 2 * config->cpml_width;
	config->default_config = true;

	init_source_default(srcs, config);
	init_receiver_default(recvs, config);

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

	init_cpml(cpml, config);
	init_models(models, config);
	init_fields(fields, config);
	init_source_default(srcs, config);
	init_receiver_default(recvs, config);

	if(config->default_config){
		set_models_default(models, config);
	}
	else{
		set_models(models, config);
	}
}

void save_gather(const char *filename, Receiver *receiver, Config *config){
	save_binary_generic(filename, receiver->h_gather, config->timesamples*config->n_receivers);

}

void finalize(Models *models, Fields *fields, CPML *cpml, Source *srcs, Receiver *recvs, Config *config){
	
	std::cout <<"Finalizing...\n";
	clean_cpml(cpml);
	clean_models(models);
	clean_fields(fields);
	clean_source(srcs);
	clean_receiver(recvs);

	std::cout << "Finalize completed\n";
}
}

