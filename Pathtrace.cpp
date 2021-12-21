
#include "precomp.h"




const int num_samples = 1;

const float3 black(1e-4, 1e-4, 1e-2);
const float3 white(1, 1, 1);





float3 Pathtrace::DirectIllumination(Primitive* objects[], int n, AreaLight* lights[], int m, float3 diffuse, float3 interseciton, float3 normal)
{



	float3 colour(1,1,1);




	for (int i = 0; i < m; i++) {

		float3 colour_from_light(0, 0, 0);

		float3 randPoints[num_samples];

		lights[i]->GenPoints(randPoints, num_samples);

		for (int j = 0; j < num_samples; j++) {


			float3 L = randPoints[i] - interseciton;


			//float3 L = normalize(raw_vector);


			float distance = length(L);


			L = L / distance;

			float cos_o = dot(-L, lights[i]->N);
			float cos_i = dot(L, normal);

			if ( !((cos_o <= 0) || (cos_i <= 0)) ){

				Ray ray = { interseciton + L * EPSILON, L, distance };

				if (!AnyIntersection(objects, n, ray)) {
					
					float3 BRDF = diffuse * INVPI;
					float solidAngle = (cos_o * lights[i]->area) / (distance * distance);
				

					colour_from_light = BRDF * m * solidAngle * cos_i * lights[i]->mat.colour  *lights[i]->mat.intensity;
				}
			}
		}

		colour *= (colour_from_light / num_samples) ;
	}
	return colour;
}



float3 Pathtrace(Primitive* objects[], int n, AreaLight* lights[], int m, Ray ray, int bounce) {


	bounce++;

	float3 N;
	float3 I;
	float3 colour;

	//printf("b%d\n", bounce);

	int object = NearestIntersection(objects, n, &ray, &I, &N);



	if (object < 0)
		return black;

	Material mat = objects[object]->mat;


	colour = mat.colour;


	if (mat.is_checker) {
		int check = int(I.x) + int(I.z);
		float quad = I.x * I.z;
		if (check % 2 == 0){
			
			if (!(quad > 0)) colour = white;
		}
		else {
			if (quad > 0) colour = white;
		}
	}



	// need to intersect light aswell
	if (mat.type == Material::Type::light) {
		colour *= mat.intensity;
	}
	else if (mat.type == Material::Type::diffuse) {
	
		colour = Pathtrace::DirectIllumination(objects,  n, lights, m, colour, I, N);
	}


	return colour;
}



