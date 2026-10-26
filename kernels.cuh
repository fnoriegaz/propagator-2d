#ifndef KERNELS_CUH
#define KERNELS_CUH

#include <cuda_runtime.h>

#include "structs.h"


//compute dp/dt using staggered grid with vel_x and vel_z half cell position.
__global__
void kernel_dp_dt(real_t *p, real_t *vel_x, real_t *vel_z, real_t *psi_vel_x, real_t *psi_vel_z, real_t *velocity, real_t *density,
		  real_t *ax, real_t *bx, real_t *az, real_t *bz, const integer_t rows, const integer_t cols,
		  const real_t dx, const real_t dz, const real_t dt);

__global__
void kernel_dvxz_dt(real_t *vel_x, real_t *vel_z, real_t *p, real_t *psi_x, real_t *psi_z, real_t *density,
		    real_t *ax, real_t *bx, real_t *az, real_t *bz, const integer_t rows, const integer_t cols,
		    const real_t dx, const real_t dz, const real_t dt);
__global__
void kernel_cpml(real_t *a, real_t *b, real_t freq, integer_t cpml_width, real_t r, real_t max_vel, integer_t axis_width, real_t dh, real_t dt);

__global__
void kernel_add_source(real_t *p, real_t src_val, integer_t axis_width, integer_t sx, integer_t sz);

__global__
void kernel_get_gather(real_t *p, real_t *gather, integer_t *recvs, integer_t n_receivers, integer_t axis_width);

#endif
