#include "io.h"

void save_binary_generic(const char *filename, real_t *data, integer_t size){
	size_t err;
	FILE *fid = fopen(filename, "wb");
	if(!fid || fid == NULL){
		printf("Error while trying opening file %s\n",filename);
		exit(-1);
	}
	err = fwrite(data, sizeof(real_t), size, fid);
	fclose(fid);
	if(err != size){
		printf("Error! written only %d elements instead of expected %d\n",err,size);
		exit(-1);
	}
}

bool load_binary_generic(const char *filename, real_t *data, integer_t size){
	printf("Trying to read: %f Gib\n",(float)size*4/1000000000);
	size_t err;
	FILE *fid = fopen(filename, "rb");
	if(!fid || fid == NULL){
		printf("Error while reading the file %s\n",filename);
		return false;
	}
	err = fread(data, sizeof(real_t), size, fid);
	fclose(fid);
	if(err != size){
		printf("Error! read only %d elements instead of expected %d\n",err,size);
		exit(-1);
	}
	return true;
}

