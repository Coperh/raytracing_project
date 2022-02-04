#pragma once

#include "cl_structs.h"

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


void get_normal(struct Primitive* prim, struct Intersection * incs)
{
	

	if (dot(prim->N, incs->D) > 0)
		incs->N = -prim->N;
	else
		incs->N = prim->N;


}
