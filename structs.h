#pragma once

#include <iostream>

#include "defines.h"


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

	void (* init)(Config *config, ...);
}Config;

typedef struct Models{
	real_t *h_velocity;
	real_t *h_density;
	real_t *d_velocity;
	real_t *d_density;

	void (* clean)(Models *models);
	void (* init)(Models *models, Config *config);
}Models;

typedef struct Fields{
	real_t *p;
	real_t *vel_x;
	real_t *vel_z;
	real_t *psi_x;
	real_t *psi_z;
	real_t *psi_vel_x;
	real_t *psi_vel_z;

	void (* clean)(Fields *fields);
	void (* init)(Fields *fields, Config *config);
}Fields;

typedef struct CPML{
	real_t *ax;
	real_t *bx;
	real_t *az;
	real_t *bz;

	void (* clean)(CPML *cpml);
	void (* init)(CPML *cpml, Config *config);
}CPML;

//contains wavelet/perturbation information and its geometry
typedef struct Source{
	integer_t *pos; //source positions as x and z coordinates
	real_t *wavelet; //e.g. wavelet ricker

	void (* init)(Source *source, Config *config);
	void (* clean)(Source *source);
}Source;

//contains adquisition information/captured gather and the geometry
typedef struct Receiver{
	integer_t *h_pos; //the x and z coordinates of receivers
	real_t *h_gather;
	
	integer_t *d_pos; //the x and z coordinates of receivers
	real_t *d_gather;

	void (* init)(Receiver *receiver, Config *config);
	void (* clean)(Receiver *receiver);
}Receiver;

