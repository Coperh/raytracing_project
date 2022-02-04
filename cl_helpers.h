#pragma once

#include "cl_structs.h"





float intersect_sphere(struct Primitive* sphere, struct Ray* ray)
{

	//printf("boop\n");


	//printf("point 2\n");

	float3 sub = ray->O - sphere->pos;
	float distance_squared = dot(sub, sub);

	float t = -1;

	//printf("distance_squared %f  sphere->o  %f\n", distance_squared, sphere->o );

	// if distance squared greater than radius squared, outside of circle
	if (distance_squared > sphere->o) {




		float3 C = sphere->pos - ray->O;
		t = dot(C, ray->D);


		float3 Q = C - t * ray->D;
		float p2 = dot(Q, Q);

		if (p2 > sphere->o ) return -1; // sphere->o  = r * r


		t -= sqrt(sphere->o  - p2);


	}
	else {
		// else inside

		//printf("boop\n");

		float a = dot(ray->D, ray->D);

		float3 OminC = ray->O - sphere->pos;

		float b = dot(2 * ray->D, OminC);

		float c = dot(OminC, OminC) - sphere->o ;


		float root_arg = (b * b) - (4 * a * c);

		//printf("point 4\n");

		// negative, not in circle
		if (root_arg < 0) return -1;

		float root = sqrt(root_arg);

		float a2 = a * 2;

		float t1 = (-b + root) / a2;

		float t2 = (-b - root) / a2;


		//printf("2ts: %f, %f\n", t1, t2);

		//printf("point 5\n");


		if (t1 < 0)
			t = t2;
		else if (t2 < 0)
			t = t1;
		else t = fmin(t1, t2);

	}

	//printf("%f\n", t);

	return t;
}




float intersect_plane(struct Primitive * plane, struct Ray * ray)
{

	float denom = dot(plane->N, ray->D);

	if (denom != 0)
	{
		float num = -(dot(ray->O, plane->N) + plane->o);
		float t = num / denom;


		return t;
	}
	return -1;
}




float intersect(struct Primitive* prim, struct Ray* ray) {

	if (prim->type == 0) return intersect_plane(prim, ray);

	else if (prim->type == 1) return intersect_sphere(prim, ray);
	return -1;
	
}





void get_normal(struct Primitive* prim, struct Intersection * incs)
{
	

	if (prim->type == 0) {

		if (dot(prim->N, incs->D) > 0)
			incs->N = -prim->N;
		else
			incs->N = prim->N;
	}
	else if (prim->type == 1){

		incs->N = normalize(incs->I - prim->pos);

		//printf("%f %f %f\n", incs->N.x, incs->N.y, incs->N.z);
	}
}

