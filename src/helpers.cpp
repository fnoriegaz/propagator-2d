#include "helpers.hpp"
#include "constants.h"


void print_config(Config *config){
	std::cout << "\ncols: " << config->cols \
	<< ", rows: " << config->rows \
	<< ", dx: " << config->dx \
	<< ", dz: " << config->dz\
	<< ", dt: " << config->dt \
	<< ", timesamples: " << config->timesamples \
	<< ", freq: " << config->freq \
	<< ", r: " << config->r \
	<< ", cpml_width: " << config->cpml_width \
	<< ", max_vel: " << config->max_vel \
	<< ", n_sources: " << config->n_sources \
	<< ", n_receivers: " << config->n_receivers \
	<< ", velocity path: " << config->velocity_filename \
	<< ", density path: " << config->density_filename << std::endl;
}

void init_config_default(Config *config, ...){
	config->rows = 256;
	config->cols = 256;
	config->timesamples = 3000;
	config->dt = 1e-3;
	config->dx = 12.5;
	config->dz = 12.5;

	config->freq = 3.0;
	config->r = 1e-4;
	config->cpml_width = 20;
	config->max_vel = 3000;

	config->n_sources = 1;

	config->velocity_filename = (char *)"default";
	config->density_filename = (char *)"default";
}

void init_models(Models *models, Config *config){

	models->h_velocity = (real_t *)calloc(config->cols*config->rows, sizeof(real_t));
	models->h_density = (real_t *)calloc(config->cols*config->rows, sizeof(real_t));

	cudaMalloc((void **)&models->d_velocity, config->rows*config->cols*sizeof(real_t));
	cudaMalloc((void **)&models->d_density, config->rows*config->cols*sizeof(real_t));

	cudaMemset(models->d_velocity, 0x00, config->rows*config->cols*sizeof(real_t));
	cudaMemset(models->d_density, 0x00, config->rows*config->cols*sizeof(real_t));
}

void init_fields(Fields *fields, Config *config){
	cudaMalloc((void **)&fields->p, config->rows*config->cols*sizeof(real_t));
	cudaMalloc((void **)&fields->vel_x, config->rows*config->cols*sizeof(real_t));
	cudaMalloc((void **)&fields->vel_z, config->rows*config->cols*sizeof(real_t));
	cudaMalloc((void **)&fields->psi_x, config->rows*config->cols*sizeof(real_t));
	cudaMalloc((void **)&fields->psi_z, config->rows*config->cols*sizeof(real_t));
	cudaMalloc((void **)&fields->psi_vel_x, config->rows*config->cols*sizeof(real_t));
	cudaMalloc((void **)&fields->psi_vel_z, config->rows*config->cols*sizeof(real_t));

	
	cudaMemset(fields->p, 0x00, config->rows*config->cols*sizeof(real_t));
	cudaMemset(fields->vel_x, 0x00, config->rows*config->cols*sizeof(real_t));
	cudaMemset(fields->vel_z, 0x00, config->rows*config->cols*sizeof(real_t));
	cudaMemset(fields->psi_x, 0x00, config->rows*config->cols*sizeof(real_t));
	cudaMemset(fields->psi_z, 0x00, config->rows*config->cols*sizeof(real_t));
	cudaMemset(fields->psi_vel_x, 0x00, config->rows*config->cols*sizeof(real_t));
	cudaMemset(fields->psi_vel_z, 0x00, config->rows*config->cols*sizeof(real_t));
}

void init_cpml(CPML *cpml, Config *config){
	cudaMalloc((void **)&cpml->ax, config->cols*sizeof(real_t));
	cudaMalloc((void **)&cpml->bx, config->cols*sizeof(real_t));
	cudaMalloc((void **)&cpml->az, config->rows*sizeof(real_t));
	cudaMalloc((void **)&cpml->bz, config->rows*sizeof(real_t));

	cudaMemset(cpml->ax, 0x00, config->cols*sizeof(real_t));
	cudaMemset(cpml->bx, 0x00, config->cols*sizeof(real_t));
	cudaMemset(cpml->az, 0x00, config->rows*sizeof(real_t));
	cudaMemset(cpml->bz, 0x00, config->rows*sizeof(real_t));
}

