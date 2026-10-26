/*
 * 2D implementation of the acoustic wave equation with variable density
 * Hopefully gets to a mature state...*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "propagator.cuh"


void print_config(Config config){
	config.init(&config);
	std::cout << "\ncols: " << config.cols \
	<< " rows: " << config.rows \
	<< " dx: " << config.dx \
	<< " dz: " << config.dz\
	<< " dt: " << config.dt \
	<< " timesamples: " << config.timesamples << std::endl;
}

void init_config_default(Config *config, ...){
	config->rows = 1024;
	config->cols = 1024;
	config->timesamples = 3000;
	config->dt = 1e-3;
	config->dx =  25.0;
	config->dz =  25.0;

	config->freq = 3.0;
	config->r = 1e-4;
	config->cpml_width = 20;
	config->max_vel = 3000;
}

void init_models(Models *models, const Config config){
	print_config(config);
	cudaError_t error;

	models->h_velocity = (real_t *)calloc(config.cols*config.rows, sizeof(real_t));
	models->h_density = (real_t *)calloc(config.cols*config.rows, sizeof(real_t));

	cudaMalloc((void **)&models->d_velocity, config.rows*config.cols*sizeof(real_t));
	cudaMalloc((void **)&models->d_density, config.rows*config.cols*sizeof(real_t));
}

void init_fields(Fields *fields, const Config config){
	cudaMalloc((void **)&fields->p, config.rows*config.cols*sizeof(real_t));
	cudaMalloc((void **)&fields->vel_x, config.rows*config.cols*sizeof(real_t));
	cudaMalloc((void **)&fields->vel_z, config.rows*config.cols*sizeof(real_t));
	cudaMalloc((void **)&fields->psi_x, config.rows*config.cols*sizeof(real_t));
	cudaMalloc((void **)&fields->psi_z, config.rows*config.cols*sizeof(real_t));
	cudaMalloc((void **)&fields->psi_vel_x, config.rows*config.cols*sizeof(real_t));
	cudaMalloc((void **)&fields->psi_vel_z, config.rows*config.cols*sizeof(real_t));

	
	cudaMemset(fields->p, 0x00, config.rows*config.cols*sizeof(real_t));
	cudaMemset(fields->vel_x, 0x00, config.rows*config.cols*sizeof(real_t));
	cudaMemset(fields->vel_z, 0x00, config.rows*config.cols*sizeof(real_t));
	cudaMemset(fields->psi_x, 0x00, config.rows*config.cols*sizeof(real_t));
	cudaMemset(fields->psi_z, 0x00, config.rows*config.cols*sizeof(real_t));
	cudaMemset(fields->psi_vel_x, 0x00, config.rows*config.cols*sizeof(real_t));
	cudaMemset(fields->psi_vel_z, 0x00, config.rows*config.cols*sizeof(real_t));
}

void init_cpml(CPML *cpml, const Config config){
	cudaMalloc((void **)&cpml->ax, config.cols*sizeof(real_t));
	cudaMalloc((void **)&cpml->bx, config.cols*sizeof(real_t));
	cudaMalloc((void **)&cpml->az, config.rows*sizeof(real_t));
	cudaMalloc((void **)&cpml->bz, config.rows*sizeof(real_t));

	cudaMemset(cpml->ax, 0x00, config.cols*sizeof(real_t));
	cudaMemset(cpml->bx, 0x00, config.cols*sizeof(real_t));
	cudaMemset(cpml->az, 0x00, config.rows*sizeof(real_t));
	cudaMemset(cpml->bz, 0x00, config.rows*sizeof(real_t));
}

void init_source_default(Source *source, const Config config){
	source->pos = (integer_t *)calloc(2*config.n_sources, sizeof(integer_t));
	source->wavelet = (real_t *)calloc(config.timesamples, sizeof(real_t));
}


void init_receiver_default(Receiver *receiver, const Config config){
	integer_t n_recvs = config.cols - 2 * config.cpml_width; //2 because of 2D geometry. even if depth is the same.

	receiver->h_pos = (integer_t *)calloc(2*n_recvs, sizeof(integer_t));
	receiver->h_gather = (real_t *)calloc(config.n_sources*2*n_recvs, sizeof(real_t));

	cudaMalloc((void **)&receiver->d_pos, 2*n_recvs*sizeof(integer_t));
	cudaMalloc((void **)&receiver->d_gather, config.n_sources*n_recvs*sizeof(real_t));
}

void clean_models(Models models){
	free(models.h_velocity);
	free(models.h_density);
	cudaFree(models.d_velocity);
	cudaFree(models.d_density);
}

void clean_fields(Fields fields){
	cudaFree(fields.p);
	cudaFree(fields.vel_x);
	cudaFree(fields.vel_z);
	cudaFree(fields.psi_x);
	cudaFree(fields.psi_z);
	cudaFree(fields.psi_vel_x);
	cudaFree(fields.psi_vel_z);
}

void clean_cpml(CPML cpml){
	cudaFree(cpml.ax);
	cudaFree(cpml.bx);
	cudaFree(cpml.az);
	cudaFree(cpml.bz);
}

void clean_source(Source *source){
}

void clean_receiver(Receiver *receiver){
	free(receiver->h_pos);
	free(receiver->h_gather);

	cudaFree(receiver->d_pos);
	cudaFree(receiver->d_gather);
}

void set_models(Models *models, const Config config){

	for(integer_t c=0;c<config.rows*config.cols;c++){
		models->h_velocity[c] = 1500.0;
		models->h_density[c] = 1000.0;
	}

	cudaMemcpy(models->d_velocity, models->h_velocity, config.rows*config.cols*sizeof(real_t), cudaMemcpyHostToDevice);
	cudaMemcpy(models->d_density, models->h_density, config.rows*config.cols*sizeof(real_t), cudaMemcpyHostToDevice);
}

int main(int argc, char *argv[]){

	Config config;
	Models models;
	Fields fields;
	CPML cpml;
	Source srcs;
	Receiver recvs;

	config.init = init_config_default;
	cpml.init = init_cpml;
	models.init = init_models;
	fields.init = init_fields;
	srcs.init = init_source_default;
	recvs.init = init_receiver_default;

	config.init(&config);
	cpml.init(&cpml, config);
	models.init(&models, config);
	fields.init(&fields, config);

	propagate(&models, &fields, &cpml, config);

	return 0;
}
