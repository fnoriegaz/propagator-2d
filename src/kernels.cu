#include "kernels.cuh"
#include "constants.h"


__constant__ real_t c1 =  1225.0f/1024.0f;
__constant__ real_t c2 = -245.0f/3072.0f;
__constant__ real_t c3 =  49.0f/5120.0f;
__constant__ real_t c4 = -5.0f/7168.0f;


//compute dp/dt using staggered grid with vel_x and vel_z half cell position.
//in discrete time, this will commpute pressure field p at timestep t=i before adding the source perturbationat the same timestep
__global__
void kernel_dp_dt(real_t *p, real_t *vel_x, real_t *vel_z, real_t *psi_vel_x, real_t *psi_vel_z, real_t *velocity, real_t *density,
		  real_t *ax, real_t *bx, real_t *az, real_t *bz, const integer_t rows, const integer_t cols,
		  const real_t dx, const real_t dz, const real_t dt){

	int tidx = threadIdx.x + blockIdx.x*blockDim.x;
	int tidz = threadIdx.y + blockIdx.y*blockDim.y; //migth get confusing but .y is faster indexing than .z in a grid of threads
							//but the naming convention implies the physical world axis, i.e. depth.
	int tid = tidx + tidz * cols;

	real_t r_dvx_dx = 0.0;
	real_t r_dvz_dz = 0.0;
	real_t r_psi_vel_x = 0.0;
	real_t r_psi_vel_z = 0.0;


	if(tidx > 3 && tidx < cols-3 && tidz > 3 && tidz < rows-3){
		r_dvx_dx = (c1 * (vel_x[tid] - vel_x[tid-1]) +
			 c2 * (vel_x[tid+1] - vel_x[tid-2]) +
			 c3 * (vel_x[tid+2] - vel_x[tid-3]) +
			 c4 * (vel_x[tid+3] - vel_x[tid-4])) / dx;

		r_dvz_dz = (c1 * (vel_z[tid] - vel_z[tid-1*cols]) +
			 c2 * (vel_z[tid+1*cols] - vel_z[tid-2*cols]) +
			 c3 * (vel_z[tid+2*cols] - vel_z[tid-3*cols]) +
			 c4 * (vel_z[tid+3*cols] - vel_z[tid-4*cols])) / dz;

		r_psi_vel_x = psi_vel_x[tid] * ax[tidx] + bx[tidx] * r_dvx_dx;
		r_psi_vel_z = psi_vel_z[tid] * az[tidz] + bz[tidz] * r_dvz_dz;

		p[tid] -= dt * velocity[tid] * velocity[tid] * density[tid] * (r_dvx_dx + r_dvz_dz + r_psi_vel_x + r_psi_vel_z);
		psi_vel_x[tid] = r_psi_vel_x;
		psi_vel_z[tid] = r_psi_vel_z;
	}

}

//compute first time derivative of Vx and Vz. it is assumed at timestep t=i+1/2 since pressure field gets calculated at
//timestep t=i
__global__
void kernel_dvxz_dt(real_t *vel_x, real_t *vel_z, real_t *p, real_t *psi_x, real_t *psi_z, real_t *density,
		    real_t *ax, real_t *bx, real_t *az, real_t *bz, const integer_t rows, const integer_t cols,
		    const real_t dx, const real_t dz, const real_t dt){


	int tidx = threadIdx.x + blockIdx.x*blockDim.x;
	int tidz = threadIdx.y + blockIdx.y*blockDim.y; //migth get confusing but .y is faster indexing than .z in a grid of threads
							//but the naming convention implies the physical world axis, i.e. depth.
	int tid = tidx + tidz * cols;

	real_t r_dp_dx = 0.0;
	real_t r_dp_dz = 0.0;
	real_t r_psi_x = 0.0;
	real_t r_psi_z = 0.0;

	if(tidx > 2 && tidx < cols-4 && tidz > 2 && tidz < rows-4){
		real_t r_density_inv = 2.0 / (density[tid+1] + density[tid]);
		r_dp_dx = (c1 * (p[tid+1] - p[tid]) +
			c2 * (p[tid+2] - p[tid-1]) +
			c3 * (p[tid+3] - p[tid-2]) +
			c4 * (p[tid+4] - p[tid-3])) / dx;

		r_dp_dz = (c1 * (p[tid+1*cols] - p[tid]) +
			c2 * (p[tid+2*cols] - p[tid-1*cols]) +
			c3 * (p[tid+3*cols] - p[tid-2*cols]) +
			c4 * (p[tid+4*cols] - p[tid-3*cols])) / dz;

		r_psi_x = psi_x[tid] * ax[tidx] + bx[tidx] * r_dp_dx;
		r_psi_z = psi_z[tid] * az[tidz] + bz[tidz] * r_dp_dz;

		vel_x[tid] -= dt * r_density_inv * (r_dp_dx + r_psi_x);
		vel_z[tid] -= dt * r_density_inv * (r_dp_dz + r_psi_z);
		psi_x[tid] = r_psi_x;
		psi_z[tid] = r_psi_z;
	}
}

//this kernel computes the absorbing layers. thus simulating virtual boundaries in the x and z axis.
__global__
void kernel_cpml(real_t *a, real_t *b, real_t freq, integer_t cpml_width, real_t r, real_t max_vel, integer_t axis_width, real_t dh, real_t dt){
	int tidx = threadIdx.x + blockIdx.x * blockDim.x;

	real_t lx = cpml_width;
	real_t d0 = -3 * log(r) / (2 * lx);
	real_t fx = 0.0;
	real_t dx = 0.0;
	real_t alpha = 0.0;

	if(tidx < cpml_width){
		fx = (cpml_width - tidx - 1) * dh;
	}
	if(tidx > axis_width-cpml_width-1 && tidx < axis_width){
		fx = (tidx - axis_width + cpml_width) * dh;
	}

	dx = d0 * max_vel * (fx / lx) * (fx / lx);
	alpha = PI * freq * (lx - fx) / lx;

	if(tidx < axis_width){
		a[tidx] = expf(-(dx+alpha)*dt);
		b[tidx] = (dx / (dx + alpha)) * (a[tidx] - 1.0);
	}
}

//assuming row major memory allocation
__global__
void kernel_add_source(real_t *p, real_t src_val, integer_t axis_width, integer_t sx, integer_t sz){
	int tid = sx + axis_width * sz;
	p[tid] += src_val;
}

//assuming row major memory allocation. therefore axis_width=cols
__global__
void kernel_get_gather(real_t *p, real_t *gather, integer_t *recvs, integer_t n_receivers, integer_t axis_width, integer_t offset){
	int tidx = threadIdx.x + blockIdx.x * blockDim.x;

	if(tidx < n_receivers){
		gather[offset + tidx] = p[recvs[tidx] + axis_width * recvs[tidx+n_receivers]];
	}
}
