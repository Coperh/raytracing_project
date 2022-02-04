

#include "cl_structs.h"


#define EYE (float3)(0, 0, 0)
#define VEC (float3)(0,	0, 1)
#define DIST 2.0f



#define EPISLON 1e-4f



__kernel void generate_primary_rays(
	__global struct Ray* rays, 
	const int n,
	const float d,
	const float3 w_E,
	__global float * rot4)
{


	int idx = get_global_id(0);
	int idy = get_global_id(1);
	int id = idx + idy * get_global_size(0);


	if (id >= n) return;


	float3 E = EYE, V = VEC;



	float3 center = E + d * V;
	float3 P0 = center + (float3)(-1, 1, 0);
	float3 P1 = center + (float3)(1, 1, 0);
	float3 P2 = center + (float3)(-1, -1, 0);


	float u = (float)idx / get_global_size(0);
	float v = (float)idy / get_global_size(1);
	float3 O = P0 + u * (P1 - P0) + v * (P2 - P0);
	float3 D = normalize(O - E);





	//if (id == 0) printf("1: %f %f %f\n", D.x, D.y, D.z);

	// rotate vector
	D = (float3)(
		rot4[0] * D.x + rot4[1] * D.y + rot4[2] * D.z,
		rot4[4] * D.x + rot4[5] * D.y + rot4[6] * D.z,
		rot4[8] * D.x + rot4[9] * D.y + rot4[10] * D.z
		);



	//if (id == 0) printf("2: %f %f %f\n", D.x, D.y, D.z);


	// save ray stuff
	rays[id].D = D;

	rays[id].O = O + w_E;

	rays[id].id = id;


	//printf("Ray: %d\n", rays[id].id);

}



__kernel void cast_rays(
	__global struct Primitive* prims,
	const int num_prims,
	__global struct Ray* rays, 
	const int num_rays,
	__global struct Intersection* intersctions,
	volatile __global int* incs
	)
{
	

	int id = get_global_id(0) + get_global_id(1) * get_global_size(0);
	

	if(id == 1) *incs = 0;


	float best_t = FLT_MAX;
	int best_prim = -1;

	struct Ray ray = rays[id];
	

	// combine into function
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




	if (best_prim >= 0) {
		int index = atomic_inc(incs);

		//Intersection
		intersctions[index].t = best_t;

		intersctions[index].id = ray.id;


		struct Primitive prim = prims[best_prim];

		intersctions[index].mat = prim.mat;

		if (dot(prim.N, ray.D) > 0)
			intersctions[index].N = -prim.N;
		else
			intersctions[index].N = prim.N;

		intersctions[index].D = ray.D;
		intersctions[index].I = ray.O + (ray.D * best_t);

	}
	

	//if (best_prim == 1) printf("test %f %d\n", best_t,id);
}


__kernel void shade_intersections(
	__global float3* ac, 
	__global struct Material* mats,
	__global struct Intersection* intersctions,
	__global struct Ray* shade_rays,
	__global struct Ray* ext_rays,
	volatile __global int* types
)
{

	

	int id = get_global_id(0) + get_global_id(1) * get_global_size(0);

	if (id == 0) {
		types[0] = 0;
		types[1] = 0;
		types[2] = 0;
		types[3] = 0;
	}


	struct Intersection intersection = intersctions[id];


	struct Material mat = mats[intersection.mat];
	ac[intersection.id] = mat.colour;

	
	
	
	int index = atomic_inc(&types[mat.type]);


	//if (index > 400000) printf("%d\n", types[mat.type]);


	float3 D = shade_rays[id].D;
	float3 N = intersection.N;

	int t;



	//struct Ray* output;


	if (mat.type == 0) {
		shade_rays[index].D = N;
		shade_rays[index].id = intersection.id;
		shade_rays[index].O = intersection.I;
	}
	else{
		ext_rays[index].D = D - 2 * (dot(D, N) * N);;
		ext_rays[index].id = intersection.id;
		ext_rays[index].O = intersection.I;
		
	}

	/*
	output[index].id = intersection.id;
	output[index].O = intersection.I;
	output[index].D = D;
	*/



}



__kernel void direct_illumination(
	__global float3* ac,
	__global struct PointLight* lights,
	const int num_lights,
	__global struct Primitive* prims,
	const int num_prims,
	__global struct Ray* rays,
	const int num_rays
)
{



	int id = get_global_id(0) + get_global_id(1) * get_global_size(0);


	if (id > num_rays - 1) return;



	struct Ray ray  = rays[id];


	float3 orig_o = ray.O;
	// ray direction used for normal
	float3 normal = ray.D;

	float light_contribution = 0;


	for (int l = 0; l < num_lights; l++)
	{
		ray.D = normalize(lights[l].pos - orig_o);
		ray.O = orig_o + (ray.D * EPISLON);
		float dist = distance(ray.O, lights[l].pos);

		bool blocked = false;

		for (int i = 0; i < num_prims; i++){

			struct Primitive prim = prims[i];
			float denom = dot(prim.N, ray.D);

			if (denom != 0)
			{
				float num = -(dot(ray.O, prim.N) + prim.o);
				float t = num / denom;

				if  (t > 0 && t < dist) {
					//if (id == 0 && i == 3) printf("%f, %d", t, i );
					blocked = true;
					break;
				}
			}	
		}
		if (!blocked){
			float cosine = max(0.0f, dot(ray.D, normal));
			float dis_square = (dist * dist);
			light_contribution += (lights[l].intensity * cosine)/ dis_square;
		}

	}
	ac[ray.id] = ac[ray.id] * light_contribution;

	//printf("boop\n");
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



__kernel void test_kern( global struct Ray *rays ){

	int id = get_global_id(0);

	//a[id] = 10.0;



	struct Ray ray;

	ray.id = id;

	printf("ID: %d\n", ray.id);
}