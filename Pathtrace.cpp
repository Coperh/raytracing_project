
#include "precomp.h"



void Pathtrace::DirectIllumination(Primitive* objects[], int n, Primitive* lights[], int m, float3 interseciton, float3 normal) 
{





}



float3 Pathtrace(Primitive* objects[], int n, Primitive* lights[], int m, Ray ray, int bounce) {


	bounce++;

	float3 N;
	float3 I;
	float3 colour;

	//printf("b%d\n", bounce);

	int i = NearestIntersection(objects, n, &ray, &I, &N);



	if (i < 0)
		return float3(0, 0, 0);

	Material mat = objects[i]->mat;

	if (mat.is_checker) {
		int check = int(I.x) + int(I.z);
		float quad = I.x * I.z;
		if (check % 2 == 0){
			
			if (quad > 0) colour = float3(255, 255, 255);
			else colour = mat.colour;
		}
		else {
			if (quad > 0)
				colour = mat.colour;
			else colour = float3(255, 255, 255);
		}
	}
	else {

		colour = mat.colour;
	}




	if (mat.type == Material::Type::diffuse) {
	
		Pathtrace::DirectIllumination(objects,  n, lights, m, I, N);
	}




	return colour;
}



