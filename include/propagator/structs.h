#pragma once

#include <iostream>

#include "propagator/types.h"


typedef struct Config{
	integer_t cols;
	integer_t rows;
	integer_t timesamples;
	real_t dx;
	real_t dz;
	real_t dt;

	//cpml necessary configuration
	integer_t cpml_width;
	real_t r;
	real_t max_vel;
	real_t freq;

	//source and receivers geometry
	integer_t n_sources;
	integer_t n_receivers;

	bool default_config;
	bool default_source;
	bool default_source_pos;
	bool default_receiver;
 	
	char *velocity_file;
	char *density_file;
	char *source_file;
	char *srcs_positions_file;
	char *recvs_positions_file;
}Config;

typedef struct Models{
	real_t *h_velocity;
	real_t *h_density;
	real_t *d_velocity;
	real_t *d_density;
}Models;

typedef struct Fields{
	real_t *p;
	real_t *vel_x;
	real_t *vel_z;
	real_t *psi_x;
	real_t *psi_z;
	real_t *psi_vel_x;
	real_t *psi_vel_z;
}Fields;

typedef struct CPML{
	real_t *ax;
	real_t *bx;
	real_t *az;
	real_t *bz;
}CPML;

//contains wavelet/perturbation information and its geometry
typedef struct Source{
	integer_t *pos; //source positions as x and z coordinates
	real_t *wavelet; //e.g. wavelet ricker
}Source;

//contains adquisition information/captured gather and the geometry
typedef struct Receiver{
	integer_t *h_pos; //the x and z coordinates of receivers
	real_t *h_gather;
	
	integer_t *d_pos; //the x and z coordinates of receivers
	real_t *d_gather;
}Receiver;

