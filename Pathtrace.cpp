
#include "precomp.h"




const int num_samples = 1;

const float3 sky(0.039, 0.407, 0.741);
const float3 black(0, 0, 0);
const float3 white(1, 1, 1);

const int checker_scale = 2;



float3 Pathtrace::DiffuseReflection(Primitive* objects[], int n, AreaLight* lights[], int m, float3 diffuse, float3 interseciton, float3 normal){







	// chose random light

	int i = RandomUInt() % m;


	//printf("%d %d %d\n", m,  i, RandomUInt());


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
				colour_from_light += black;
			}
			else {
				float3 BRDF = diffuse * INVPI;
				float solidAngle = (cos_o * lights[i]->area) / (distance * distance);


				colour_from_light += BRDF * m * solidAngle * cos_i * lights[i]->mat.colour;
			}
		}
		else colour_from_light += black;
	}

	return colour_from_light / num_samples;
}



float3 Sample(Primitive* objects[], int n, AreaLight* lights[], int m, Ray ray, int bounce) {


	bounce++;


	if (bounce > 10) return black;






	float3 N;
	float3 I;
	float3 colour;

	//printf("b%d\n", bounce);

	int object = NearestIntersection(objects, n, &ray, &I, &N);

	//printf("%d\n", object);

	if (object < 0)
		return sky;

	Material mat = objects[object]->mat;



	colour = mat.colour;



	if (mat.type == Material::Type::light) {
		return colour;
	}


	float3 r = ReflectRay(ray.D, N);
	Ray reflected = { I + r * EPSILON, r, -1 };

	if (mat.type == Material::Type::reflect) {

		

		return Sample(objects, n, lights, m, reflected, bounce) * colour ;

	}


	if (mat.type == Material::Type::refract) {

	

		Ray refracted{I,ray.D, -1};


		if (objects[object]->Refract(&refracted)) {
		
			//printf("Reft: %f\n", refracted.t);


			float3 beer = -(colour * refracted.t);

			refracted.t = -1;
			refracted.O = refracted.O + refracted.D * EPSILON;



			//printf("O: %f %f %f\n", refracted.O.x, refracted.O.y, refracted.O.z);
			//printf("D: %f %f %f\n", refracted.D.x, refracted.D.y, refracted.D.z);


			//printf("Refracted: ");
			float3 refracted_colour = Sample(objects, n, lights, m, refracted, bounce);

			refracted_colour.x = pow(refracted_colour.x, beer.x);
			refracted_colour.y = pow(refracted_colour.y, beer.y);
			refracted_colour.z = pow(refracted_colour.z, beer.z);


			return refracted_colour;
		}
		else return Sample(objects, n, lights, m, reflected, bounce) * colour;
	
	}



	



	if (mat.is_checker) {


		float3 scaled_pos = floorf(I / checker_scale);

		int check = scaled_pos.x + scaled_pos.y + scaled_pos.z;


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



