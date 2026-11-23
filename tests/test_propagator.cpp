/*
 * 2D implementation of the acoustic wave equation with variable density
 * Hopefully gets to a mature state...*/

#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "../include/propagator/propagator.h"



int main(int argc, char *argv[]){

	Config config = {};
	Models models = {};
	Fields fields = {};
	CPML cpml = {};
	Source srcs = {};
	Receiver recvs = {};

	std::cout << "initializing...\n";
	Propagator::load_config_default(&config, &srcs, &recvs);
	Propagator::initialize(&models, &fields, &cpml, &srcs, &recvs, &config);
	Propagator::propagate(&models, &fields, &cpml, &srcs, &recvs, &config);
	Propagator::save_gather("/home/fabian/Documents/git/propagator-2d/gather.bin", &recvs, &config);
	Propagator::finalize(&models, &fields, &cpml, &srcs, &recvs, &config);
	/*
	*/
	return 0;

}
