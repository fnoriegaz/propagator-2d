#pragma once

#include <iostream>

#include "defines.h"

void save_binary_generic(const char *filename, real_t *data, integer_t size);

bool load_binary_generic(const char *filename, real_t *data, integer_t size);
