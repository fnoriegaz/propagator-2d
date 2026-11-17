#include "io.h"

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

