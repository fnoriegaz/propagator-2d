#include "propagator.cuh"
#include "kernels.cuh"

void propagate(Models *models, Fields *fields, CPML *cpml, Source *source, Receiver *receivers, Config config){
	cudaError_t err;

	dim3 tpb(TPBX,TPBY,1);
	dim3 bpg((config.cols+TPBX-1)/TPBX, (config.rows+TPBY-1)/TPBY,1);
	
	//thread grid configurations for cpml kernel launches
	dim3 bpgx((config.cols+TPBX-1)/TPBX,1,1);
	dim3 bpgz((config.rows+TPBX-1)/TPBX,1,1);

	kernel_cpml<<<bpgx,TPBX>>>(cpml->ax, cpml->bx, config.freq, config.cpml_width, config.r, config.max_vel, config.cols, config.dx, config.dt);
	kernel_cpml<<<bpgz,TPBX>>>(cpml->az, cpml->bz, config.freq, config.cpml_width, config.r, config.max_vel, config.rows, config.dx, config.dt);

	for(integer_t t=0;t<config.timesamples;t++){

		kernel_dvxz_dt<<<bpg,tpb>>>(fields->vel_x, fields->vel_z, fields->p, fields->psi_x, fields->psi_z, models->d_density,
		cpml->ax, cpml->bx, cpml->az, cpml->bz, config.rows, config.cols, config.dx, config.dz, config.dt);

		kernel_dp_dt<<<bpg,tpb>>>(fields->p, fields->vel_x, fields->vel_z, fields->psi_vel_x, fields->psi_vel_z, models->d_velocity, models->d_density,
	       	cpml->ax, cpml->bx, cpml->az, cpml->bz, config.rows, config.cols, config.dx, config.dz, config.dt);

		kernel_add_source<<<1,1>>>(fields->p, source->wavelet[t], config.cols, source->pos[0], source->pos[1]);

		kernel_get_gather<<<bpgx,TPBX>>>(fields->p, receivers->d_gather, receivers->d_pos, config.n_receivers, config.cols, t*config.n_receivers);

		err = cudaGetLastError();
		if(err != cudaSuccess){
			printf("at iteration %d error %d %s\n",t,err,cudaGetErrorString(err));

		}
	}

	cudaMemcpy(receivers->h_gather, receivers->d_gather, config.n_receivers*config.timesamples*sizeof(real_t), cudaMemcpyDeviceToHost);
}
