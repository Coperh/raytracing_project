#pragma once
#include "cmath"
#include "Light.h"
#include "Primitive.h"

using std::pow, std::sqrt;


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




float DirectIllumination(Primitive * objects[], int n, Light light, float3 interseciton, float3 normal)
{

	float3 D = normalize(light.pos - interseciton);

	


	float distance = sqrt(pow(interseciton.x - light.pos.x, 2) + pow(interseciton.y - light.pos.y, 2) + pow(interseciton.z - light.pos.z, 2));


	Ray ray = { interseciton, D, distance };




	//printf("1: %f %f %f\n", ray.D.x, ray.D.y, ray.D.z);


	if (AnyIntersection(objects, n, ray)) return 0;

	//printf("I: %f %f %f, D: %f\n", interseciton.x, interseciton.y, interseciton.z, distance);


	if (distance <= 10) return light.intensity;


	return light.intensity * (10 / pow(distance,2));

}


void NearestIntersection(Primitive * objects[], int n, Ray* r, float3* intersection, float3* normal, Material* material) {

	r->t = -1;

	int nearest = -1;

	for (int i = 0; i < n; i++)
	{
		if (objects[i]->Intersect(r)) {
			nearest = i;
			material->type = objects[i]->mat.type;
			material->colour = objects[i]->mat.colour;
		}
	}

	// if intersected

	if (r->t > 0) {

		float3 I = r->O + r->D * r->t;



		intersection->x = I.x;
		intersection->y = I.y;
		intersection->z = I.z;

		float3 N = normalize(I - objects[nearest]->pos);

		normal->x = N.x;
		normal->y = N.y;
		normal->z = N.z;
	}

	//printf("%f\n", r->t);
}


float3 Trace(Light light, Primitive * objects[], int n, Ray ray) {

	float3 N;
	float3 I;
	Material mat;
	float3 colour;


	

	NearestIntersection(objects, n, &ray, &I, &N, &mat);

	if (ray.t < 0)
		return float3(0, 0, 0);

	//printf("I: %f %f %f\n", I.x, I.y, I.z);

	if (mat.type == Material::Type::diffuse) {
		colour = mat.colour * DirectIllumination(objects, n, light, I, N);
	
		//printf("Col %f %f %f\n", colour.x, colour.y, colour.z);

		return colour;
	}

	else if (mat.type == Material::Type::mirror);
	else if (mat.type == Material::Type::glass);

	return float3(0, 0, 0);
}


