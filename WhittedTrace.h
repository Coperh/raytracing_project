#pragma once
#include "cmath"
#include "Light.h"
#include "Primitive.h"
#include "Intersection.h"


using std::pow;



const float3 sky(0, 0, 0);






float DirectIllumination(Primitive * objects[], int n, Light lights[], int m, float3 interseciton, float3 normal)
{

	float intensity = 0;


	for (int i = 0; i < m; i++) {

		//printf("%d\n", i);

		float3 raw_vector = lights[i].pos - interseciton;


		float3 D = normalize(raw_vector);

	
		float distance = length(raw_vector);


		Ray ray = { interseciton + D*EPSILON, D, distance };


		//printf("distance %f\n", distance);

		//printf("1: %f %f %f\n", ray.D.x, ray.D.y, ray.D.z);

	
		if (!AnyIntersection(objects, n, ray)){

			//printf("test\n");

			float cosine = max(0.0f, dot(D, normal));

			float dis_square = (distance * distance);

			//printf("distance %f\n", distance);


			//if (distance <= 10) intensity += light.intensity;


			intensity += (lights[i].intensity * cosine)/ dis_square ;

			//printf("%f %f\n ", cosine, dis_square);
			//printf("I: %f %f %f, D: %f\n", interseciton.x, interseciton.y, interseciton.z, distance);
	
		
		}
	
		
	}

	return intensity;
}



float3 WhittedTrace(Primitive* objects[], int n, Light  lights[], int m, Ray ray, int bounce) {

	bounce++;

	float3 N;
	float3 I;
	float3 colour;

	//printf("b%d\n", bounce);

	int i = NearestIntersection(objects, n, &ray, &I, &N);

	if (i < 0)
		return float3(0, 0, 0);


	//printf("I: %f, %f, %f\n", I.x, I.y, I.z);
	//printf("N: %f, %f, %f\n", N.x, N.y, N.z);


	Material mat = objects[i]->mat;

	//printf("Mat Type %d\n", mat.type);

	if (mat.is_checker) {

	
		int check = int(I.x) + int(I.z);

		if (check % 2 == 0) colour = float3(255, 255, 255);
		else colour = mat.colour;
	}
	else { 
		
		colour = mat.colour; }


	//printf("bounce %d\n", bounce);

	if (mat.type == Material::Type::diffuse || bounce  > 10) {


		float dir = DirectIllumination(objects, n, lights, m, I, N);

		//printf("%f\n", dir);

		//printf("Col %f %f %f\n", colour.x, colour.y, colour.z);

		return colour * dir;
	}

	else {
		
		float s = mat.intensity;

		float d = 1 - s;

		float3 R = ReflectRay(ray.D, N);

		Ray reflect = { I + R * EPSILON,R, -1 };


		if (mat.type == Material::Type::reflect) {

			float reflected = mat.intensity;

			if (s == 0) 
				return colour * DirectIllumination(objects, n, lights, m, I, N);

			if (s == 1) 
				return WhittedTrace(objects, n, lights, m, reflect, bounce);
			else {

				colour = (s * WhittedTrace(objects, n, lights, m, reflect, bounce)) +
					(d * colour * DirectIllumination(objects, n, lights, m, I, N));
				return colour;
			}
			 
		}


		// not fully working

		else if (mat.type == Material::Type::refract)
		{



			//printf("Refract");



			if (s == 0)
				return colour * DirectIllumination(objects, n, lights, m, I, N);


			// refraction

			float cosine = dot(N, -ray.D);
			float k = 1 - pow(mat.index, 2) * (1 - pow(cosine, 2));

			float3 refracted_colour = mat.colour;


			//printf("Start Refraction\n");

			if (k >= 0)
			{

				//printf("kay yay\n");	
				float3 T = mat.index * ray.D + N * (mat.index * cosine - sqrtf(k));
				Ray refracted = { I + T * EPSILON, T, -1 };

				float3 beer = -(mat.colour * ray.t);

				beer = float3(1, 1, 1);


				refracted_colour = WhittedTrace(objects, n, lights, m, refracted, bounce);

				//refracted_colour.x = pow(refracted_colour.x, beer.x);
				//refracted_colour.y = pow(refracted_colour.y, beer.y);
				//refracted_colour.z = pow(refracted_colour.x, beer.z);
			

			}
			else {
				refracted_colour = refracted_colour * DirectIllumination(objects, n, lights, m, I, N);
			}

			//printf("Done Refraction\n");

			if (s == 1) return refracted_colour;


			float3 colour = (s * refracted_colour) * (d * WhittedTrace(objects, n, lights, m, reflect, bounce));




			//printf("point 1\n");




			return colour;
		};




	}


	return sky;
}



