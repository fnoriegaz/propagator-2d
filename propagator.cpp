/*
 * 2D implementation of the acoustic wave equation with variable density
 * Hopefully gets to a mature state...*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

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
	config->rows = 2801;
	config->cols = 13601;
	config->timesamples = 50000;
	config->dt = 1e-4;
	config->dx = 1.25;
	config->dz = 1.25;

	config->freq = 9.0;
	config->r = 1e-4;
	config->cpml_width = 20;
	config->max_vel = 5000;

	config->n_sources = 1;
	config->n_receivers = config->cols - 2 * config->cpml_width;
}

void init_models(Models *models, const Config config){
	print_config(config);
	cudaError_t error;

	models->h_velocity = (real_t *)calloc(config.cols*config.rows, sizeof(real_t));
	models->h_density = (real_t *)calloc(config.cols*config.rows, sizeof(real_t));

	cudaMalloc((void **)&models->d_velocity, config.rows*config.cols*sizeof(real_t));
	cudaMalloc((void **)&models->d_density, config.rows*config.cols*sizeof(real_t));

	cudaMemset(models->d_velocity, 0x00, config.rows*config.cols*sizeof(real_t));
	cudaMemset(models->d_density, 0x00, config.rows*config.cols*sizeof(real_t));
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

	source->pos[0] = config.cols / 2;
	source->pos[1] = config.rows / 2;

	real_t t_shifted = 2.0 / config.freq;
	for(int t=0;t<config.timesamples;t++){
		real_t t_current = t * config.dt - 1;
		real_t alpha = PI * PI * config.freq * config.freq * t_current* t_current;
		source->wavelet[t] = (1 - 2 * alpha) * exp(-alpha);
	}

}


void init_receiver_default(Receiver *receiver, const Config config){

	receiver->h_pos = (integer_t *)calloc(2*config.n_receivers, sizeof(integer_t));
	receiver->h_gather = (real_t *)calloc(config.timesamples*config.n_receivers, sizeof(real_t));

	cudaMalloc((void **)&receiver->d_pos, 2*config.n_receivers*sizeof(integer_t));
	cudaMalloc((void **)&receiver->d_gather, config.timesamples*config.n_receivers*sizeof(real_t));

	for(int x=0;x<config.n_receivers;x++){
		receiver->h_pos[x] = x + config.cpml_width;
		receiver->h_pos[x+config.n_receivers] = config.rows/2;
	}

	cudaMemcpy(receiver->d_pos,receiver->h_pos,2*config.n_receivers*sizeof(integer_t),cudaMemcpyHostToDevice);
}

void clean_models(Models *models){
	free(models->h_velocity);
	free(models->h_density);
	cudaFree(models->d_velocity);
	cudaFree(models->d_density);
}

void clean_fields(Fields *fields){
	cudaFree(fields->p);
	cudaFree(fields->vel_x);
	cudaFree(fields->vel_z);
	cudaFree(fields->psi_x);
	cudaFree(fields->psi_z);
	cudaFree(fields->psi_vel_x);
	cudaFree(fields->psi_vel_z);
}

void clean_cpml(CPML *cpml){
	cudaFree(cpml->ax);
	cudaFree(cpml->bx);
	cudaFree(cpml->az);
	cudaFree(cpml->bz);
}

void clean_source(Source *source){
	free(source->wavelet);
	free(source->pos);
}

void clean_receiver(Receiver *receiver){
	free(receiver->h_pos);
	free(receiver->h_gather);

	cudaFree(receiver->d_pos);
	cudaFree(receiver->d_gather);
}

void save_binary_generic(const char *filename, real_t *data, integer_t size){
	size_t err;
	FILE *fid = fopen(filename, "wb");
	if(!fid || fid == NULL){
		printf("Error while trying opening file %s\n",filename);
		exit(-1);
	}
	err = fwrite(data, size, sizeof(real_t), fid);
	fclose(fid);
}

bool load_binary_generic(const char *filename, real_t *data, integer_t size){
	size_t err;
	FILE *fid = fopen(filename, "rb");
	if(!fid || fid == NULL){
		printf("Error while reading the file %s\n",filename);
		return false;
	}
	err = fread(data, size, sizeof(real_t), fid);
	fclose(fid);
	return true;
}

void set_models(Models *models, const Config config, const char *velocity_filename, const char *density_filename){

	bool loaded_vel = load_binary_generic(velocity_filename, models->h_velocity, config.cols*config.rows);
	bool loaded_den = load_binary_generic(density_filename, models->h_density, config.cols*config.rows);

	if(!loaded_vel && !loaded_den){
		for(integer_t c=0;c<config.rows*config.cols;c++){
			models->h_velocity[c] = 1500.0;
			models->h_density[c] = 1000.0;
		}
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

	cpml.clean = clean_cpml;
	models.clean = clean_models;
	fields.clean = clean_fields;
	srcs.clean = clean_source;
	recvs.clean = clean_receiver;

	config.init(&config);
	cpml.init(&cpml, config);
	models.init(&models, config);
	fields.init(&fields, config);
	srcs.init(&srcs, config);
	recvs.init(&recvs, config);

	set_models(&models, config, "MODEL_P-WAVE_VELOCITY_1.25m.bin", "MODEL_DENSITY_1.25m.bin");
	propagate(&models, &fields, &cpml, &srcs, &recvs, config);
	save_binary_generic("gather.bin", recvs.h_gather, config.timesamples*config.n_receivers);

	cpml.clean(&cpml);
	models.clean(&models);
	fields.clean(&fields);
	srcs.clean(&srcs);
	recvs.clean(&recvs);

	return 0;
}
