
#include "precomp.h"




const int num_samples = 1;

const float3 sky(0.039, 0.407, 0.741);
const float3 black(0, 0, 0);
const float3 white(1, 1, 1);

const int checker_scale = 2;



float3 Pathtrace::DiffuseReflection(Primitive* objects[], int n, AreaLight* lights[], int m, float3 diffuse, float3 interseciton, float3 normal)
{



	float3 colour(1,1,1);




	for (int i = 0; i < m; i++) {

		float3 colour_from_light(0, 0, 0);

		float3 randPoints[num_samples];

		lights[i]->GenPoints(randPoints, num_samples);

		for (int j = 0; j < num_samples; j++) {



			//printf("Rand: %f %f %f\n", randPoints[i].x, randPoints[i].y, randPoints[i].z);



			float3 L = randPoints[i] - interseciton;


			//float3 L = normalize(raw_vector);


			float distance = length(L);


			L = L / distance;

			float cos_o = dot(-L, lights[i]->N);
			float cos_i = dot(L, normal);

			if (!((cos_o <= 0) || (cos_i <= 0))) {

				Ray ray = { interseciton + L * EPSILON, L, distance };

				if (AnyIntersection(objects, n, ray)) {
					colour += black;
				}
				else {
					float3 BRDF = diffuse * INVPI;
					float solidAngle = (cos_o * lights[i]->area) / (distance * distance);


					colour_from_light += BRDF * m * solidAngle * cos_i * lights[i]->mat.colour * lights[i]->mat.intensity;
				}
			}
			else colour += black;
		}

		colour = (colour_from_light / num_samples) ;
	}
	return colour;
}



float3 Sample(Primitive* objects[], int n, AreaLight* lights[], int m, Ray ray, int bounce) {


	bounce++;

	float3 N;
	float3 I;
	float3 colour;

	//printf("b%d\n", bounce);

	int object = NearestIntersection(objects, n, &ray, &I, &N);



	if (object < 0)
		return sky;

	Material mat = objects[object]->mat;







	if (mat.type == Material::Type::light) {
		return mat.colour;
	}

	if (mat.type == Material::Type::reflect) {

		float3 r = ReflectRay(ray.D, N);

		Ray reflected = { I + r * EPSILON, r, -1 };

		return Sample(objects, n, lights, m, reflected, bounce);

	}




	colour = mat.colour;



	if (mat.is_checker) {


		float3 scaled_pos = I / checker_scale;

		int check = int(scaled_pos.x) + int(scaled_pos.y) + int(scaled_pos.z);

		if (check % 2 == 0){
			colour = white;
		}
	}



	// need to intersect light aswell
	
	if (mat.type == Material::Type::diffuse) {
	
		colour = Pathtrace::DiffuseReflection(objects,  n, lights, m, colour, I, N);
	}


	return colour;
}