void init_source_default(Source *source, Config *config){
	source->pos = (integer_t *)calloc(2*config->n_sources, sizeof(integer_t));
	source->wavelet = (real_t *)calloc(config->timesamples, sizeof(real_t));

	source->pos[0] = config->cols / 2;
	source->pos[1] = config->rows / 2;

	real_t t_shifted = 2.0 / config->freq;
	for(int t=0;t<config->timesamples;t++){
		real_t t_current = t * config->dt - 1;
		real_t alpha = PI * PI * config->freq * config->freq * t_current* t_current;
		source->wavelet[t] = (1 - 2 * alpha) * exp(-alpha);
		std::cout << source->wavelet[t] << std::endl;
	}

}


void init_receiver_default(Receiver *receiver, Config *config){

	receiver->h_pos = (integer_t *)calloc(2*config->n_receivers, sizeof(integer_t));
	receiver->h_gather = (real_t *)calloc(config->timesamples*config->n_receivers, sizeof(real_t));

	cudaMalloc((void **)&receiver->d_pos, 2*config->n_receivers*sizeof(integer_t));
	cudaMalloc((void **)&receiver->d_gather, config->timesamples*config->n_receivers*sizeof(real_t));

	for(int x=0;x<config->n_receivers;x++){
		receiver->h_pos[x] = x + config->cpml_width;
		receiver->h_pos[x+config->n_receivers] = config->rows/2;
	}

	cudaMemcpy(receiver->d_pos,receiver->h_pos,2*config->n_receivers*sizeof(integer_t),cudaMemcpyHostToDevice);
}

void clean_models(Models *models){
	std::cout << "Cleaning models\n";

	free(models->h_velocity);
	free(models->h_density);
	cudaFree(models->d_velocity);
	cudaFree(models->d_density);
}

void clean_fields(Fields *fields){
	std::cout << "Cleaning fields\n";

	cudaFree(fields->p);
	cudaFree(fields->vel_x);
	cudaFree(fields->vel_z);
	cudaFree(fields->psi_x);
	cudaFree(fields->psi_z);
	cudaFree(fields->psi_vel_x);
	cudaFree(fields->psi_vel_z);
}

void clean_cpml(CPML *cpml){
	std::cout << "Cleaning cpml\n";

	cudaFree(cpml->ax);
	cudaFree(cpml->bx);
	cudaFree(cpml->az);
	cudaFree(cpml->bz);
}

void clean_source(Source *source){
	std::cout << "Cleaning source\n";

	free(source->wavelet);
	free(source->pos);
}

void clean_receiver(Receiver *receiver){
	std::cout << "Cleaning receiver\n";

	free(receiver->h_pos);
	free(receiver->h_gather);

	cudaFree(receiver->d_pos);
	cudaFree(receiver->d_gather);
}

void set_models_default(Models *models, Config *config){
	//fallback to default water model.
	for(integer_t c=0;c<config->rows*config->cols;c++){
		models->h_velocity[c] = 1500.0;
		models->h_density[c] = 1000.0;
	}

	cudaMemcpy(models->d_velocity, models->h_velocity, config->rows*config->cols*sizeof(real_t), cudaMemcpyHostToDevice);
	cudaMemcpy(models->d_density, models->h_density, config->rows*config->cols*sizeof(real_t), cudaMemcpyHostToDevice);

	std::cout << "Models set to default water...\n";
}

bool set_models(Models *models, Config *config){

	print_config(config);
	bool loaded_vel = load_binary_generic((const char *)config->velocity_filename, models->h_velocity, config->cols*config->rows);
	bool loaded_den = load_binary_generic((const char *)config->density_filename, models->h_density, config->cols*config->rows);

	if(!loaded_vel){
		std::cout << "Could not load the velocity model " << config->velocity_filename << "\n";
		return false;
	}
	else if(!loaded_den){
		std::cout << "Could not load the density model " << config->velocity_filename << "\n";
		return false;
	}

	cudaMemcpy(models->d_velocity, models->h_velocity, config->rows*config->cols*sizeof(real_t), cudaMemcpyHostToDevice);
	cudaMemcpy(models->d_density, models->h_density, config->rows*config->cols*sizeof(real_t), cudaMemcpyHostToDevice);

	std::cout << "Models set correctly\n";
	return true;
}
