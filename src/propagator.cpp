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
bool load_config_default(Config *config){

	init_config_default(config);
	config->n_receivers = config->cols - 2 * config->cpml_width;
	config->default_config = true;
	config->default_source = true;
	config->default_source_pos = true;
	config->default_receiver = true;

	return true;
}

bool load_config(Config *config, const char *config_path){
	config->default_config = false;
	config->default_config = true;
	config->default_source = true;
	config->default_source_pos = true;
	config->default_receiver = true;

	FILE *fid = fopen(config_path, "r");
	if(!fid || fid == NULL){
		std::cout << "Could not open file for read. Provided path: " << config_path << "\n";
		std::cout << "Loading default configuration\n";
		return load_config_default(config);
	}
	char *lineptr = NULL;
	int read_bytes = 0;
	size_t size = 0;
	while((read_bytes = getline(&lineptr, &size, fid)) != -1){
		size_t lineptr_len = strlen(lineptr);
		char *lineptr_copy = (char *)malloc(lineptr_len + 1);
		memcpy(lineptr_copy, lineptr, lineptr_len+1);

		char *line_split = strtok(lineptr_copy, " ");
		if(!strcmp(line_split, "inline")){
			line_split = strtok(NULL, " ");
			config->cols = atoi(line_split);
		}
		else if(!strcmp(line_split, "depth")){
			line_split = strtok(NULL, " ");
			config->rows = atoi(line_split);
		}
		else if(!strcmp(line_split, "dx")){
			line_split = strtok(NULL, " ");
			config->dx = atof(line_split);
		}
		else if(!strcmp(line_split, "dz")){
			line_split = strtok(NULL, " ");
			config->dz = atof(line_split);
		}
		else if(!strcmp(line_split, "dt")){
			line_split = strtok(NULL, " ");
			config->dt = atof(line_split);
		}
		else if(!strcmp(line_split, "freq")){
			line_split = strtok(NULL, " ");
			config->freq = atof(line_split);
		}
		else if(!strcmp(line_split, "timesamples")){
			line_split = strtok(NULL, " ");
			config->timesamples = atoi(line_split);
		}
		else if(!strcmp(line_split, "cpml")){
			line_split = strtok(NULL, " ");
			config->cpml_width = atoi(line_split);
		}
		else if(!strcmp(line_split, "max_vel")){
			line_split = strtok(NULL, " ");
			config->max_vel = atoi(line_split);
		}
		else if(!strcmp(line_split, "n_sources")){
			line_split = strtok(NULL, " ");
			config->n_sources = atoi(line_split);
		}
		else if(!strcmp(line_split, "n_receivers")){
			line_split = strtok(NULL, " ");
			config->n_receivers = atoi(line_split);
		}
		else if(!strcmp(line_split, "velocity_file")){
			line_split = strtok(NULL, " \t\r\n");
			config->velocity_file = strdup(line_split);
		}
		else if(!strcmp(line_split, "density_file")){
			line_split = strtok(NULL, " \t\r\n");
			config->density_file = strdup(line_split);
		}
		else if(!strcmp(line_split, "source_file")){
			line_split = strtok(NULL, " \t\r\n");
			config->default_source = false;
			config->source_file = strdup(line_split);
		}
		else if(!strcmp(line_split, "srcs_positions_file")){
			line_split = strtok(NULL, " \t\r\n");
			config->default_source_pos = false;
			config->srcs_positions_file = strdup(line_split);
		}
		else if(!strcmp(line_split, "recvs_positions_file")){
			line_split = strtok(NULL, " \t\r\n");
			config->default_receiver = false;
			config->recvs_positions_file = strdup(line_split);
		}

		free(lineptr_copy);
	}
	fclose(fid);
	free(lineptr);
	print_config(config);
	return true;
}

void initialize(Models *models, Fields *fields, CPML *cpml, Source *srcs, Receiver *recvs, Config *config){

	init_cpml(cpml, config);
	init_models(models, config);
	init_fields(fields, config);
	init_source(srcs, config);
	init_receiver(recvs, config);

	if(config->default_config){
		set_models_default(models, config);
	}
	else{
		set_models(models, config);
	}
	if(config->default_source){
		set_source_default(srcs, config);
	}
	else{
		set_source_default(srcs, config);
	}
	if(config->default_receiver){
		set_receivers_default(recvs, config);
	}
	else{
		set_receivers_default(recvs, config);
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

