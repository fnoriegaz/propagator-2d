#pragma once


#include <stdio.h>
#include <stdlib.h>

#include "defines.h"

void save_binary_generic(const char *filename, real_t *data, integer_t size);

bool load_binary_generic(const char *filename, real_t *data, integer_t size);
