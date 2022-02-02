

#include "cl_structs.h"


#define EYE (float3)(0, 10, 0)
#define VEC (float3)(0,	0, 1)
#define DIST 2.0f


__kernel void generate_primary_rays(__global struct Ray* rays,  const int n, float3 E, float d, float3 V)
{


	int idx = get_global_id(0);
	int idy = get_global_id(1);


	int id = idx + idy * get_global_size(0);



	if (id >= n) return;


	float3 center = E + d * V;


	float3 P0 = center + (float3)(-1, 1, 0);
	float3 P1 = center + (float3)(1, 1, 0);
	float3 P2 = center + (float3)(-1, -1, 0);


	float u = (float)idx / get_global_size(0);
	float v = (float)idy / get_global_size(1);
	float3 O = P0 + u * (P1 - P0) + v * (P2 - P0);
	float3 D = O - E;


	// save ray stuff
	rays[id].D = normalize(D);
	rays[id].O = O;

	rays[id].id = id;


	//printf("Ray: %d\n", rays[id].id);

}



__kernel void cast_rays(__global struct Ray* rays, __global struct Primitive* prims, const int num_prims, __global float3* a)
{
	

	int id = get_global_id(0) + get_global_id(1) * get_global_size(0);
	


	float best_t = FLT_MAX;
	int best_prim = -1;

	struct Ray ray = rays[id];
	

	for (int i = 0; i < num_prims; i++){

		struct Primitive prim = prims[i];
		float denom = dot(prim.N, ray.D);

		if (denom != 0)
		{
			float num = -(dot(ray.O, prim.N) + prim.o);
			float t = num / denom;

			//printf("test %d\n", t)

			if  (t > 0 && t < best_t) {
				best_t = t;
				best_prim = i;

			}
	
		}
	}

	switch(best_prim){

		case(0):
			a[id] = (float3)(0,1,1);
			break;
		case(1):
			a[id] = (float3)(1,0,1);
			break;
		case(2):
			a[id] = (float3)(1,1,0);
			break;
		case(3):
			a[id] = (float3)(1,0,0);
			break;
		default:
			a[id] = (float3)(0,0,0);
			break;
		}
}




__kernel void anti_alias(__global float3* input, __global float3* output, int aa_res){
	

	// get the local x and y in aa_res step sizes
	int local_x = get_global_id(0) * aa_res;
	int local_y = get_global_id(1) * aa_res;
	

	float3 colour = (float3)(0,0,0);

	
	for( int i = 0; i < aa_res; i++){
		for( int j = 0; j < aa_res; j++){


			
			// get the sub pixels, screen width needs to be multiplied by aa res
			long local_id = (local_x + j) + ((local_y + i) * get_global_size(0) * aa_res);
			
			//if (id == 0) printf("test %d\n", local_id);

			colour += input[local_id];
		
		}
	}


	long id = get_global_id(0) + get_global_id(1) * get_global_size(0);

	output[id] = colour / 4;




}









// old stuff

__kernel void device_function(write_only image2d_t a)
{
	
	int idx = get_global_id(0);
	int idy = get_global_id(1);

	int2 dim = get_image_dim(a);

	//printf("%d %d\n", dim);

	int id = idx + dim.x * idy;

	if (id >= (dim.x * dim.y)) return;


	// generate ray
	float3 E = EYE;
	float3 center = E + (DIST * VEC);

	float3 P0 = center + (float3)(-1, 1, 0);
	float3 P1 = center + (float3)(1, 1, 0);
	float3 P2 = center + (float3)(-1, -1, 0);

	float u = (float)idx / dim.x;
	float v = (float)idy / dim.y;


	//printf("%f %f\n", u, v);


	float3 O = P0 + u * (P1 - P0) + v * (P2 - P0);
	float3 D = O - E;

	D = normalize(D);


	//printf("X:%f Y:%f Z:%f\n",D.x,D.y,D.z);

	//printf("O- X: % f Y : % f Z : % f\n",O.x,O.y,O.z);

	// sphere
	float t = 0;
	float r2 = 25; // radius of 5
	float3 pos = (float3)(0, 10, 20);
	float4 col = (float4)(1, 0, 0, 1);

	int2 pix = (int2)(idx, idy);

	// intersect circle

	float3 C = pos - O;

	//printf("X:%f Y:%f Z:%f\n", C.x, C.y, C.z);

	t = dot(C, D);



	float3 Q = C - t * D;
	float p2 = dot(Q, Q);



	//printf("%f %f\n", r2, p2);

	if (p2 > r2) {
		write_imagef(a, pix, 0.f);
		return; 
	}
	


	t -= sqrt(r2 - p2);

	//printf("test\n");

	// display pixel

	if (t <= 0) {
		write_imagef(a, pix, 0.f);
		return;
	}
		
	write_imagef(a, pix, col);
}




__kernel void test_kern( global struct Ray *rays ){

	int id = get_global_id(0);

	//a[id] = 10.0;



	struct Ray ray;

	ray.id = id;

	printf("ID: %d\n", ray.id);
}