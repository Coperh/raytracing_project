#pragma once
#include "cmath"
#include "Light.h"
#include "Primitive.h"



using std::pow;


const float EPSILON = 1e-4;


float3 ReflectRay(float3 D, float3 N) {
	return D - 2 * (dot(D, N) * N);
}



bool AnyIntersection(Primitive * objects[], int n, Ray ray) {

	//printf("2: %f %f %f\n", ray.D.x, ray.D.y, ray.D.z);

	for (int i = 0; i < n; i++)
	{
		//printf("Sphape: %d, T:%f\n",i, r.t);
		
		// only returns true if there is an intersection nearer than the lightsource
		if (objects[i]->Intersect(&ray)) {
			
			return true;
		}
	}
	return false;
}




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



			float cosine = max(0.0f, dot(D, normal));

			float dis_square = (distance * distance);

			//if (distance <= 10) intensity += light.intensity;


			intensity += (lights[i].intensity * cosine)/ dis_square ;

			//printf("%f %f\n ", cosine, dis_square);
			//printf("I: %f %f %f, D: %f\n", interseciton.x, interseciton.y, interseciton.z, distance);
	
		
		}
	
		
	}

	return intensity;
}


void NearestIntersection(Primitive * objects[], int n, Ray* r, float3* intersection, float3* normal, Material* material) {

	r->t = -1;

	int nearest = -1;




	for (int i = 0; i < n; i++)
	{
		//printf("boop\n");
		
		if (objects[i]->Intersect(r)) {
			nearest = i;
			
			
			material->type = objects[i]->mat.type;
			material->colour = objects[i]->mat.colour;
			material->intensity = objects[i]->mat.intensity;
			material->is_checker = objects[i]->mat.is_checker;
		}
	}

	// if intersected

	if (r->t > 0) {

		float3 I = r->O + r->D * r->t;

		intersection->x = I.x;
		intersection->y = I.y;
		intersection->z = I.z;

		float3 N = objects[nearest]->GetNormal(I, r->D);

		normal->x = N.x;
		normal->y = N.y;
		normal->z = N.z;
	}

	//printf("%f\n", r->t);
}


float3 Trace(Primitive* objects[], int n, Light  lights[], int m, Ray ray, int bounce, bool is_refracted = false) {

	float3 N;
	float3 I;
	Material mat;
	float3 colour;

	//printf("Bounce %d\n", bounce);

	NearestIntersection(objects, n, &ray, &I, &N, &mat);

	if (ray.t < 0)
		return float3(0, 0, 0);

	//printf("I: %f %f %f\n", I.x, I.y, I.z);



	if (mat.is_checker) {

	
		int check = int(I.x) + int(I.z);

		if (check % 2 == 0) colour = float3(255, 255, 255);
		else colour = mat.colour;
	}
	else { 
		
		colour = mat.colour; }



	if (mat.type == Material::Type::diffuse || bounce  > 4) {
		//printf("Col %f %f %f\n", colour.x, colour.y, colour.z);

		return colour * DirectIllumination(objects, n, lights, m, I, N);;
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



			

			if (s == 1) return Trace(objects, n, lights, m, reflect, bounce++);
			else {

				colour = (s * Trace(objects, n, lights, m, reflect, bounce++)) +
					(d * colour * DirectIllumination(objects, n, lights, m, I, N));
				return colour;
			}
			 
		}

		else if (mat.type == Material::Type::refract) 
		{



			//printf("Refract");



			if (s == 0) 
				return colour * DirectIllumination(objects, n, lights, m, I, N);
				



			// refraction

			float cosine = dot(N,  -ray.D);

			float k = 1 - pow(mat.index, 2) * (1 - pow(cosine, 2));

			float3 refracted_colour = mat.colour;


			//printf("Start Refraction\n");

			if (k >= 0)
			{

				float3 T = mat.index * ray.D + N * (mat.index * cosine - sqrtf(k));

				Ray refracted = { I + T * EPSILON, T, -1 };




				if (is_refracted) {
					float3 beer = -(mat.colour * ray.t);

					//beer = float3(1, 1, 1);


					refracted_colour = Trace(objects, n, lights, m, refracted, bounce++, false);

					refracted_colour.x = pow(refracted_colour.x, beer.x);
					refracted_colour.y = pow(refracted_colour.y, beer.y);
					refracted_colour.z = pow(refracted_colour.x, beer.z);
				}
				// does not work on inner triangle

				refracted_colour = Trace(objects, n, lights, m, refracted, bounce++, true);

			}
			else {
				refracted_colour = refracted_colour * DirectIllumination(objects, n, lights, m, I, N);
			}

			//printf("Done Refraction\n");

			if (s == 1) return refracted_colour;


			float3 colour = (s * refracted_colour) * (d * Trace(objects, n, lights, m, reflect, bounce++));




			//printf("point 1\n");



			
			return colour;
		};
	
	
	

	}

	return float3(0, 0, 0);
}



